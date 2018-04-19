/*
* Simple Encrypt / Decrypt Demo.
* Copyright (C) 2018 Farid Hajji <farid@hajji.name>
*/

#include <Wt/WApplication.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WTabWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTextArea.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WComboBox.h>

#include "crypto.h"
#include <ios>
#include <sstream>
#include <iomanip>

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

	Wt::WComboBox *cbCiphers_;
	Wt::WText     *keyText_;
	Wt::WText     *ivText_;
	Wt::WTextArea *plainTextEdit_;
	Wt::WTextArea *cipherTextEdit_;

	void newcipher();
	void newkey();
	void newiv();
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
	// plainTextEdit_ = grid->addWidget(std::make_unique<Wt::WTextArea>(), 3, 1);
	// plainTextEdit_->setFocus();
	auto tw_plain = grid->addWidget(std::make_unique<Wt::WTabWidget>(), 3, 1);
	tw_plain->addTab(std::make_unique<Wt::WTextArea>(),
		"Plaintext", Wt::ContentLoading::Eager);
	tw_plain->addTab(std::make_unique<Wt::WText>("Hexdump goes here"),
		"Hexdump", Wt::ContentLoading::Eager);
	tw_plain->setStyleClass("tabwidget");
	plainTextEdit_ = static_cast<Wt::WTextArea *>(tw_plain->widget(0));
	plainTextEdit_->setFocus();

	auto buttonEncrypt = grid->addWidget(std::make_unique<Wt::WPushButton>("Encrypt"), 3, 2);
	
	grid->addWidget(std::make_unique<Wt::WText>("Ciphertext"), 4, 0);
	// cipherTextEdit_ = grid->addWidget(std::make_unique<Wt::WTextArea>(), 4, 1);
	auto tw_cipher = grid->addWidget(std::make_unique<Wt::WTabWidget>(), 4, 1);
	tw_cipher->addTab(std::make_unique<Wt::WTextArea>(),
		"Ciphertext", Wt::ContentLoading::Eager);
	tw_cipher->addTab(std::make_unique<Wt::WText>("Hexdump goes here"),
		"Hexdump", Wt::ContentLoading::Eager);
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

	plainTextEdit_->enterPressed().connect(this, &EncDecApplication::encrypt);
	cipherTextEdit_->enterPressed().connect(this, &EncDecApplication::decrypt);

	newcipher(); // initialize cipher (and key and iv)
}

void EncDecApplication::newkey()
{
	// generate a new random key
	crypto_->newKey();

	std::ostringstream oss;
	auto key = crypto_->key();
	for (std::size_t i = 0; i<key.size(); ++i)
		oss << std::hex << static_cast<unsigned int>(key[i]);
	keyText_->setText(oss.str());
}

void EncDecApplication::newiv()
{
	// generate a new random IV
	crypto_->newIV();

	std::ostringstream oss;
	auto iv = crypto_->iv();
	for (std::size_t i = 0; i<iv.size(); ++i)
		oss << std::hex << static_cast<unsigned int>(iv[i]);
	ivText_->setText(oss.str());
}

void EncDecApplication::newcipher()
{
	const EVP_CIPHER *cipher = ciphers_[cbCiphers_->currentText().narrow()];
	crypto_->setCipher(cipher);
	newkey();
	newiv();
}

void EncDecApplication::encrypt()
{
	auto wPlain{ plainTextEdit_->text().narrow() };
	Crypto::blob_t plaintext(wPlain.cbegin(), wPlain.cend());

	Crypto::blob_t ciphertext;
	std::string ct;

	try {
		ciphertext = crypto_->encrypt(plaintext);
		// output the ciphertext as (lossless) hex
		std::ostringstream oss;
		oss << std::hex << std::setfill('0');

		for (const unsigned char c : ciphertext) {
			oss << std::setw(2) << static_cast<unsigned int>(c) << " ";
		}
		ct.assign(oss.str());
	}
	catch (std::runtime_error &e) {
		// output error message instead
		ct = e.what();
	}

	hexdump_model_ct_->rescan(ct); // ciphertext has changed
	cipherTextEdit_->setText(Wt::WString(ct));
}

void EncDecApplication::decrypt()
{
	auto wCipher{ cipherTextEdit_->text().narrow() };
	Crypto::blob_t ciphertext;

	std::istringstream iss(wCipher);
	unsigned int c;
	while (iss >> std::hex >> c) {
		ciphertext.push_back(static_cast<unsigned char>(c));
	}

	Crypto::blob_t plaintext;
	std::string pt;

	try {
		plaintext = crypto_->decrypt(ciphertext);
		pt.assign(plaintext.cbegin(), plaintext.cend());
	}
	catch (std::runtime_error &e) {
		pt = e.what();
	}

	hexdump_model_pt_->rescan(pt); // plaintext has changed
	plainTextEdit_->setText(Wt::WString(pt));
}

int main(int argc, char **argv)
{
	return Wt::WRun(argc, argv, [](const Wt::WEnvironment &env) {
		return std::make_unique<EncDecApplication>(env);
	});
}