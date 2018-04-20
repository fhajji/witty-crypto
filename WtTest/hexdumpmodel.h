// hexdumpmodel.h -- A WAbstractTabelModel class for HexDump
// Copyright (C) 2018 Farid Hajji <farid@hajji.name>

#pragma once

#include <Wt/WString.h>
#include <Wt/WModelIndex.h>
#include <Wt/WAbstractTableModel.h>
#include <Wt/WAny.h>
#include <string>
#include <vector>
#include "hexdump.h"

class HexDumpTableModel : public Wt::WAbstractTableModel
{
public:
	HexDumpTableModel() :
		Wt::WAbstractTableModel() {
		dumper_ = HexDump();
	}

	virtual int rowCount(const Wt::WModelIndex &parent = Wt::WModelIndex()) const {
		if (!parent.isValid())
			return addr_.size();
		else
			return 0;
	}

	virtual int columnCount(const Wt::WModelIndex& parent = Wt::WModelIndex()) const
	{
		if (!parent.isValid())
			return 3; // addr, hex, print
		else
			return 0;
	}

	virtual Wt::cpp17::any data(const Wt::WModelIndex& index, Wt::ItemDataRole role = Wt::ItemDataRole::Display) const
	{
		switch (role.value()) {
		case Wt::ItemDataRole::Display:
			switch (index.column()) {
			case 0:
				return Wt::WString(addr_[index.row()]);
			case 1:
				return Wt::WString(hex_[index.row()]);
			case 2:
				return Wt::WString(print_[index.row()]);
			default:
				return Wt::WString("Index(x,y) = {1},{2} out of bounds").arg(index.row()).arg(index.column());
			}
		
		default:
			return Wt::cpp17::any();
		}
	}

	void rescan(const Crypto::Bytes &input) {
		auto instr = Crypto::toString(input);

		auto addr_list = dumper_.toaddr(instr);
		auto hex_list = dumper_.tohex(instr);
		auto print_list = dumper_.toprint(instr);

		// convert std::list<> to std::vector<>
		addr_.assign(addr_list.cbegin(), addr_list.cend());
		hex_.assign(hex_list.cbegin(), hex_list.cend());
		print_.assign(print_list.cbegin(), print_list.cend());
	}

private:
	HexDump dumper_;
	std::vector<std::string> addr_;
	std::vector<std::string> hex_;
	std::vector<std::string> print_;
};