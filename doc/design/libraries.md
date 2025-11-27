# Libraries

| Name                     | Description |
|--------------------------|-------------|
| *libbitcoinII_cli*         | RPC client functionality used by *bitcoinII-cli* executable |
| *libbitcoinII_common*      | Home for common functionality shared by different executables and libraries. Similar to *libbitcoinII_util*, but higher-level (see [Dependencies](#dependencies)). |
| *libbitcoinII_consensus*   | Consensus functionality used by *libbitcoinII_node* and *libbitcoinII_wallet*. |
| *libbitcoinII_crypto*      | Hardware-optimized functions for data encryption, hashing, message authentication, and key derivation. |
| *libbitcoinII_kernel*      | Consensus engine and support library used for validation by *libbitcoinII_node*. |
| *libbitcoinIIqt*           | GUI functionality used by *bitcoinII-qt* and *bitcoinII-gui* executables. |
| *libbitcoinII_ipc*         | IPC functionality used by *bitcoinII-node*, *bitcoinII-wallet*, *bitcoinII-gui* executables to communicate when [`-DWITH_MULTIPROCESS=ON`](multiprocess.md) is used. |
| *libbitcoinII_node*        | P2P and RPC server functionality used by *bitcoinIId* and *bitcoinII-qt* executables. |
| *libbitcoinII_util*        | Home for common functionality shared by different executables and libraries. Similar to *libbitcoinII_common*, but lower-level (see [Dependencies](#dependencies)). |
| *libbitcoinII_wallet*      | Wallet functionality used by *bitcoinIId* and *bitcoinII-wallet* executables. |
| *libbitcoinII_wallet_tool* | Lower-level wallet functionality used by *bitcoinII-wallet* executable. |
| *libbitcoinII_zmq*         | [ZeroMQ](../zmq.md) functionality used by *bitcoinIId* and *bitcoinII-qt* executables. |

## Conventions

- Most libraries are internal libraries and have APIs which are completely unstable! There are few or no restrictions on backwards compatibility or rules about external dependencies. An exception is *libbitcoinII_kernel*, which, at some future point, will have a documented external interface.

- Generally each library should have a corresponding source directory and namespace. Source code organization is a work in progress, so it is true that some namespaces are applied inconsistently, and if you look at [`add_library(bitcoinII_* ...)`](../../src/CMakeLists.txt) lists you can see that many libraries pull in files from outside their source directory. But when working with libraries, it is good to follow a consistent pattern like:

  - *libbitcoinII_node* code lives in `src/node/` in the `node::` namespace
  - *libbitcoinII_wallet* code lives in `src/wallet/` in the `wallet::` namespace
  - *libbitcoinII_ipc* code lives in `src/ipc/` in the `ipc::` namespace
  - *libbitcoinII_util* code lives in `src/util/` in the `util::` namespace
  - *libbitcoinII_consensus* code lives in `src/consensus/` in the `Consensus::` namespace

## Dependencies

- Libraries should minimize what other libraries they depend on, and only reference symbols following the arrows shown in the dependency graph below:

<table><tr><td>

```mermaid

%%{ init : { "flowchart" : { "curve" : "basis" }}}%%

graph TD;

bitcoinII-cli[bitcoinII-cli]-->libbitcoinII_cli;

bitcoinIId[bitcoinIId]-->libbitcoinII_node;
bitcoinIId[bitcoinIId]-->libbitcoinII_wallet;

bitcoinII-qt[bitcoinII-qt]-->libbitcoinII_node;
bitcoinII-qt[bitcoinII-qt]-->libbitcoinIIqt;
bitcoinII-qt[bitcoinII-qt]-->libbitcoinII_wallet;

bitcoinII-wallet[bitcoinII-wallet]-->libbitcoinII_wallet;
bitcoinII-wallet[bitcoinII-wallet]-->libbitcoinII_wallet_tool;

libbitcoinII_cli-->libbitcoinII_util;
libbitcoinII_cli-->libbitcoinII_common;

libbitcoinII_consensus-->libbitcoinII_crypto;

libbitcoinII_common-->libbitcoinII_consensus;
libbitcoinII_common-->libbitcoinII_crypto;
libbitcoinII_common-->libbitcoinII_util;

libbitcoinII_kernel-->libbitcoinII_consensus;
libbitcoinII_kernel-->libbitcoinII_crypto;
libbitcoinII_kernel-->libbitcoinII_util;

libbitcoinII_node-->libbitcoinII_consensus;
libbitcoinII_node-->libbitcoinII_crypto;
libbitcoinII_node-->libbitcoinII_kernel;
libbitcoinII_node-->libbitcoinII_common;
libbitcoinII_node-->libbitcoinII_util;

libbitcoinIIqt-->libbitcoinII_common;
libbitcoinIIqt-->libbitcoinII_util;

libbitcoinII_util-->libbitcoinII_crypto;

libbitcoinII_wallet-->libbitcoinII_common;
libbitcoinII_wallet-->libbitcoinII_crypto;
libbitcoinII_wallet-->libbitcoinII_util;

libbitcoinII_wallet_tool-->libbitcoinII_wallet;
libbitcoinII_wallet_tool-->libbitcoinII_util;

classDef bold stroke-width:2px, font-weight:bold, font-size: smaller;
class bitcoinII-qt,bitcoinIId,bitcoinII-cli,bitcoinII-wallet bold
```
</td></tr><tr><td>

**Dependency graph**. Arrows show linker symbol dependencies. *Crypto* lib depends on nothing. *Util* lib is depended on by everything. *Kernel* lib depends only on consensus, crypto, and util.

</td></tr></table>

- The graph shows what _linker symbols_ (functions and variables) from each library other libraries can call and reference directly, but it is not a call graph. For example, there is no arrow connecting *libbitcoinII_wallet* and *libbitcoinII_node* libraries, because these libraries are intended to be modular and not depend on each other's internal implementation details. But wallet code is still able to call node code indirectly through the `interfaces::Chain` abstract class in [`interfaces/chain.h`](../../src/interfaces/chain.h) and node code calls wallet code through the `interfaces::ChainClient` and `interfaces::Chain::Notifications` abstract classes in the same file. In general, defining abstract classes in [`src/interfaces/`](../../src/interfaces/) can be a convenient way of avoiding unwanted direct dependencies or circular dependencies between libraries.

- *libbitcoinII_crypto* should be a standalone dependency that any library can depend on, and it should not depend on any other libraries itself.

- *libbitcoinII_consensus* should only depend on *libbitcoinII_crypto*, and all other libraries besides *libbitcoinII_crypto* should be allowed to depend on it.

- *libbitcoinII_util* should be a standalone dependency that any library can depend on, and it should not depend on other libraries except *libbitcoinII_crypto*. It provides basic utilities that fill in gaps in the C++ standard library and provide lightweight abstractions over platform-specific features. Since the util library is distributed with the kernel and is usable by kernel applications, it shouldn't contain functions that external code shouldn't call, like higher level code targeted at the node or wallet. (*libbitcoinII_common* is a better place for higher level code, or code that is meant to be used by internal applications only.)

- *libbitcoinII_common* is a home for miscellaneous shared code used by different BitcoinII Core applications. It should not depend on anything other than *libbitcoinII_util*, *libbitcoinII_consensus*, and *libbitcoinII_crypto*.

- *libbitcoinII_kernel* should only depend on *libbitcoinII_util*, *libbitcoinII_consensus*, and *libbitcoinII_crypto*.

- The only thing that should depend on *libbitcoinII_kernel* internally should be *libbitcoinII_node*. GUI and wallet libraries *libbitcoinIIqt* and *libbitcoinII_wallet* in particular should not depend on *libbitcoinII_kernel* and the unneeded functionality it would pull in, like block validation. To the extent that GUI and wallet code need scripting and signing functionality, they should be able to get it from *libbitcoinII_consensus*, *libbitcoinII_common*, *libbitcoinII_crypto*, and *libbitcoinII_util*, instead of *libbitcoinII_kernel*.

- GUI, node, and wallet code internal implementations should all be independent of each other, and the *libbitcoinIIqt*, *libbitcoinII_node*, *libbitcoinII_wallet* libraries should never reference each other's symbols. They should only call each other through [`src/interfaces/`](../../src/interfaces/) abstract interfaces.

## Work in progress

- Validation code is moving from *libbitcoinII_node* to *libbitcoinII_kernel* as part of [The libbitcoinIIkernel Project #27587](https://github.com/bitcoinII/bitcoinII/issues/27587)
