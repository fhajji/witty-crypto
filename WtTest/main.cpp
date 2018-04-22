/*
* Simple Encrypt / Decrypt Demo.
* Copyright (C) 2018 Farid Hajji <farid@hajji.name>
*/

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
	std::shared_ptr<EncDecModel> ed_model_;

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

	std::map<Wt::WMenuItem *, Wt::WWidget *> mitems_;

	std::shared_ptr<ValidateItemDelegate> hd_delegate_;

	void newcipher();
};

/*
* Create the GUI
*/
EncDecApplication::EncDecApplication(const Wt::WEnvironment& env)
	: WApplication(env),
	ed_model_(std::make_shared<EncDecModel>()),
	hexdump_model_pt_(std::make_shared<HexDumpTableModel>(ed_model_, HexDumpTableModel::PT)),
	hexdump_model_ct_(std::make_shared<HexDumpTableModel>(ed_model_, HexDumpTableModel::CT)),
	hd_delegate_(std::make_shared<ValidateItemDelegate>())
{
	useStyleSheet("WtTest.css");
	setTitle("Crypt Demo");
	root()->setHeight(480);
	root()->setWidth(800);

	auto grid = root()->setLayout(std::make_unique<Wt::WGridLayout>());

	grid->addWidget(std::make_unique<Wt::WText>("Cipher"), 0, 0);
	cbCiphers_ = grid->addWidget(std::make_unique<Wt::WComboBox>(), 0, 1);
	for (const auto &p : ed_model_->ciphers()) {
		cbCiphers_->addItem(p.first);
	}
	cbCiphers_->setCurrentIndex(0);
	cbCiphers_->setMargin(10, Wt::Side::CenterX);

	grid->addWidget(std::make_unique<Wt::WText>("Key"), 1, 0);
	keyText_ = grid->addWidget(std::make_unique<Wt::WText>(), 1, 1);
	auto buttonKey = grid->addWidget(std::make_unique<Wt::WPushButton>("Generate Key"), 1, 2);

	grid->addWidget(std::make_unique<Wt::WText>("IV"), 2, 0);
	ivText_ = grid->addWidget(std::make_unique<Wt::WText>(), 2, 1);
	auto buttonIV = grid->addWidget(std::make_unique<Wt::WPushButton>("Generate IV"), 2, 2);

	grid->addWidget(std::make_unique<Wt::WText>("Plaintext"), 3, 0);
	auto tw_plain = grid->addWidget(std::make_unique<Wt::WTabWidget>(), 3, 1);
	auto mi_ptta = tw_plain->addTab(std::make_unique<Wt::WTextArea>(),
		"Plaintext", Wt::ContentLoading::Eager);
	auto mi_pthd = tw_plain->addTab(std::make_unique<Wt::WTableView>(),
		"Hexdump", Wt::ContentLoading::Eager);
	tw_plain->setStyleClass("tabwidget");
	mitems_[mi_ptta] = tw_plain->widget(0);
	mitems_[mi_pthd] = tw_plain->widget(1);

	plainTextEdit_ = static_cast<Wt::WTextArea *>(mitems_[mi_ptta]);
	plainTextEdit_->setFocus();
	plainTextHDView_ = static_cast<Wt::WTableView *>(mitems_[mi_pthd]);
	plainTextHDView_->setModel(hexdump_model_pt_);

	auto buttonEncrypt = grid->addWidget(std::make_unique<Wt::WPushButton>("Encrypt"), 3, 2);
	
	grid->addWidget(std::make_unique<Wt::WText>("Ciphertext"), 4, 0);
	auto tw_cipher = grid->addWidget(std::make_unique<Wt::WTabWidget>(), 4, 1);
	auto mi_cita = tw_cipher->addTab(std::make_unique<Wt::WTextArea>(),
		"Ciphertext", Wt::ContentLoading::Eager);
	auto mi_cihd = tw_cipher->addTab(std::make_unique<Wt::WTableView>(),
		"Hexdump", Wt::ContentLoading::Eager);
	tw_cipher->setStyleClass("tabwidget");
	mitems_[mi_cita] = tw_cipher->widget(0);
	mitems_[mi_cihd] = tw_cipher->widget(1);

	cipherTextEdit_ = static_cast<Wt::WTextArea *>(mitems_[mi_cita]);
	cipherTextHDView_ = static_cast<Wt::WTableView *>(mitems_[mi_cihd]);
	cipherTextHDView_->setModel(hexdump_model_ct_);

	auto buttonDecrypt = grid->addWidget(std::make_unique<Wt::WPushButton>("Decrypt"), 4, 2);

	grid->setRowStretch(3, 1);
	grid->setRowStretch(4, 1);
	grid->setColumnStretch(1, 1);

	plainTextHDView_->setColumnWidth(0, 80);   // addr
	plainTextHDView_->setColumnWidth(1, 350);  // hex
	plainTextHDView_->setColumnWidth(2, 150);  // print

	cipherTextHDView_->setColumnWidth(0, 80);   // addr
	cipherTextHDView_->setColumnWidth(1, 350);  // hex
	cipherTextHDView_->setColumnWidth(2, 150);  // print

	plainTextHDView_->setItemDelegate(hd_delegate_);
	cipherTextHDView_->setItemDelegate(hd_delegate_);

	plainTextHDView_->setEditTriggers(Wt::EditTrigger::SingleClicked);
	cipherTextHDView_->setEditTriggers(Wt::EditTrigger::SingleClicked);

	cbCiphers_->changed().connect(this, &EncDecApplication::newcipher);

	buttonKey->clicked().connect([=]() { ed_model_->setKey(); });
	buttonIV->clicked().connect([=]() { ed_model_->setIV(); });
	buttonEncrypt->clicked().connect([=]() { ed_model_->encrypt(); });
	buttonDecrypt->clicked().connect([=]() { ed_model_->decrypt(); });

	for (auto p = mitems_.begin(); p != mitems_.end(); ++p) {
		p->first->triggered().connect([=](Wt::WMenuItem *mi){
			mitems_[mi]->show();
		});
	}

	// connect widgets to ed_model_
	plainTextEdit_->changed().connect([=]() {
		ed_model_->setPlaintext(Crypto::toBytes(plainTextEdit_->text().narrow()));
		hexdump_model_pt_->rescan(ed_model_->plaintext());
	});
	cipherTextEdit_->changed().connect([=]() {
		ed_model_->setCiphertext(Crypto::hexToBytes(cipherTextEdit_->text().narrow()));
		hexdump_model_ct_->rescan(ed_model_->ciphertext());
	});

	// connect ed_model_ to widgets
	ed_model_->plaintextChanged().connect([=](std::string s) {
		plainTextEdit_->setText(s);
	    hexdump_model_pt_->rescan(ed_model_->plaintext());
	});
	ed_model_->ciphertextChanged().connect([=](std::string s) {
		cipherTextEdit_->setText(s);
	    hexdump_model_ct_->rescan(ed_model_->ciphertext());
	});
	ed_model_->keyivChanged().connect([=](std::string key, std::string iv) {
		keyText_->setText(key);
		ivText_->setText(iv);
	});
	ed_model_->keyChanged().connect([=](std::string key) {
		keyText_->setText(key);
	});
	ed_model_->ivChanged().connect([=](std::string iv) {
		ivText_->setText(iv);
	});

	newcipher(); // initialize cipher (and key and iv)
}

void EncDecApplication::newcipher()
{
	ed_model_->setCipher(cbCiphers_->currentText().narrow());
}

int main(int argc, char **argv)
{
	return Wt::WRun(argc, argv, [](const Wt::WEnvironment &env) {
		return std::make_unique<EncDecApplication>(env);
	});
}