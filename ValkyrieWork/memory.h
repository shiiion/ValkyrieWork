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
			baseAddress = o.baseAddress;
			moduleSize = o.moduleSize;

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
		static auto checkProcessExists(string const& name, bool refresh) -> bool;

		ProcessMgr32();
		ProcessMgr32(ProcessMgr32 const&) = delete;

		auto openProcessById(DWORD pID) -> bool;
		auto isInitialized() const -> bool;

		//loads modules and copies their memory
		auto loadModules() -> void;
		auto getModule(string const& name) const -> Module const&;

		//r/w, returns true on success
		auto read(uint32_t address, uintptr_t buffer, size_t size) -> bool;
		auto write(uint32_t address, uintptr_t buffer, size_t size) -> bool;

		template<typename T>
		auto read(uint32_t address, T* buffer, size_t length) -> bool;
		template<typename T>
		auto write(uint32_t address, T* buffer, size_t length) -> bool;

		//returns badAddr if sig wasn't found
		auto sigScan(string const& signature, string const& searchModule) const -> uint32_t;

		auto isProcessRunning() const -> bool;

		auto operator=(ProcessMgr32 const& o) -> ProcessMgr32& = delete;
		~ProcessMgr32() = default;
	};

	extern ProcessMgr32 csgoProc;
}