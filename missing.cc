/* Mostly consists of modified code to implement missing functions for certain SSL
 * implementations, e.g. BoringSSL. Original Copyright header follows:
 */

/* ====================================================================
 * Copyright (c) 1998-2002 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */

#include <stdlib.h>
extern "C" {
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/bn.h>
#include <openssl/ec.h>
}

namespace opmsg {

#ifdef HAVE_BORINGSSL
BIGNUM *EC_POINT_point2bn(const EC_GROUP * group, const EC_POINT * point, point_conversion_form_t form, BIGNUM * ret, BN_CTX * ctx)
{
	size_t buf_len = 0;
	unsigned char *buf;

	buf_len = EC_POINT_point2oct(group, point, form, nullptr, 0, ctx);
	if (buf_len == 0)
		return nullptr;

	if ((buf = (unsigned char *)malloc(buf_len)) == nullptr)
		return nullptr;

	if (!EC_POINT_point2oct(group, point, form, buf, buf_len, ctx)) {
		free(buf);
		return nullptr;
	}
	ret = BN_bin2bn(buf, buf_len, ret);

	free(buf);

	return ret;
}


EC_POINT *EC_POINT_bn2point(const EC_GROUP * group, const BIGNUM * bn, EC_POINT * point, BN_CTX * ctx)
{
	size_t buf_len = 0;
	unsigned char *buf;
	EC_POINT *ret;

	if ((buf_len = BN_num_bytes(bn)) == 0)
		return nullptr;
	buf = (unsigned char *)malloc(buf_len);
	if (buf == nullptr)
		return nullptr;

	if (!BN_bn2bin(bn, buf)) {
		free(buf);
		return nullptr;
	}
	if (point == nullptr) {
		if ((ret = EC_POINT_new(group)) == nullptr) {
			free(buf);
			return nullptr;
		}
	} else
		ret = point;

	if (!EC_POINT_oct2point(group, ret, buf, buf_len, ctx)) {
		if (point == nullptr)
			EC_POINT_clear_free(ret);
		free(buf);
		return nullptr;
	}
	free(buf);
	return ret;
}

#endif

}
