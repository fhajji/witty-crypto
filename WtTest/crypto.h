// crypto.h

#pragma once

#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <vector>
#include <utility>
#include <exception>

class Crypto {
public:
	using bytes_t = unsigned char;
	using blob_t = std::vector<bytes_t>;

	Crypto() {
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

	void newKey(const std::size_t nbytes) {
		key_ = newrand<EVP_MAX_KEY_LENGTH>(nbytes);
	}

	void newIV(const std::size_t nbytes) {
		iv_ = newrand<EVP_MAX_IV_LENGTH>(nbytes);
	}

	blob_t encrypt(const blob_t &plaintext) {
		EVP_CIPHER_CTX ctx;
		EVP_CIPHER_CTX_init(&ctx);

		if (1 != EVP_EncryptInit_ex(&ctx,
			EVP_aes_128_cbc(),
			NULL,
			key_.data(),
			iv_.data())) {
			EVP_CIPHER_CTX_cleanup(&ctx);
			throw std::runtime_error("EVP_EncryptInit_ex()");
		}

		// output buffer size = inl + cipher_block_size - 1
		blob_t ciphertext(plaintext.size() + 16 - 1);

		int outl = 0;
		if (1 != EVP_EncryptUpdate(&ctx, ciphertext.data(), &outl,
			plaintext.data(), plaintext.size())) {
			EVP_CIPHER_CTX_cleanup(&ctx);
			throw std::runtime_error("EVP_EncryptUpdate()");
		}
		ciphertext.resize(outl);

		blob_t finalblock(16);
		if (1 != EVP_EncryptFinal_ex(&ctx, finalblock.data(), &outl)) {
			EVP_CIPHER_CTX_cleanup(&ctx);
			throw std::runtime_error("EVP_EncryptFinal_ex()");
		}
		for (int i = 0; i < outl; ++i)
			ciphertext.push_back(finalblock[i]);

		EVP_CIPHER_CTX_cleanup(&ctx);
		return ciphertext;
	}

	blob_t decrypt(const blob_t &ciphertext) {
		EVP_CIPHER_CTX ctx;
		EVP_CIPHER_CTX_init(&ctx);

		if (1 != EVP_DecryptInit_ex(&ctx,
			EVP_aes_128_cbc(),
			NULL,
			key_.data(),
			iv_.data())) {
			EVP_CIPHER_CTX_cleanup(&ctx);
			throw std::runtime_error("EVP_DecryptInit_ex()");
		}

		// output buffer size = inl + cipher_block_size
		blob_t plaintext(ciphertext.size() + 16);

		int outl = 0;
		if (1 != EVP_DecryptUpdate(&ctx, plaintext.data(), &outl,
			ciphertext.data(), ciphertext.size())) {
			EVP_CIPHER_CTX_cleanup(&ctx);
			throw std::runtime_error("EVP_DecryptUpdate()");
		}
		plaintext.resize(outl);

		blob_t finalblock(16);
		if (1 != EVP_DecryptFinal_ex(&ctx, finalblock.data(), &outl)) {
			EVP_CIPHER_CTX_cleanup(&ctx);
			throw std::runtime_error("EVP_DecryptFinal_ex()");
		}
		for (int i = 0; i < outl; ++i)
			plaintext.push_back(finalblock[i]);

		EVP_CIPHER_CTX_cleanup(&ctx);
		return plaintext;
	}

private:
	template <std::size_t MAXBYTES>
	blob_t newrand(const std::size_t nbytes) {
		if (nbytes > MAXBYTES)
			throw std::out_of_range("Key/IV too long");

		blob_t key(nbytes);

		if (!RAND_bytes(key.data(), nbytes))
			throw std::runtime_error("RAND_bytes()");

		return key;
	}

	blob_t key_;
	blob_t iv_;
};