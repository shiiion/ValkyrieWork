#include "netvarreader.h"
#include "globals.h"
#include "valkAPI.h"
#include "memory.h"

namespace valkyrie
{
	NetvarReader::RecvTable::RecvTable(string const& name) : className(name) {}

	auto NetvarReader::RecvTable::setName(string const& name) -> void
	{
		className = name;
	}

	auto NetvarReader::RecvTable::setParent(string const& name) -> void
	{
		parent = name;
	}

	auto NetvarReader::RecvTable::getParent() const -> string const&
	{
		return parent;
	}

	auto NetvarReader::RecvTable::addProp(string const& name, uint32_t off) -> void
	{
		propNameToOffset[name] = off;
	}

	auto NetvarReader::RecvTable::getPropOffset(string const& propName) const -> uint32_t
	{
		return propNameToOffset[propName];
	}

	auto NetvarReader::RecvTable::propExists(string const& propName) const -> bool
	{
		return propNameToOffset.find(propName) != propNameToOffset.end();
	}

	auto NetvarReader::RecvTable::printProps() const -> void
	{
		for (auto const& pair : propNameToOffset)
		{
			cout << "|  prop " << pair.first << " offset " << std::hex << pair.second << endl;
		}
	}

	//---------------------------------------

	NetvarReader::NetvarReader() : _tablesRead(false) {}

	auto NetvarReader::readNetvarTable() -> void
	{
		uint32_t tableHead = globals.dataTable;
		if (!tableHead)
		{
			PRINTLN_LOG("Failed to read netvar table, dataTable pointer null");
			return;
		}

		while (tableHead)
		{
			uint32_t recvTable, classID;
			csgoProc.read<uint32_t>(tableHead + 0x0C, &recvTable, 1);
			csgoProc.read<uint32_t>(tableHead + 0x14, &classID, 1);

			if (classIDToName.find(classID) == classIDToName.end())
			{
				parseRecvTable(recvTable, classID, "");
			}

			csgoProc.read<uint32_t>(tableHead + 0x10, &tableHead, 1);
		}

		_tablesRead = true;
	}

	auto NetvarReader::printTables() const -> void
	{
		for (auto const& pair : classNameToTable)
		{
			string const& parent = pair.second.getParent();
			if (parent == "")
			{
				cout << "Table " << pair.first.c_str() << ":"  << endl;
			}
			else
			{
				cout << "Table " << pair.first.c_str() << " child of " << parent.c_str() << ":" << endl;
			}
			
			pair.second.printProps();
		}
	}

	constexpr auto NetvarReader::tablesRead() const -> bool
	{
		return _tablesRead;
	}

	auto NetvarReader::parseRecvTable(uint32_t tableAddress, uint32_t classID, string const& parent) -> void
	{
		constexpr uint32_t propSize = 0x3C;

		uint32_t recvPropBase, numProps, tableName;
		csgoProc.read<uint32_t>(tableAddress, &recvPropBase, 1);
		csgoProc.read<uint32_t>(tableAddress + 0x04, &numProps, 1);
		csgoProc.read<uint32_t>(tableAddress + 0x0C, &tableName, 1);

		char tableNameOut[256];
		csgoProc.read(tableName, reinterpret_cast<uintptr_t>(tableNameOut), sizeof(tableNameOut));

		//skip tables which aren't "DT_..."
		if (tableNameOut[0] != 'D' || tableNameOut[1] != 'T')
		{
			return;
		}

		if (classID != -1)
		{
			classIDToName[classID] = tableNameOut;
		}

		classNameToTable[tableNameOut] = RecvTable(tableNameOut);
		RecvTable& cur = classNameToTable[tableNameOut];
		cur.setParent(parent);

		for (uint32_t a = 0; a < numProps; a++)
		{
			uint32_t offset;
			uint32_t propName;
			uint32_t subRecvTable;
			uint32_t recvType;

			char propNameOut[256];
			csgoProc.read(recvPropBase + (propSize * a) + 0x2C, &offset, 1);
			csgoProc.read(recvPropBase + (propSize * a), &propName, 1);
			csgoProc.read(propName, reinterpret_cast<uintptr_t>(propNameOut), sizeof(propNameOut));

			csgoProc.read(recvPropBase + (propSize * a) + 0x28, &subRecvTable, 1);
			csgoProc.read(recvPropBase + (propSize * a) + 0x04, &recvType, 1);
			
			if (subRecvTable && recvType == 6)
			{
				parseRecvTable(subRecvTable, -1, tableNameOut);
			}

			cur.addProp(propNameOut, offset);
		}
	}
}