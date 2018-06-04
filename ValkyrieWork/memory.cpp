#include "memory.h"
#include "valkAPI.h"
#include <Psapi.h>
#include <TlHelp32.h>

namespace valkyrie
{
	//global instance for mem mgmt
	ProcessMgr32 csgoProc;


	static auto getByte(const char* byteStr) -> uint8_t
	{
		const auto getNibble = [](const char c) -> uint8_t
		{
			if (c & '0') { return c - '0'; }
			if (c & '@') { return c - ('A' - 10i8); }
			return 0;
		};
		return (getNibble(byteStr[0]) << 4) | getNibble(byteStr[1]);
	}

	static auto enumModules(const HANDLE handle, 
		std::function<bool(MODULEINFO const& modInfo, TCHAR modName[MAX_PATH])> forEach)
	{
		HMODULE modules[1024];
		DWORD needed;
		if (EnumProcessModulesEx(handle, modules, sizeof(modules), &needed, LIST_MODULES_ALL))
		{
			for (uint32_t a = 0; a < (needed / sizeof(HMODULE)); a++)
			{
				TCHAR modNameTC[MAX_PATH];

				if (GetModuleFileNameEx(handle, modules[a], modNameTC, sizeof(modNameTC) / sizeof(TCHAR)))
				{
					MODULEINFO modInfo;
					if (GetModuleInformation(handle, modules[a], &modInfo, sizeof(MODULEINFO)))
					{
						if (forEach(modInfo, modNameTC))
						{
							return;
						}
					}
				}
			}
		}
	}

	//precondition: mask is in format XX XX XX XX, length is 3*numBytes-1
	static auto patternMatch(unique_ptr<uint8_t[]> const& data, string const& mask, const size_t dataSize) -> uint32_t
	{
		vector<std::pair<uint8_t, bool>> bytesWithMask;
		uint8_t* sigAddress = nullptr;
		
		bytesWithMask.reserve((mask.length() + 1) / 3);
		for (uint32_t mb = 0; mb < mask.length(); mb += 3)
		{
			bytesWithMask.emplace_back(std::pair<uint8_t, bool>(getByte(&mask[mb]),
				mask[mb] == '?'));
		}

		sigAddress = std::search(data.get(), data.get() + dataSize, bytesWithMask.begin(), bytesWithMask.end(),
		[](uint8_t left, std::pair<uint8_t, bool> right) -> bool
		{
			return right.second || left == right.first;
		});

		if (sigAddress == data.get() + dataSize)
		{
			return badAddr;
		}

		return static_cast<uint32_t>(sigAddress - data.get());
	}

	auto ProcessMgr32::getProcessIDList(bool refresh) -> map<string, DWORD> const&
	{
		if (!refresh)
		{
			return pidMap;
		}

		pidMap.clear();

		PROCESSENTRY32 pe32 = { 0 };
		pe32.dwSize = sizeof(PROCESSENTRY32);

		WindowsHandle snapshot = WindowsHandle(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0), CloseHandle);
		if (snapshot.get() == INVALID_HANDLE_VALUE || snapshot.get() == 0)
		{
			return pidMap;
		}

		if (!Process32First(snapshot.get(), &pe32))
		{
			return pidMap;
		}
		do
		{
			pidMap[pe32.szExeFile] = pe32.th32ProcessID;
		} while (Process32Next(snapshot.get(), &pe32));

		return pidMap;
	}

	map<string, DWORD> ProcessMgr32::pidMap;

	ProcessMgr32::ProcessMgr32() 
		: handle(INVALID_HANDLE_VALUE, CloseHandle), pID(0u)
	{
		//initialize bad module
		moduleMap.emplace(std::make_pair<string, Module>(string("$modulenotfound"), Module("$modulenotfound", badAddr, 0)));
	}

	auto ProcessMgr32::checkProcessExists(string const& name, const bool refresh) -> bool
	{
		getProcessIDList(refresh);

		return pidMap.find(name) != pidMap.end();
	}

	auto ProcessMgr32::openProcessById(const DWORD pID) -> bool
	{
		this->pID = pID;
		handle = std::move(WindowsHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID), CloseHandle));

		return isInitialized();
	}

	auto ProcessMgr32::readModule(Module& m) -> void
	{
		m.memory = std::make_unique<uint8_t[]>(m.moduleSize);
		if (!read<uint8_t>(m.baseAddress, m.memory.get(), m.moduleSize))
		{
			m.memory.reset();
		}
	}

	auto ProcessMgr32::moduleExists(string const& name) const -> bool
	{
		bool exists = false;
		enumModules(handle.get(), [&exists, &name](auto const& moduleInfo, TCHAR modNameTC[MAX_PATH]) -> bool
		{
			string modName = string(modNameTC);
			modName = modName.substr(modName.find_last_of("/\\") + 1);
			return exists = (modName == name);
		});
		return exists;
	}

	void ProcessMgr32::loadModules()
	{
		auto& mmapRef = moduleMap;
		enumModules(handle.get(), [&mmapRef, this](auto const& moduleInfo, TCHAR modNameTC[MAX_PATH]) -> bool
		{
			string modName = string(modNameTC);
			modName = modName.substr(modName.find_last_of("/\\") + 1);
			Module newMod(modName, reinterpret_cast<uint32_t>(moduleInfo.lpBaseOfDll), moduleInfo.SizeOfImage);
			this->readModule(newMod);
			mmapRef[modName] = std::move(newMod);
			return false;
		});
	}

	auto ProcessMgr32::getModule(string const& name) const -> Module const&
	{
		if (moduleMap.find(name) != moduleMap.end())
		{
			return moduleMap.at(name);
		}
		else
		{
			return moduleMap.at("$modulenotfound");
		}
	}

	auto ProcessMgr32::isInitialized() const -> const bool
	{
		return handle.get() != INVALID_HANDLE_VALUE && pID != 0u;
	}

	auto ProcessMgr32::sigScanModule(string const& signature, Module const& m) -> uint32_t
	{
		return patternMatch(m.memory, signature, m.moduleSize);
	}

	auto ProcessMgr32::sigScan(string const& signature, string const& searchModule) const -> uint32_t
	{
		//32 bit men
		uint32_t sigAddress = badAddr;
		auto&& kvIter = moduleMap.find(searchModule);
		if (kvIter != moduleMap.end())
		{
			if ((sigAddress = sigScanModule(signature, kvIter->second)) != badAddr)
			{
				sigAddress += kvIter->second.baseAddress;
			}
		}
		return sigAddress;
	}

	auto ProcessMgr32::isProcessRunning() const -> bool
	{
		const auto checkExitCode = [](WindowsHandle const& handle) -> bool
		{
			DWORD exitCode = 0;
			BOOL success = GetExitCodeProcess(handle.get(), &exitCode);
			return success && (exitCode == STILL_ACTIVE);
		};

		return isInitialized() && checkExitCode(handle);
	}
}