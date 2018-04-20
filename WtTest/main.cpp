/*
* Simple Encrypt / Decrypt Demo.
* Copyright (C) 2018 Farid Hajji <farid@hajji.name>
*/

#include <Wt/WApplication.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WTabWidget.h>
#include <Wt/WMenuItem.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTextArea.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WComboBox.h>

#include "crypto.h"
#include <ios>
#include <sstream>
#include <iomanip>
#include <vector>

#include "hexdumpmodel.h"

/*
* A simple encryptor / decryptor.
*/
class EncDecApplication : public Wt::WApplication
{
public:
	EncDecApplication(const Wt::WEnvironment& env);

private:
	Crypto::cipher_map_t    ciphers_;
	std::unique_ptr<Crypto> crypto_;
	std::unique_ptr<HexDumpTableModel> hexdump_model_pt_; // plaintext hexdump model
	std::unique_ptr<HexDumpTableModel> hexdump_model_ct_; // ciphertext hexdump model

	// our application data
	const EVP_CIPHER *theCipher_;
	Crypto::Bytes theKey_;
	Crypto::Bytes theIV_;
	Crypto::Bytes thePlainText_;
	Crypto::Bytes theCipherText_;

	// widgets displaying our application data
	Wt::WComboBox *cbCiphers_;
	Wt::WText     *keyText_;
	Wt::WText     *ivText_;
	Wt::WTextArea *plainTextEdit_;
	Wt::WTextArea *cipherTextEdit_;

	std::vector<Wt::WMenuItem *> plainTabMenuItems_;
	std::vector<Wt::WMenuItem *> cipherTabMenuItems_;

	void newcipher();
	void newkey();
	void newiv();
	void updatePlainText();
	void updateCipherText();
	void encrypt();
	void decrypt();
};

