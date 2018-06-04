#pragma once

#include "res.h"

namespace valkyrie
{
	typedef BOOL(__stdcall *HandleCloser_t)(HANDLE);
	using WindowsHandle = unique_ptr<std::remove_pointer_t<HANDLE>, HandleCloser_t>;

	//Snapshot of a module
	struct Module
	{
		Module(string const& name, uint32_t baseAddress, size_t moduleSize)
			: name(name), baseAddress(baseAddress), moduleSize(moduleSize), memory(nullptr) {}

		Module(Module&& other) 
			: name(std::move(other.name)), memory(std::move(other.memory)),
			baseAddress(other.baseAddress), moduleSize(other.moduleSize) {}

		Module()
			: name(""), memory(nullptr), baseAddress(0), moduleSize(0) {}

		//no copying
		Module(Module const& o) = delete;

		string name;
		unique_ptr<uint8_t[]> memory;

		//32bit addr space
		uint32_t baseAddress;
		size_t moduleSize;

		//no copying
		auto operator=(Module const& o) -> Module& = delete;

		//preserve memory
		auto operator=(Module&& o) -> Module&
		{
			name = std::move(o.name);
			memory = std::move(o.memory);
			baseAddress = std::move(o.baseAddress);
			moduleSize = std::move(o.moduleSize);

			return *this;
		}
	};

	//Process memory read management
	class ProcessMgr32
	{
	private:
		//using windows types here for reasons
		WindowsHandle handle;
		DWORD pID;

		map<string, Module> moduleMap;

		static map<string, DWORD> pidMap;

		auto readModule(Module& m) -> void;

		static auto sigScanModule(string const& signature, Module const& m) -> uint32_t;

	public:
		static auto getProcessIDList(bool refresh)->map<string, DWORD> const&;
		static auto checkProcessExists(string const& name, const bool refresh) -> bool;

		ProcessMgr32();
		ProcessMgr32(ProcessMgr32 const&) = delete;

		auto openProcessById(const DWORD pID) -> bool;
		auto isInitialized() const -> const bool;

		auto moduleExists(string const& name) const -> bool;

		//loads modules and copies their memory
		auto loadModules() -> void;
		auto getModule(string const& name) const -> Module const&;

		////r/w, returns true on success
		//auto read(const uint32_t address, uintptr_t buffer, const size_t size) -> bool;
		//auto write(const uint32_t address, uintptr_t buffer, const size_t size) -> bool;

		template<typename T>
		auto read(const uint32_t address, T* buffer, const size_t length) -> bool
		{
			size_t bytesRead = 0;
			return ReadProcessMemory(handle.get(), reinterpret_cast<LPVOID>(address), reinterpret_cast<LPVOID>(buffer), length * sizeof(T), &bytesRead);
		}
		template<typename T>
		auto write(const uint32_t address, T* buffer, const size_t length) -> bool
		{
			size_t bytesWritten = 0;
			return WriteProcessMemory(handle.get(), reinterpret_cast<LPVOID>(address), reinterpret_cast<LPCVOID>(buffer), length * sizeof(T), &bytesWritten);
		}

		template<typename T>
		auto read(const uint32_t address) -> T
		{
			T buffer;
			read(address, &buffer, 1ull);
			return buffer;
		}

		//returns badAddr if sig wasn't found
		auto sigScan(string const& signature, string const& searchModule) const -> uint32_t;

		auto isProcessRunning() const -> bool;

		auto operator=(ProcessMgr32 const& o) -> ProcessMgr32& = delete;
		~ProcessMgr32() = default;
	};

	extern ProcessMgr32 csgoProc;
}