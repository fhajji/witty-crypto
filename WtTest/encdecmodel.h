// encdecmodel.h -- An MVC model class for PT/CT/Key/IV/...
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

#include <string>
#include <memory>
#include <sstream>
#include <iomanip>

#include <Wt/WSignal.h>

#include "crypto.h"

class EncDecModel
{
public:
	EncDecModel() :
		cryptor_(std::make_unique<Crypto>()),
		ciphers_(Crypto::CipherMap()) {
		cipherChanged_.connect([=] { setKeyIV(); });
		keyChanged_.connect([=]() { encrypt(); });
		ivChanged_.connect([=]() { encrypt(); });
		keyivChanged_.connect([=]() { encrypt(); });
		plaintextChanged().connect([=]() { encrypt(); });
		// circular calls if you do this:
		// ciphertextChanged().connect([=]() { decrypt(); });
	}

	Wt::Signal<std::string>& cipherChanged() { return cipherChanged_; }
	Wt::Signal<std::string>& keyChanged() { return keyChanged_; }
	Wt::Signal<std::string>& ivChanged() { return ivChanged_; }
	Wt::Signal<std::string, std::string>& keyivChanged() { return keyivChanged_; }
	Wt::Signal<std::string>& plaintextChanged() { return plaintextChanged_; }
	Wt::Signal<std::string>& ciphertextChanged() { return ciphertextChanged_; }

	const Crypto::cipher_map_t ciphers() const { return ciphers_; }

	void setCipher(const std::string &cipher) {
		if (cipher != cipher_str_) {
			cryptor_->setCipher(ciphers_[cipher]);
			cipher_str_ = cipher;
			cipherChanged_.emit(cipher);
		}
	}
	const std::string cipher() const { return cipher_str_; }

	void setKey(/* const Crypto::Bytes & newKey */) {
		cryptor_->newKey();
		key_ = cryptor_->key();
		key_str_ = bytesToHex(key_);
		keyChanged_.emit(key_str_);
	}
	const std::string key() const { return key_str_; }

	void setIV(/* const Crypto::Bytes & newIV */) {
		cryptor_->newIV();
		iv_ = cryptor_->iv();
		iv_str_ = bytesToHex(iv_);
		ivChanged_.emit(iv_str_);
	}
	const std::string iv() const { return iv_str_; }

	void setKeyIV() {
		// set key and iv simultaneously

		cryptor_->newKey();
		key_ = cryptor_->key();
		key_str_ = bytesToHex(key_);

		cryptor_->newIV();
		iv_ = cryptor_->iv();
		iv_str_ = bytesToHex(iv_);

		keyivChanged_.emit(key_str_, iv_str_);
	}

	void setPlaintext(const Crypto::Bytes &plaintext) {
		if (plaintext != plaintext_) {
			plaintext_ = plaintext;
			plaintext_str_ = Crypto::toString(plaintext_);
			plaintextChanged_.emit(plaintext_str_);
		}
	}
	const std::string plaintext_str() const { return plaintext_str_; }
	const Crypto::Bytes plaintext() const { return plaintext_; }

	void setCiphertext(const Crypto::Bytes &ciphertext) {
		if (ciphertext != ciphertext_) {
			ciphertext_ = ciphertext;
			ciphertext_str_ = bytesToHex(ciphertext_);
			ciphertextChanged_.emit(ciphertext_str_);
		}
	}
	const std::string ciphertext_str() const { return ciphertext_str_; }
	const Crypto::Bytes ciphertext() const { return ciphertext_; }

	void encrypt() {
		try {
			auto ciphertext = cryptor_->encrypt(plaintext_);
			setCiphertext(ciphertext);
		}
		catch (std::runtime_error &e) {
			auto ciphertext = Crypto::toBytes(e.what());
			setCiphertext(ciphertext);
		}
	}

	void decrypt() {
		try {
			auto plaintext = cryptor_->decrypt(ciphertext_);
			setPlaintext(plaintext);
		}
		catch (std::runtime_error &e) {
			auto plaintext = Crypto::toBytes(e.what());
			setPlaintext(plaintext);
		}
	}

private:
	std::string bytesToHex(const Crypto::Bytes &input) {
		std::ostringstream oss;
		for (const auto &c : input)
			oss << std::hex << std::setw(2) << std::setfill('0')
				<< static_cast<unsigned int>(c);
		return oss.str();
	}

private:
	std::unique_ptr<Crypto> cryptor_;
	Crypto::cipher_map_t ciphers_;

	std::string cipher_str_; // name of current cipher

	Crypto::Bytes key_;
	std::string key_str_; // key as hex string

	Crypto::Bytes iv_;
	std::string iv_str_; // IV as hex string

	Crypto::Bytes plaintext_;
	std::string plaintext_str_;

	Crypto::Bytes ciphertext_;
	std::string ciphertext_str_;

	Wt::Signal<std::string> cipherChanged_;
	Wt::Signal<std::string> keyChanged_;
	Wt::Signal<std::string> ivChanged_;
	Wt::Signal<std::string, std::string> keyivChanged_;
	Wt::Signal<std::string> plaintextChanged_;
	Wt::Signal<std::string> ciphertextChanged_;
};