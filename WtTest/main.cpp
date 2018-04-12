/*
* Simple Encrypt / Decrypt Demo.
* Copyright (C) 2018 Farid Hajji <farid@hajji.name>
*/

#include <Wt/WApplication.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTextArea.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>

/*
* A simple encryptor / decryptor.
*/
class EncDecApplication : public Wt::WApplication
{
public:
	EncDecApplication(const Wt::WEnvironment& env);

private:
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
	setTitle("Crypt Demo");

	root()->addWidget(std::make_unique<Wt::WText>("Key "));
	keyTextEdit_ = root()->addWidget(std::make_unique<Wt::WLineEdit>());
	keyTextEdit_->setFocus();

	root()->addWidget(std::make_unique<Wt::WBreak>());    // insert a line break

	root()->addWidget(std::make_unique<Wt::WText>("Plaintext "));
	plainTextEdit_ = root()->addWidget(std::make_unique<Wt::WTextArea>());
	auto buttonEncrypt = root()->addWidget(std::make_unique<Wt::WPushButton>("Encrypt"));
	buttonEncrypt->setMargin(5, Wt::Side::Left);
	
	root()->addWidget(std::make_unique<Wt::WBreak>());    // insert a line break

	root()->addWidget(std::make_unique<Wt::WText>("Ciphertext "));
	cipherTextEdit_ = root()->addWidget(std::make_unique<Wt::WTextArea>());
	auto buttonDecrypt = root()->addWidget(std::make_unique<Wt::WPushButton>("Decrypt"));
	buttonDecrypt->setMargin(5, Wt::Side::Left);
	
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