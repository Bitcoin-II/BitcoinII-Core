// Copyright (c) 2014-2022 The BitcoinII Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOINII_CRYPTO_RIPEMD160_H
#define BITCOINII_CRYPTO_RIPEMD160_H

#include <cstdlib>
#include <stdint.h>

/** A hasher class for RIPEMD-160. */
class CRIPEMD160
{
private:
    uint32_t s[5];
    unsigned char buf[64];
    uint64_t bytes{0};

public:
    static const size_t OUTPUT_SIZE = 20;

    CRIPEMD160();
    CRIPEMD160& Write(const unsigned char* data, size_t len);
    void Finalize(unsigned char hash[OUTPUT_SIZE]);
    CRIPEMD160& Reset();
};

#endif // BITCOINII_CRYPTO_RIPEMD160_H
