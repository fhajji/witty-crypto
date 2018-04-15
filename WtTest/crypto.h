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
	std::vector<bytes_t> newKey(const std::size_t nbytes) {
		return newrand<EVP_MAX_KEY_LENGTH>(nbytes);
	}

	std::vector<bytes_t> newIV(const std::size_t nbytes) {
		return newrand<EVP_MAX_IV_LENGTH>(nbytes);
	}

private:
	template <std::size_t MAXBYTES>
	std::vector<bytes_t> newrand(const std::size_t nbytes) {
		if (nbytes > MAXBYTES)
			throw std::out_of_range("Key/IV too long");

		std::vector<bytes_t> key(nbytes);

		if (!RAND_bytes(key.data(), nbytes))
			throw std::runtime_error("RAND_bytes()");

		return key;
	}
};