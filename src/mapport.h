// Copyright (c) 2011-2020 The BitcoinII Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOINII_MAPPORT_H
#define BITCOINII_MAPPORT_H

static constexpr bool DEFAULT_NATPMP = false;

void StartMapPort(bool enable);
void InterruptMapPort();
void StopMapPort();

#endif // BITCOINII_MAPPORT_H
