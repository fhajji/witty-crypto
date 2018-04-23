// scopeguard.h -- A RAII scope guard to clean up resources
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