/*
* Simple Encrypt / Decrypt Demo.
* Copyright (C) 2018 Farid Hajji <farid@hajji.name>
*/

#include <Wt/WApplication.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTextArea.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>

#include "crypto.h"
#include <ios>
#include <sstream>
#include <iomanip>

/*
* A simple encryptor / decryptor.
*/
class EncDecApplication : public Wt::WApplication
{
public:
	EncDecApplication(const Wt::WEnvironment& env);

private:
	std::unique_ptr<Crypto> crypto_;

	Wt::WText     *keyText_;
	Wt::WText     *ivText_;
	Wt::WTextArea *plainTextEdit_;
	Wt::WTextArea *cipherTextEdit_;

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
	crypto_ = std::make_unique<Crypto>();

	setTitle("Crypt Demo");
	root()->setHeight(480);
	root()->setWidth(640);

	auto grid = root()->setLayout(std::make_unique<Wt::WGridLayout>());

	grid->addWidget(std::make_unique<Wt::WText>("Key"), 0, 0);
	keyText_ = grid->addWidget(std::make_unique<Wt::WText>(), 0, 1);
	auto buttonKey = grid->addWidget(std::make_unique<Wt::WPushButton>("Generate Key"), 0, 2);

	grid->addWidget(std::make_unique<Wt::WText>("IV"), 1, 0);
	ivText_ = grid->addWidget(std::make_unique<Wt::WText>(), 1, 1);
	auto buttonIV = grid->addWidget(std::make_unique<Wt::WPushButton>("Generate IV"), 1, 2);

	grid->addWidget(std::make_unique<Wt::WText>("Plaintext"), 2, 0);
	plainTextEdit_ = grid->addWidget(std::make_unique<Wt::WTextArea>(), 2, 1);
	plainTextEdit_->setFocus();
	auto buttonEncrypt = grid->addWidget(std::make_unique<Wt::WPushButton>("Encrypt"), 2, 2);
	
	grid->addWidget(std::make_unique<Wt::WText>("Ciphertext"), 3, 0);
	cipherTextEdit_ = grid->addWidget(std::make_unique<Wt::WTextArea>(), 3, 1);
	auto buttonDecrypt = grid->addWidget(std::make_unique<Wt::WPushButton>("Decrypt"), 3, 2);

	grid->setRowStretch(2, 1);
	grid->setRowStretch(3, 1);
	grid->setColumnStretch(1, 1);

	buttonKey->clicked().connect(this, &EncDecApplication::newkey);
	buttonIV->clicked().connect(this, &EncDecApplication::newiv);
	buttonEncrypt->clicked().connect(this, &EncDecApplication::encrypt);
	buttonDecrypt->clicked().connect(this, &EncDecApplication::decrypt);

	plainTextEdit_->enterPressed().connect(this, &EncDecApplication::encrypt);
	cipherTextEdit_->enterPressed().connect(this, &EncDecApplication::decrypt);

	newkey(); // initialize key
	newiv();  // initialize iv
}

void EncDecApplication::newkey()
{
	// generate a new random key
	crypto_->newKey(16);

	std::ostringstream oss;
	auto key = crypto_->key();
	for (std::size_t i = 0; i<key.size(); ++i)
		oss << std::hex << static_cast<unsigned int>(key[i]);
	keyText_->setText(oss.str());
}

void EncDecApplication::newiv()
{
	// generate a new random IV
	crypto_->newIV(16);

	std::ostringstream oss;
	auto iv = crypto_->iv();
	for (std::size_t i = 0; i<iv.size(); ++i)
		oss << std::hex << static_cast<unsigned int>(iv[i]);
	ivText_->setText(oss.str());
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

	plainTextEdit_->setText(Wt::WString(pt));
}

int main(int argc, char **argv)
{
	return Wt::WRun(argc, argv, [](const Wt::WEnvironment &env) {
		return std::make_unique<EncDecApplication>(env);
	});
}