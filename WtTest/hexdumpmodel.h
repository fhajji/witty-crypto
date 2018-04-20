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
		dumper_ = HexDump<std::vector<std::string>>();
	}

	virtual int rowCount(const Wt::WModelIndex &parent = Wt::WModelIndex()) const {
		if (!parent.isValid())
			return static_cast<int>(addr_.size());
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

		addr_  = dumper_.toaddr(instr);
		hex_   = dumper_.tohex(instr);
		print_ = dumper_.toprint(instr);

		reset(); // send modelReset() signal to all attached views.
	}

private:
	HexDump<std::vector<std::string>> dumper_;
	std::vector<std::string> addr_;
	std::vector<std::string> hex_;
	std::vector<std::string> print_;
};