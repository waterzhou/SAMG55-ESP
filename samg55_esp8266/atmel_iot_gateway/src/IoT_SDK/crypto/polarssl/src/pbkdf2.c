/**
 * \file pbkdf2.c
 *
 * \brief Password-Based Key Derivation Function 2 (from PKCS#5)
 *        DEPRECATED: Use pkcs5.c instead
 *
 * \author Mathias Olsson <mathias@kompetensum.com>
 *
 *  Copyright (C) 2006-2014, Brainspark B.V.
 *
 *  This file is part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*
 * PBKDF2 is part of PKCS#5
 *
 * http://tools.ietf.org/html/rfc2898 (Specification)
 * http://tools.ietf.org/html/rfc6070 (Test vectors)
 */

#if !defined(POLARSSL_CONFIG_FILE)
#include "polarssl/config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#if defined(POLARSSL_PBKDF2_C)

#include "polarssl/pbkdf2.h"
#include "polarssl/pkcs5.h"
#include "polarssl/sha1.h"

int pbkdf2_hmac( md_context_t *ctx, const unsigned char *password, size_t plen,
                 const unsigned char *salt, size_t slen,
                 unsigned int iteration_count,
                 uint32_t key_length, unsigned char *output )
{
    return pkcs5_pbkdf2_hmac( ctx, password, plen, salt, slen, iteration_count,
                              key_length, output );
}


#define A_SHA_DIGEST_LEN    20
unsigned char pbkdf2_sha1_f(unsigned char *key, unsigned char key_length,
                     unsigned char *data, unsigned long data_len,
                   unsigned long iterations, unsigned long count, unsigned char *digest)
{
    unsigned char tmp1[36] = {0}; // [36]
    unsigned char tmp2[A_SHA_DIGEST_LEN] = {0}; // [A_SHA_DIGEST_LEN];
    unsigned int i, j;
    
    for (i = 0; i < key_length ; i++)
    {
        if(key[i] < 32)
        {           
            return 0;
        }
        else if(key[i] > 126)
        {           
            return 0;
        }
    }


    /* U1 = PRF(P, S || int(i)) */
    memcpy(tmp1, data, data_len);

    tmp1[data_len]   = (unsigned char)((count>>24) & 0xFF);
    tmp1[data_len+1] = (unsigned char)((count>>16) & 0xFF);
    tmp1[data_len+2] = (unsigned char)((count>>8) & 0xFF);
    tmp1[data_len+3] = (unsigned char)(count & 0xFF);
	
    sha1_hmac(key, key_length, tmp1, (data_len + 4), tmp2);


    /* digest = U1 */
    memcpy(digest, tmp2, A_SHA_DIGEST_LEN);

    for (i = 1; i < iterations; i++)
    {
        /* Un = PRF(P, Un-1) */
        sha1_hmac(key, key_length, tmp2, A_SHA_DIGEST_LEN, tmp1);

        memcpy(tmp2, tmp1, A_SHA_DIGEST_LEN);

        /* digest = digest xor Un */
        for (j = 0; j < A_SHA_DIGEST_LEN; j++)
        {
            digest[j] ^= tmp1[j];
        }
    }   
    return 1;
}


/* This function runs the PBKDF2 on the data with a given                    */
/* key. To obtain the PSK from the password, PBKDF2 is                       */
/* run on SSID (data) with password (key)                                    */
unsigned char pbkdf2_sha1(unsigned char *key, unsigned char key_len,
                          unsigned char *data, unsigned char data_len,
                     unsigned char *digest)
{
   if ((key_len > 64) || (data_len > 32))
        return 0;

    if(pbkdf2_sha1_f(key, key_len, data, data_len, 4096, 1, digest) == 0)
    {
        return 0;
    }
    return pbkdf2_sha1_f(key, key_len, data, data_len,
                            4096, 2, &digest[A_SHA_DIGEST_LEN]);
							
  
}

#if defined(POLARSSL_SELF_TEST)
int pbkdf2_self_test( int verbose )
{
    return pkcs5_self_test( verbose );
}
#endif /* POLARSSL_SELF_TEST */

#endif /* POLARSSL_PBKDF2_C */
