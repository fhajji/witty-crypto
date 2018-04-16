// crypto.h

#pragma once

#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <vector>
#include <utility>
#include <exception>

#include "scopeguard.h"

class Crypto {
public:
	using bytes_t = unsigned char;
	using blob_t = std::vector<bytes_t>;

	Crypto(const EVP_CIPHER *cipher) : cipher_(cipher) {
		ERR_load_crypto_strings();
		OpenSSL_add_all_algorithms();
		OPENSSL_config(NULL);
	}

	~Crypto() {
		EVP_cleanup();
		CRYPTO_cleanup_all_ex_data();
		ERR_free_strings();
	}

public:
	const blob_t key() const { return key_; }
	const blob_t iv() const { return iv_; }

	void newKey() {
		key_ = newrand<EVP_MAX_KEY_LENGTH>(EVP_CIPHER_key_length(cipher_));
	}

	void newIV() {
		iv_ = newrand<EVP_MAX_IV_LENGTH>(EVP_CIPHER_iv_length(cipher_));
	}

	blob_t encrypt(const blob_t &plaintext) {
		EVP_CIPHER_CTX ctx;
		ScopeGuard guard(&ctx);

		EVP_CIPHER_CTX_init(&ctx);

		if (1 != EVP_EncryptInit_ex(&ctx,
			cipher_,
			NULL,
			key_.data(),
			iv_.data())) {
			std::ostringstream ess;
			ess << "EVP_EncryptInit_ex(): " << std::endl;
			while (auto err = ERR_get_error()) {
				char buf[256];
				ERR_error_string_n(err, buf, sizeof(buf));
				ess << buf << std::endl;
			}

			throw std::runtime_error(ess.str());
		}

		// output buffer size = inl + cipher_block_size - 1
		blob_t ciphertext(plaintext.size() + 16 - 1);

		int outl = 0;
		if (1 != EVP_EncryptUpdate(&ctx, ciphertext.data(), &outl,
			plaintext.data(), plaintext.size())) {
			std::ostringstream ess;
			ess << "EVP_EncryptUpdate(): " << std::endl;
			while (auto err = ERR_get_error()) {
				char buf[256];
				ERR_error_string_n(err, buf, sizeof(buf));
				ess << buf << std::endl;
			}

			throw std::runtime_error(ess.str());
		}
		ciphertext.resize(outl);

		blob_t finalblock(16);
		if (1 != EVP_EncryptFinal_ex(&ctx, finalblock.data(), &outl)) {
			std::ostringstream ess;
			ess << "EVP_EncryptFinal_ex(): " << std::endl;
			while (auto err = ERR_get_error()) {
				char buf[256];
				ERR_error_string_n(err, buf, sizeof(buf));
				ess << buf << std::endl;
			}

			throw std::runtime_error(ess.str());
		}
		for (int i = 0; i < outl; ++i)
			ciphertext.push_back(finalblock[i]);

		return ciphertext;
	}

	blob_t decrypt(const blob_t &ciphertext) {
		EVP_CIPHER_CTX ctx;
		ScopeGuard guard(&ctx);

		EVP_CIPHER_CTX_init(&ctx);

		if (1 != EVP_DecryptInit_ex(&ctx,
			cipher_,
			NULL,
			key_.data(),
			iv_.data())) {
			std::ostringstream ess;
			ess << "EVP_DecryptInit_ex(): " << std::endl;
			while (auto err = ERR_get_error()) {
				char buf[256];
				ERR_error_string_n(err, buf, sizeof(buf));
				ess << buf << std::endl;
			}

			throw std::runtime_error(ess.str());
		}

		// output buffer size = inl + cipher_block_size
		blob_t plaintext(ciphertext.size() + 16);

		int outl = 0;
		if (1 != EVP_DecryptUpdate(&ctx, plaintext.data(), &outl,
			ciphertext.data(), ciphertext.size())) {
			std::ostringstream ess;
			ess << "EVP_DecryptUpdate(): " << std::endl;
			while (auto err = ERR_get_error()) {
				char buf[256];
				ERR_error_string_n(err, buf, sizeof(buf));
				ess << buf << std::endl;
			}

			throw std::runtime_error(ess.str());
		}
		plaintext.resize(outl);

		blob_t finalblock(16);
		if (1 != EVP_DecryptFinal_ex(&ctx, finalblock.data(), &outl)) {
			std::ostringstream ess;
			ess << "EVP_DecryptFinal_ex(): " << std::endl;
			while (auto err = ERR_get_error()) {
				char buf[256];
				ERR_error_string_n(err, buf, sizeof(buf));
				ess << buf << std::endl;
			}

			throw std::runtime_error(ess.str());
		}
		for (int i = 0; i < outl; ++i)
			plaintext.push_back(finalblock[i]);

		return plaintext;
	}

private:
	template <std::size_t MAXBYTES>
	blob_t newrand(const std::size_t nbytes) {
		if (nbytes > MAXBYTES)
			throw std::out_of_range("Key/IV too long");

		blob_t key(nbytes);

		if (!RAND_bytes(key.data(), nbytes)) {
			std::ostringstream ess;
			ess << "RAND_bytes(): " << std::endl;
			while (auto err = ERR_get_error()) {
				char buf[256];
				ERR_error_string_n(err, buf, sizeof(buf));
				ess << buf << std::endl;
			}

			throw std::runtime_error(ess.str());
		}
		return key;
	}

	const EVP_CIPHER *cipher_;
	blob_t key_;
	blob_t iv_;
};