// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2023 The BitcoinII Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOINII_UTIL_EXCEPTION_H
#define BITCOINII_UTIL_EXCEPTION_H

#include <exception>
#include <string_view>

void PrintExceptionContinue(const std::exception* pex, std::string_view thread_name);

#endif // BITCOINII_UTIL_EXCEPTION_H
