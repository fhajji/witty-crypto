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

/*
* A simple encryptor / decryptor.
*/
class EncDecApplication : public Wt::WApplication
{
public:
	EncDecApplication(const Wt::WEnvironment& env);

private:
	std::unique_ptr<Crypto> crypto_;

	Wt::WLineEdit *keyTextEdit_;
	Wt::WTextArea *plainTextEdit_;
	Wt::WTextArea *cipherTextEdit_;

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
	keyTextEdit_ = grid->addWidget(std::make_unique<Wt::WLineEdit>(), 0, 1);
	keyTextEdit_->setFocus();

	grid->addWidget(std::make_unique<Wt::WText>("Plaintext "), 1, 0);
	plainTextEdit_ = grid->addWidget(std::make_unique<Wt::WTextArea>(), 1, 1);
	auto buttonEncrypt = grid->addWidget(std::make_unique<Wt::WPushButton>("Encrypt"), 1, 2);
	
	grid->addWidget(std::make_unique<Wt::WText>("Ciphertext "), 2, 0);
	cipherTextEdit_ = grid->addWidget(std::make_unique<Wt::WTextArea>(), 2, 1);
	auto buttonDecrypt = grid->addWidget(std::make_unique<Wt::WPushButton>("Decrypt"), 2, 2);

	grid->setRowStretch(1, 1);
	grid->setRowStretch(2, 1);
	grid->setColumnStretch(1, 1);

	buttonEncrypt->clicked().connect(this, &EncDecApplication::encrypt);
	buttonDecrypt->clicked().connect(this, &EncDecApplication::decrypt);

	plainTextEdit_->enterPressed().connect(this, &EncDecApplication::encrypt);
	cipherTextEdit_->enterPressed().connect(this, &EncDecApplication::decrypt);
	keyTextEdit_->enterPressed().connect(this, &EncDecApplication::encrypt);
}

void EncDecApplication::encrypt()
{
	// dummy encrytion for now...
	cipherTextEdit_->setText(Wt::WString("E({1}, {2})").arg(keyTextEdit_->text()).arg(plainTextEdit_->text()));
}

void EncDecApplication::decrypt()
{
	// dummy encryption for now...
	plainTextEdit_->setText(Wt::WString("D({1}, {2})").arg(keyTextEdit_->text()).arg(cipherTextEdit_->text()));
}


int main(int argc, char **argv)
{
	return Wt::WRun(argc, argv, [](const Wt::WEnvironment &env) {
		return std::make_unique<EncDecApplication>(env);
	});
}