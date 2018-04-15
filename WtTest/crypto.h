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
		// dummy encryption for now
		blob_t ciphertext{ plaintext };
		return ciphertext;
	}

	blob_t decrypt(const blob_t &ciphertext) {
		// dummy decryption for now
		blob_t plaintext{ ciphertext };
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