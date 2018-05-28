#pragma once

#include "res.h"

namespace valkyrie
{
	class NetvarReader
	{
	public:
		class RecvTable
		{
		private:
			string className;
			string parent;
			mutable unordered_map<string, uint32_t> propNameToOffset;
		public:
			RecvTable(string const& name);
			RecvTable() {}

			auto setName(string const& name) -> void;
			auto setParent(string const& name) -> void;
			auto getParent() const -> string const&;
			auto addProp(string const& name, uint32_t off) -> void;
			auto getPropOffset(string const& propName) const -> uint32_t;
			auto propExists(string const& propName) const -> bool;
			auto printProps() const -> void;
		};

		NetvarReader();

		auto readNetvarTable() -> void;

		auto printTables() const -> void;

		constexpr auto tablesRead() const -> bool;

		auto getTable(string const& className) const -> RecvTable const&
		{
			return classNameToTable.at(className);
		}

		auto getClassName(int classID) const -> string const&
		{
			return classIDToName.at(classID);
		}

	private:

		auto parseRecvTable(uint32_t tableAddress, uint32_t classID, string const& parent) -> void;
		
		unordered_map<string, RecvTable> classNameToTable;
		unordered_map<uint32_t, string> classIDToName;

		bool _tablesRead;
	};
}