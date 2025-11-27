// Copyright (c) 2024 The BitcoinII Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOINII_IPC_CAPNP_MINING_TYPES_H
#define BITCOINII_IPC_CAPNP_MINING_TYPES_H

#include <interfaces/mining.h>
#include <ipc/capnp/common.capnp.proxy-types.h>
#include <ipc/capnp/common-types.h>
#include <ipc/capnp/mining.capnp.proxy.h>
#include <node/miner.h>
#include <node/types.h>
#include <validation.h>

namespace mp {
// Custom serialization for BlockValidationState.
void CustomBuildMessage(InvokeContext& invoke_context,
                        const BlockValidationState& src,
                        ipc::capnp::messages::BlockValidationState::Builder&& builder);
void CustomReadMessage(InvokeContext& invoke_context,
                       const ipc::capnp::messages::BlockValidationState::Reader& reader,
                       BlockValidationState& dest);
} // namespace mp

#endif // BITCOINII_IPC_CAPNP_MINING_TYPES_H
