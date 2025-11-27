// Copyright (c) 2022 The BitcoinII Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOINII_POLICY_FEES_ARGS_H
#define BITCOINII_POLICY_FEES_ARGS_H

#include <util/fs.h>

class ArgsManager;

/** @return The fee estimates data file path. */
fs::path FeeestPath(const ArgsManager& argsman);

#endif // BITCOINII_POLICY_FEES_ARGS_H
