// scopeguard.h -- A RAII scope guard to clean up resources

#pragma once

#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>

class ScopeGuard
{
public:
	ScopeGuard(EVP_CIPHER_CTX *ctx = nullptr) : ctx_ptr_(ctx) {};
	~ScopeGuard() {
		if (ctx_ptr_)
			EVP_CIPHER_CTX_cleanup(ctx_ptr_);
		ctx_ptr_ = nullptr;
	}

private:
	EVP_CIPHER_CTX * ctx_ptr_;
};