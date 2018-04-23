// EncDecApplication.h -- The GUI for the EncDec Demo
// Copyright (C) 2018 Farid Hajji <farid@hajji.name>

// ISC License
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

// squelch msvs-2017 annoying dll-interface warnings
#pragma warning ( disable: 4251 )
#pragma warning ( disable: 4275 )

#include <memory>

#include <Wt/WApplication.h>
#include <Wt/WGridLayout.h>
#include <Wt/WTabWidget.h>
#include <Wt/WMenuItem.h>
#include <Wt/WTextArea.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WComboBox.h>
#include <Wt/WTableView.h>
#include <Wt/WRegExpValidator.h>

#include "crypto.h"
#include "encdecmodel.h"
#include "hexdumpmodel.h"
#include "validateitemdelegate.h"

/*
* A simple encryptor / decryptor.
*/
class EncDecApplication : public Wt::WApplication
{
public:
	EncDecApplication(const Wt::WEnvironment& env);

private:
	std::shared_ptr<EncDecModel> ed_model_; // model holding our app data

	const std::shared_ptr<HexDumpTableModel> hexdump_model_pt_; // plaintext hexdump model
	const std::shared_ptr<HexDumpTableModel> hexdump_model_ct_; // ciphertext hexdump model

	// widgets displaying our application data
	Wt::WComboBox *cbCiphers_;
	Wt::WText     *keyText_;
	Wt::WText     *ivText_;
	Wt::WTextArea *plainTextEdit_;
	Wt::WTextArea *cipherTextEdit_;
	Wt::WTableView *plainTextHDView_;
	Wt::WTableView *cipherTextHDView_;
	Wt::WPushButton *buttonKey_;
	Wt::WPushButton *buttonIV_;
	Wt::WPushButton *buttonEncrypt_;
	Wt::WPushButton *buttonDecrypt_;

	std::map<Wt::WMenuItem *, Wt::WWidget *> mitems_;

	std::shared_ptr<ValidateItemDelegate> hd_delegate_;  // hexdump view editor
	std::shared_ptr<Wt::WRegExpValidator> hd_validator_; // hexdump validator

	void create_gui();
	void connect_signals();
	void newcipher();
};