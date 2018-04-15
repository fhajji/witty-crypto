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
	Wt::WTextArea *plainTextEdit_;
	Wt::WTextArea *cipherTextEdit_;

	void newkey();
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

	grid->addWidget(std::make_unique<Wt::WText>("Key "), 0, 0);
	keyText_ = grid->addWidget(std::make_unique<Wt::WText>(), 0, 1);
	auto buttonKey = grid->addWidget(std::make_unique<Wt::WPushButton>("Generate Key"), 0, 2);

	grid->addWidget(std::make_unique<Wt::WText>("Plaintext "), 1, 0);
	plainTextEdit_ = grid->addWidget(std::make_unique<Wt::WTextArea>(), 1, 1);
	plainTextEdit_->setFocus();
	auto buttonEncrypt = grid->addWidget(std::make_unique<Wt::WPushButton>("Encrypt"), 1, 2);
	
	grid->addWidget(std::make_unique<Wt::WText>("Ciphertext "), 2, 0);
	cipherTextEdit_ = grid->addWidget(std::make_unique<Wt::WTextArea>(), 2, 1);
	auto buttonDecrypt = grid->addWidget(std::make_unique<Wt::WPushButton>("Decrypt"), 2, 2);

	grid->setRowStretch(1, 1);
	grid->setRowStretch(2, 1);
	grid->setColumnStretch(1, 1);

	buttonKey->clicked().connect(this, &EncDecApplication::newkey);
	buttonEncrypt->clicked().connect(this, &EncDecApplication::encrypt);
	buttonDecrypt->clicked().connect(this, &EncDecApplication::decrypt);

	plainTextEdit_->enterPressed().connect(this, &EncDecApplication::encrypt);
	cipherTextEdit_->enterPressed().connect(this, &EncDecApplication::decrypt);
}

void EncDecApplication::newkey()
{
	// generate a new random key
	crypto_->newKey(16);
	crypto_->newIV(16);

	std::ostringstream oss;
	auto key = crypto_->key();
	for (std::size_t i = 0; i<key.size(); ++i)
		oss << std::hex << static_cast<unsigned int>(key[i]);
	keyText_->setText(oss.str());
}

void EncDecApplication::encrypt()
{
	auto wPlain{ plainTextEdit_->text().narrow() };
	Crypto::blob_t plaintext(wPlain.cbegin(), wPlain.cend());

	Crypto::blob_t ciphertext{ crypto_->encrypt(plaintext) };
	std::string ct(ciphertext.cbegin(), ciphertext.cend());
	cipherTextEdit_->setText(Wt::WString(ct));
}

void EncDecApplication::decrypt()
{
	auto wCipher{ cipherTextEdit_->text().narrow() };
	Crypto::blob_t ciphertext(wCipher.cbegin(), wCipher.cend());

	Crypto::blob_t plaintext{ crypto_->decrypt(ciphertext) };
	std::string pt(plaintext.cbegin(), plaintext.cend());
	plainTextEdit_->setText(Wt::WString(pt));
}

int main(int argc, char **argv)
{
	return Wt::WRun(argc, argv, [](const Wt::WEnvironment &env) {
		return std::make_unique<EncDecApplication>(env);
	});
}