/*
* Create the GUI
*/
EncDecApplication::EncDecApplication(const Wt::WEnvironment& env)
	: WApplication(env)
{
	ciphers_ = Crypto::CipherMap();
	crypto_ = std::make_unique<Crypto>();
	hexdump_model_pt_ = std::make_unique<HexDumpTableModel>();
	hexdump_model_ct_ = std::make_unique<HexDumpTableModel>();

	useStyleSheet("WtTest.css");
	setTitle("Crypt Demo");
	root()->setHeight(480);
	root()->setWidth(800);

	auto grid = root()->setLayout(std::make_unique<Wt::WGridLayout>());

	grid->addWidget(std::make_unique<Wt::WText>("Cipher"), 0, 0);
	cbCiphers_ = grid->addWidget(std::make_unique<Wt::WComboBox>(), 0, 1);
	for (const auto &p : ciphers_) {
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
	plainTabMenuItems_.push_back(tw_plain->addTab(std::make_unique<Wt::WTextArea>(),
		"Plaintext", Wt::ContentLoading::Eager));
	plainTabMenuItems_.push_back(tw_plain->addTab(std::make_unique<Wt::WText>("Hexdump goes here"),
		"Hexdump", Wt::ContentLoading::Eager));
	tw_plain->setStyleClass("tabwidget");
	plainTextEdit_ = static_cast<Wt::WTextArea *>(tw_plain->widget(0));
	plainTextEdit_->setFocus();

	auto buttonEncrypt = grid->addWidget(std::make_unique<Wt::WPushButton>("Encrypt"), 3, 2);
	
	grid->addWidget(std::make_unique<Wt::WText>("Ciphertext"), 4, 0);
	auto tw_cipher = grid->addWidget(std::make_unique<Wt::WTabWidget>(), 4, 1);
	cipherTabMenuItems_.push_back(tw_cipher->addTab(std::make_unique<Wt::WTextArea>(),
		"Ciphertext", Wt::ContentLoading::Eager));
	cipherTabMenuItems_.push_back(tw_cipher->addTab(std::make_unique<Wt::WText>("Hexdump goes here"),
		"Hexdump", Wt::ContentLoading::Eager));
	tw_cipher->setStyleClass("tabwidget");
	cipherTextEdit_ = static_cast<Wt::WTextArea *>(tw_cipher->widget(0));

	auto buttonDecrypt = grid->addWidget(std::make_unique<Wt::WPushButton>("Decrypt"), 4, 2);

	grid->setRowStretch(3, 1);
	grid->setRowStretch(4, 1);
	grid->setColumnStretch(1, 1);

	cbCiphers_->changed().connect(this, &EncDecApplication::newcipher);

	buttonKey->clicked().connect(this, &EncDecApplication::newkey);
	buttonIV->clicked().connect(this, &EncDecApplication::newiv);
	buttonEncrypt->clicked().connect(this, &EncDecApplication::encrypt);
	buttonDecrypt->clicked().connect(this, &EncDecApplication::decrypt);

	plainTextEdit_->changed().connect(this, &EncDecApplication::updatePlainText);
	cipherTextEdit_->changed().connect(this, &EncDecApplication::updateCipherText);

	plainTextEdit_->enterPressed().connect(this, &EncDecApplication::encrypt);
	cipherTextEdit_->enterPressed().connect(this, &EncDecApplication::decrypt);

	tw_plain->currentChanged().connect([=](int newTabIdx){
		tw_plain->setCurrentIndex(newTabIdx); // XXX no visible effect. Why?
		plainTabMenuItems_.at(newTabIdx)->addStyleClass("active");
	});
	tw_cipher->currentChanged().connect([=](int newTabIdx) {
		tw_cipher->setCurrentIndex(newTabIdx); // XXX no visible effect. Why?
		cipherTabMenuItems_.at(newTabIdx)->addStyleClass("active");
	});

	newcipher(); // initialize cipher (and key and iv)
}

void EncDecApplication::newkey()
{
	// generate a new random key
	crypto_->newKey();
	theKey_ = crypto_->key();

	std::ostringstream oss;
	for (std::size_t i = 0; i<theKey_.size(); ++i)
		oss << std::hex << static_cast<unsigned int>(theKey_[i]);
	keyText_->setText(oss.str());
}

void EncDecApplication::newiv()
{
	// generate a new random IV
	crypto_->newIV();
	theIV_ = crypto_->iv();

	std::ostringstream oss;
	for (std::size_t i = 0; i<theIV_.size(); ++i)
		oss << std::hex << static_cast<unsigned int>(theIV_[i]);
	ivText_->setText(oss.str());
}

void EncDecApplication::newcipher()
{
	theCipher_ = ciphers_[cbCiphers_->currentText().narrow()];
	crypto_->setCipher(theCipher_);
	newkey();
	newiv();
}

void EncDecApplication::updatePlainText()
{
	thePlainText_ = Crypto::toBytes(plainTextEdit_->text().narrow());
}

void EncDecApplication::updateCipherText()
{
	theCipherText_ = Crypto::toBytes(cipherTextEdit_->text().narrow());
}

void EncDecApplication::encrypt()
{
	// assume that our thePlainText_ is already synchronized
	// with plainTextEdit_

	// first, get the new plaintext from the widget:
	// auto wPlain{ plainTextEdit_->text().narrow() };
	// thePlainText_.assign(wPlain.cbegin(), wPlain.cend())
	// Crypto::blob_t plaintext(wPlain.cbegin(), wPlain.cend());

	try {
		theCipherText_ = crypto_->encrypt(thePlainText_);
	}
	catch (std::runtime_error &e) {
		// output error message instead
		theCipherText_ = Crypto::toBytes(e.what());
	}

	hexdump_model_ct_->rescan(theCipherText_); // ciphertext has changed
	cipherTextEdit_->setText(Wt::WString(Crypto::toString(theCipherText_)));
}

void EncDecApplication::decrypt()
{
	// assume that our theCipherText_ is already synchronized
	// with cipherTextEdit_

	// first, get the new ciphertext from the widget:
	// auto wCipher{ cipherTextEdit_->text().narrow() };
	// Crypto::blob_t ciphertext;

	// std::istringstream iss(Crypto::toString(theCipherText_));
	// unsigned int c;
	// while (iss >> std::hex >> c) {
	// 	ciphertext.push_back(static_cast<unsigned char>(c));
	// }

	try {
		thePlainText_ = crypto_->decrypt(theCipherText_);
	}
	catch (std::runtime_error &e) {
		thePlainText_ = Crypto::toBytes(e.what());
	}

	hexdump_model_pt_->rescan(thePlainText_); // plaintext has changed
	plainTextEdit_->setText(Wt::WString(Crypto::toString(thePlainText_)));
}

int main(int argc, char **argv)
{
	return Wt::WRun(argc, argv, [](const Wt::WEnvironment &env) {
		return std::make_unique<EncDecApplication>(env);
	});
}