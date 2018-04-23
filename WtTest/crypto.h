// crypto.h -- Crypto class with calls to OpenSSL's libcrypto.
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

#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <exception>

#include "scopeguard.h"

class Crypto {
public:
	using Bytes = std::vector<unsigned char>;
	using cipher_map_t = std::map<std::string, const EVP_CIPHER *>;

	Crypto(const EVP_CIPHER *cipher = nullptr) : cipher_(cipher) {
		ERR_load_crypto_strings();
		OpenSSL_add_all_algorithms();
		OPENSSL_config(NULL);
	}

	~Crypto() {
		EVP_cleanup();
		CRYPTO_cleanup_all_ex_data();
		ERR_free_strings();
	}

	static const cipher_map_t CipherMap() {
		const cipher_map_t ciphers = {
			{ "EVP_aes_128_ecb", EVP_aes_128_ecb() },
			{ "EVP_aes_128_cbc", EVP_aes_128_cbc() },
			{ "EVP_aes_256_cbc", EVP_aes_256_cbc() },
		};
		return ciphers;
	}

	void setCipher(const EVP_CIPHER *cipher) { cipher_ = cipher; }

	const Bytes key() const { return key_; }
	const Bytes iv() const { return iv_; }

	void newKey() {
		assert(cipher_ != nullptr);
		key_ = newrand<EVP_MAX_KEY_LENGTH>(EVP_CIPHER_key_length(cipher_));
	}

	void newIV() {
		assert(cipher_ != nullptr);
		iv_ = newrand<EVP_MAX_IV_LENGTH>(EVP_CIPHER_iv_length(cipher_));
	}

	Bytes encrypt(const Bytes &plaintext) {
		assert(cipher_ != nullptr);
		EVP_CIPHER_CTX ctx;
		ScopeGuard guard(&ctx);

		EVP_CIPHER_CTX_init(&ctx);

		if (1 != EVP_EncryptInit_ex(&ctx,
			cipher_,
			NULL,
			key_.data(),
			iv_.data())) {
			throw std::runtime_error(error_msg());
		}

		// output buffer size = inl + cipher_block_size - 1
		Bytes ciphertext(plaintext.size() + 16 - 1);

		int outl = 0;
		if (1 != EVP_EncryptUpdate(&ctx, ciphertext.data(), &outl,
			plaintext.data(), static_cast<int>(plaintext.size()))) {
			throw std::runtime_error(error_msg());
		}
		ciphertext.resize(outl);

		Bytes finalblock(16);
		if (1 != EVP_EncryptFinal_ex(&ctx, finalblock.data(), &outl)) {
			throw std::runtime_error(error_msg());
		}
		for (int i = 0; i < outl; ++i)
			ciphertext.push_back(finalblock[i]);

		return ciphertext;
	}

	Bytes decrypt(const Bytes &ciphertext) {
		assert(cipher_ != nullptr);
		EVP_CIPHER_CTX ctx;
		ScopeGuard guard(&ctx);

		EVP_CIPHER_CTX_init(&ctx);

		if (1 != EVP_DecryptInit_ex(&ctx,
			cipher_,
			NULL,
			key_.data(),
			iv_.data())) {
			throw std::runtime_error(error_msg());
		}

		// output buffer size = inl + cipher_block_size
		Bytes plaintext(ciphertext.size() + 16);

		int outl = 0;
		if (1 != EVP_DecryptUpdate(&ctx, plaintext.data(), &outl,
			ciphertext.data(), static_cast<int>(ciphertext.size()))) {
			throw std::runtime_error(error_msg());
		}
		plaintext.resize(outl);

		Bytes finalblock(16);
		if (1 != EVP_DecryptFinal_ex(&ctx, finalblock.data(), &outl)) {
			throw std::runtime_error(error_msg());
		}
		for (int i = 0; i < outl; ++i)
			plaintext.push_back(finalblock[i]);

		return plaintext;
	}

	static std::string toString(const Bytes &input) {
		std::string out;
		for (const auto &byte : input)
			out.push_back(static_cast<char>(byte));
		return out;
	}

	static Bytes toBytes(const std::string &input) {
		Bytes out;
		for (const auto &c : input)
			out.push_back(static_cast<unsigned char>(c));
		return out;
	}

	static Bytes hexToBytes(const std::string &hexinput) {
		Bytes out;
		if (hexinput.size() % 2)
			return out; // odd size, invalid hex
		for (auto i = 0; i != hexinput.size(); i += 2) {
			std::string hextwo;
			hextwo.push_back(hexinput[i]);
			hextwo.push_back(hexinput[i + 1]);
			std::istringstream iss(hextwo);
			unsigned int c;
			iss >> std::hex >> c;
			out.push_back(static_cast<unsigned char>(c));
		}
		return out;
	}

private:
	std::string error_msg() {
		std::ostringstream ess;
		while (auto err = ERR_get_error()) {
			char buf[256];
			ERR_error_string_n(err, buf, sizeof(buf));
			ess << buf << std::endl;
		}
		return ess.str();
	}

	template <std::size_t MAXBYTES>
	Bytes newrand(const std::size_t nbytes) {
		if (nbytes > MAXBYTES)
			throw std::out_of_range("Key/IV too long");

		Bytes key(nbytes);

		if (!RAND_bytes(key.data(), static_cast<int>(nbytes))) {
			throw std::runtime_error(error_msg());
		}
		return key;
	}

	const EVP_CIPHER *cipher_;
	Bytes key_;
	Bytes iv_;
};