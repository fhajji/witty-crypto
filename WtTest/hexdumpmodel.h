// hexdumpmodel.h -- A WAbstractTabelModel class for HexDump
// Copyright (C) 2018 Farid Hajji <farid@hajji.name>

// ISC License(ISC)
// 
// Copyright 2018 Farid Hajji <farid@hajji.name>
// 
// Permission to use, copy, modify, and/or distribute this software
// for any purpose with or without fee is hereby granted, provided
// that the above copyright notice and this permission notice appear
// in all copies.
// 
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS.IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
// OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#pragma once

#include <Wt/WString.h>
#include <Wt/WModelIndex.h>
#include <Wt/WAbstractTableModel.h>
#include <Wt/WAny.h>
#include <string>
#include <vector>
#include "hexdump.h"
#include "encdecmodel.h"

class HexDumpTableModel : public Wt::WAbstractTableModel
{
public:
	constexpr static int PT = 0;
	constexpr static int CT = 1;

	HexDumpTableModel(const std::shared_ptr<EncDecModel> &ed_model, const int ptct = PT) :
		Wt::WAbstractTableModel(),
		ed_model_(ed_model),
		ptct_(ptct)
	{
		assert(ptct_ == 0 || ptct_ == 1);
		dumper_ = HexDump<std::vector<std::string>>();
	}

	int rowCount(const Wt::WModelIndex &parent = Wt::WModelIndex()) const override {
		if (!parent.isValid())
			return static_cast<int>(addr_.size());
		else
			return 0;
	}

	int columnCount(const Wt::WModelIndex& parent = Wt::WModelIndex()) const override {
		if (!parent.isValid())
			return 3; // addr, hex, print
		else
			return 0;
	}

	Wt::cpp17::any data(const Wt::WModelIndex& index, Wt::ItemDataRole role = Wt::ItemDataRole::Display) const override {
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

		case Wt::ItemDataRole::Edit:
			if (index.column() == 1)
				return Wt::WString(hex_[index.row()]); // prefill for edit
			else
				return Wt::cpp17::any();

		default:
			return Wt::cpp17::any();
		}
	}

	bool setData(const Wt::WModelIndex& index, const Wt::cpp17::any &value, Wt::ItemDataRole role = Wt::ItemDataRole::Edit) override {
		std::string value_str, str_to_print;

		switch (role.value()) {
		case Wt::ItemDataRole::Edit:
			assert(index.column() == 1); // enforced by flags()

			value_str = Wt::asString(value).narrow();
			// NYI: validate and reformat value_str

			// convert string w/ hex codes to _printable_ string
			str_to_print = dumper_.toprintline(dumper_.fromhex(value_str));

			// update model
			hex_[index.row()] = value_str;
			print_[index.row()] = str_to_print;

			// update associated plaintext/ciphertext view
			// by updating the underlying EncDecModel, which
			// will signal those views to update themselves.
			switch (ptct_) {
			case PT:
				ed_model_->setPlaintext(Crypto::toBytes(dumper_.fromhexlines(hex_)));
				break;
			case CT:
				ed_model_->setCiphertext(Crypto::toBytes(dumper_.fromhexlines(hex_)));
				break;
			default:
				// NOTREACHED
				break;
			}

			// selectively signal views about changed AREA
			// by emitting dataChanged():
			// dataChanged().emit(index, index with column()+1);

			// easier than dataChanged():
			// signal interested views that the _whole_ model was updated
			reset();

			return true;

		default:
			return false;
		}
	}

	Wt::WFlags<Wt::ItemFlag> flags(const Wt::WModelIndex &index) const override {
		switch (index.column()) {
		case 0:
			return Wt::ItemFlag::Selectable; // addr non-editable
		case 1:
			return Wt::ItemFlag::Editable; // hex IS editable
		case 2:
			return Wt::ItemFlag::Selectable; // print non-editable
		default:
			return Wt::ItemFlag::Selectable; // NOTREACHED
		}
	}

	void rescan(const Crypto::Bytes &input) {
		auto instr = Crypto::toString(input);

		addr_ = dumper_.toaddr(instr);
		hex_ = dumper_.tohex(instr);
		print_ = dumper_.toprint(instr);

		reset(); // send modelReset() signal to all attached views.
	}

private:
	HexDump<std::vector<std::string>> dumper_;
	std::vector<std::string> addr_;
	std::vector<std::string> hex_;
	std::vector<std::string> print_;
	std::shared_ptr<EncDecModel> ed_model_;
	int ptct_;
};