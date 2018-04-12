// crypto.h

#pragma once

#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>

class Crypto {
public:
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
};