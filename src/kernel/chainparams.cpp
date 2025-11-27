// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The BitcoinII Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kernel/chainparams.h>

#include <chainparamsseeds.h>
#include <consensus/amount.h>
#include <consensus/merkle.h>
#include <consensus/params.h>
#include <hash.h>
#include <kernel/messagestartchars.h>
#include <logging.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/interpreter.h>
#include <script/script.h>
#include <uint256.h>
#include <util/chaintype.h>
#include <util/strencodings.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>

using namespace util::hex_literals;

// Workaround MSVC bug triggering C7595 when calling consteval constructors in
// initializer lists.
// A fix may be on the way:
// https://developercommunity.visualstudio.com/t/consteval-conversion-function-fails/1579014
#if defined(_MSC_VER)
auto consteval_ctor(auto&& input) { return input; }
#else
#define consteval_ctor(input) (input)
#endif

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.version = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "BBC News 12/04/2024 French government collapses in no-confidence vote";
    const CScript genesisOutputScript = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network on which people trade goods and services.
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        m_chain_type = ChainType::MAIN;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.script_flag_exceptions.emplace( // BIP16 exception
            uint256{"0000000028f062b221c1a8a5cf0244b1627315f7aa5b775b931cfec46dc17ceb"}, SCRIPT_VERIFY_NONE);
        consensus.script_flag_exceptions.emplace( // Taproot exception
            uint256{"0000000028f062b221c1a8a5cf0244b1627315f7aa5b775b931cfec46dc17ceb"}, SCRIPT_VERIFY_P2SH | SCRIPT_VERIFY_WITNESS);
        consensus.BIP34Height = 250;
        consensus.BIP34Hash = uint256{"000000008e0241217a0c31509cc92ef8367d6add5668e07fd36269e287b5e7ef"};
        consensus.BIP65Height = 260; // 000000009b9a28c9368119f47479cfb0024d957d7416efcc0cc550acce627234
        consensus.BIP66Height = 270; // 00000000ab1c93522aa21d8461f37c3f70c34e4c6bfff5d9737026aa46e08b0b
        consensus.CSVHeight = 280; // 00000000ad2f324d3b3cf38ecfd168004b66346a0e447eda7bd73c2573805a42
        consensus.SegwitHeight = 290; // 0000000043d4294e484e2f10ad2c5a8d19bc8d60295a33f7c804dbed872cced8
        consensus.MinBIP9WarningHeight = 2306; // segwit activation height + miner confirmation window
        consensus.powLimit = uint256{"00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1815; // 90% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = 1734019071; // April 24th, 2021
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = 18942120000; // August 11th, 2021
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 300; // Approximately November 12th, 2021

        consensus.nMinimumChainWork = uint256{"0000000000000000000000000000000000000000000000959028194ff1139272"};
        consensus.defaultAssumeValid = uint256{"00000000000000067e82c9cebc8b58e70f0be31908598d3240a4ecbaa527682e"}; // 33000

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0x42;
        pchMessageStart[1] = 0x49;
        pchMessageStart[2] = 0x49;
        pchMessageStart[3] = 0x21;
        nDefaultPort = 8338;
        nPruneAfterHeight = 200000;
        m_assumed_blockchain_size = 10;
        m_assumed_chain_state_size = 10;

        genesis = CreateGenesisBlock(1734019071,1597163478, 0x1d00ffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"0000000028f062b221c1a8a5cf0244b1627315f7aa5b775b931cfec46dc17ceb"});
        assert(genesis.hashMerkleRoot == uint256{"80d1b4e9ca868f83b88b9301036205876072bdd3ded0ad4dc022e1f9266ddc49"});

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as an addrfetch if they don't support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.
        vSeeds.emplace_back("dnsseed.bitcoin-ii.org.");
        vSeeds.emplace_back("bitcoinII.ddns.net."); // Original BitcoinII seed server

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,0);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        bech32_hrp = "bc";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_main), std::end(chainparams_seed_main));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                { 50, uint256{"000000001edb1b92b898111135767e3e4b86eeac0ada772ffca8c4f00302105b"}},
                { 250, uint256{"000000008e0241217a0c31509cc92ef8367d6add5668e07fd36269e287b5e7ef"}},
                { 260, uint256{"000000009b9a28c9368119f47479cfb0024d957d7416efcc0cc550acce627234"}},
                { 270, uint256{"00000000ab1c93522aa21d8461f37c3f70c34e4c6bfff5d9737026aa46e08b0b"}},
                { 280, uint256{"00000000ad2f324d3b3cf38ecfd168004b66346a0e447eda7bd73c2573805a42"}},
                { 290, uint256{"0000000043d4294e484e2f10ad2c5a8d19bc8d60295a33f7c804dbed872cced8"}},
                { 425, uint256{"00000000dc956ee0d18f0e8f401c4dc3f248d00ca1c3f8ea0d60c6842b022390"}},
                { 500, uint256{"00000000d89bb3d9e7410a1cc00e48eac1c8af5d27f781465e732d0a22ed868a"}},
                { 750, uint256{"000000004ffbcd37f7da4b112dc0a782b39bb014c12a4cd38fd961027a0f86dd"}},
                { 1000, uint256{"00000000d7e4e8062230de032e163c9ccf082568ec3db3e345acc1065cabf2d3"}},
                { 1250, uint256{"000000008abd7b9793c776183bdf14731c45466e43049691dbdc2687b90d1779"}},
                { 1500, uint256{"000000000381048281f3d4be6b23d424b43f11502a95d862c5f316fc8dfe7859"}},
                { 1750, uint256{"0000000089e45559192c25bc437e34dad753c443d6418cdbc06f9d1d000155a7"}},
                { 1900, uint256{"000000004e4b848131fb1e3615d0c4ca2f1a613e22e02bf4cacfa31b11dd4260"}},
                { 2000, uint256{"0000000092460949b8686786acea9333d407bce955aff6f6a5f8d16d29644ffc"}},
                { 2016, uint256{"000000001477dda4e29a1333f4ed3702bca0040fada8d8254ce02f180873e182"}},
                { 10000, uint256{"000000000034a7f5c7100e911ab43ce02ac228c9e8ac2ae9fe7a1eb61914cdc0"}},
                { 15000, uint256{"0000000000048dbb85404da5d97fbdb9b5c15dcd9b41239d49a6ff3f13d2b0b8"}},
                { 20000, uint256{"0000000000009d769d62468e90627f274ef7d83f3f99ce6bfc128991cd57a53a"}},
                { 25000, uint256{"00000000000000b393c072088fd2ef8bcdacc05e15fe937b8b09ca1241877a7f"}},
                { 30000, uint256{"000000000000001fc3181c713894a3e1f92b2b74b37a01dac267e58e2d53669c"}},
                { 49000, uint256{"000000000000000003016ffca13aaee2ac07ea3edb81e7e612ccce9c98c7ca04"}},
                
            }
        };

        m_assumeutxo_data = {
            {
                .height = 49'491,
                .hash_serialized = AssumeutxoHash{uint256{"7923774181d3be2bd319458ba07b7eb311ac6d4c431f9064db7ed015a8ea3ce7"}},
                .m_chain_tx_count = 82281,
                .blockhash = consteval_ctor(uint256{"00000000000000001984b0fe236bec1ec49a14a01d8d340c31cae864d96a3b9b"}),
            }
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 00000000000000001984b0fe236bec1ec49a14a01d8d340c31cae864d96a3b9b
            .nTime    = 1764034154,
            .tx_count = 82281,
            .dTxRate  = 0.004174125833742384,
        };
    }
};

/**
 * Testnet (v3): public test network which is reset from time to time.
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        m_chain_type = ChainType::TESTNET;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.script_flag_exceptions.emplace( // BIP16 exception
            uint256{"00000000dd30457c001f4095d208cc1296b0eed002427aa599874af7a432b105"}, SCRIPT_VERIFY_NONE);
        consensus.BIP34Height = 21111;
        consensus.BIP34Hash = uint256{"0000000023b3a96d3484e5abb3755c413e7d41500f8e2a5c3f0dd01299cd8ef8"};
        consensus.BIP65Height = 581885; // 00000000007f6655f22f98e72ed80d8b06dc761d5da09df0fa1dc4be4f861eb6
        consensus.BIP66Height = 330776; // 000000002104c8c45e99a8853285a3b592602a3ccde2b832481da85e9e4ba182
        consensus.CSVHeight = 770112; // 00000000025e930139bac5c6c31a403776da130831ab85be56578f3fa75369bb
        consensus.SegwitHeight = 834624; // 00000000002b980fcd729daaa248fd9316a5200e9b367f4ff2c42453e84201ca
        consensus.MinBIP9WarningHeight = 836640; // segwit activation height + miner confirmation window
        consensus.powLimit = uint256{"00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = 1619222400; // April 24th, 2021
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = 1628640000; // August 11th, 2021
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256{"0000000000000000000000000000000000000000000015f5e0c9f13455b0eb17"};
        consensus.defaultAssumeValid = uint256{"00000000000003fc7967410ba2d0a8a8d50daedc318d43e8baf1a9782c236a57"}; // 3974606

        pchMessageStart[0] = 0x0c;
        pchMessageStart[1] = 0x12;
        pchMessageStart[2] = 0x10;
        pchMessageStart[3] = 0x08;
        nDefaultPort = 18338;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 200;
        m_assumed_chain_state_size = 19;

        genesis = CreateGenesisBlock(1750495449, 344360033, 0x1d00ffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"000000004d52d701341aa65ad81dfd0b9c29132d261fc8b8a7f4ebcd75633c37"});
        assert(genesis.hashMerkleRoot == uint256{"80d1b4e9ca868f83b88b9301036205876072bdd3ded0ad4dc022e1f9266ddc49"});
        
        
        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("testnet-seed.bitcoinII.ddns.net.");
        

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tb";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_test), std::end(chainparams_seed_test));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {546, uint256{"000000002a936ca763904c3c35fce2f3556c559c0214345d31b1bcebf76acb70"}},
            }
        };

        m_assumeutxo_data = {
            {
                .height = 2'500'000,
                .hash_serialized = AssumeutxoHash{uint256{"f841584909f68e47897952345234e37fcd9128cd818f41ee6c3ca68db8071be7"}},
                .m_chain_tx_count = 66484552,
                .blockhash = consteval_ctor(uint256{"0000000000000093bcb68c03a9a168ae252572d348a2eaeba2cdf9231d73206f"}),
            }
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 00000000000003fc7967410ba2d0a8a8d50daedc318d43e8baf1a9782c236a57
            .nTime    = 1741042082,
            .tx_count = 475477615,
            .dTxRate  = 17.15933950357594,
        };
    }
};

/**
 * Testnet (v4): public test network which is reset from time to time.
 */
class CTestNet4Params : public CChainParams {
public:
    CTestNet4Params() {
        m_chain_type = ChainType::TESTNET4;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.script_flag_exceptions.emplace( // BIP16 exception
            uint256{"000000005541e67eb6f1851976a2269faf7aeb5195df05e4cd0f9a7649852e7d"}, SCRIPT_VERIFY_NONE);
        consensus.script_flag_exceptions.emplace( // Taproot exception
            uint256{"000000005541e67eb6f1851976a2269faf7aeb5195df05e4cd0f9a7649852e7d"}, SCRIPT_VERIFY_P2SH | SCRIPT_VERIFY_WITNESS);
        consensus.BIP34Height = 250;
        consensus.BIP34Hash = uint256{"00000000a57e762b33b4a3c35b63f6de3b14ab459d22b5890c07137a4c7cfa37"};
        consensus.BIP65Height = 260; // 000000009b9a28c9368119f47479cfb0024d957d7416efcc0cc550acce627234
        consensus.BIP66Height = 270; // 00000000ab1c93522aa21d8461f37c3f70c34e4c6bfff5d9737026aa46e08b0b
        consensus.CSVHeight = 280; // 00000000ad2f324d3b3cf38ecfd168004b66346a0e447eda7bd73c2573805a42
        consensus.SegwitHeight = 290; // 0000000043d4294e484e2f10ad2c5a8d19bc8d60295a33f7c804dbed872cced8
        consensus.MinBIP9WarningHeight = 2306; // segwit activation height + miner confirmation window
        consensus.powLimit = uint256{"00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = 1734019071; // April 24th, 2021
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = 18942120000; // August 11th, 2021
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 300; // Approximately November 12th, 2021

        consensus.nMinimumChainWork = uint256{"0000000000000000000000000000000000000000000000000000000000000001"};
        consensus.defaultAssumeValid = uint256{"000000005541e67eb6f1851976a2269faf7aeb5195df05e4cd0f9a7649852e7d"}; // 0

        pchMessageStart[0] = 0x1b;
        pchMessageStart[1] = 0x15;
        pchMessageStart[2] = 0x3c;
        pchMessageStart[3] = 0x27;
        nDefaultPort = 48338;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 1;
        m_assumed_chain_state_size = 1;

        const char* testnet4_genesis_msg = "BBC News 12/04/2024 French government collapses in no-confidence vote";
        const CScript testnet4_genesis_script = CScript() << "04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f"_hex << OP_CHECKSIG;
        genesis = CreateGenesisBlock(testnet4_genesis_msg,
                testnet4_genesis_script,
                1753841191,
                4262804050,
                0x1d00ffff,
                1,
                50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"000000005541e67eb6f1851976a2269faf7aeb5195df05e4cd0f9a7649852e7d"});
        assert(genesis.hashMerkleRoot == uint256{"80d1b4e9ca868f83b88b9301036205876072bdd3ded0ad4dc022e1f9266ddc49"});

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("seed.testnet4.bitcoinII.ddns.net."); 
        

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tb";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_testnet4), std::end(chainparams_seed_testnet4));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {},
            }
        };

        m_assumeutxo_data = {
            {}
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 0000000000003ed4f08dbdf6f7d6b271a6bcffce25675cb40aa9fa43179a89f3
            .nTime    = 1741070246,
            .tx_count = 7653966,
            .dTxRate  = 1.239174414591965,
        };
    }
};

/**
 * Signet: test network with an additional consensus parameter (see BIP325).
 */
class SigNetParams : public CChainParams {
public:
    explicit SigNetParams(const SigNetOptions& options)
    {
        std::vector<uint8_t> bin;
        vFixedSeeds.clear();
        vSeeds.clear();

        if (!options.challenge) {
            bin = "512103ad5e0edad18cb1f0fc0d28a3d4f1f3e445640337489abb10404f2d1e086be430210359ef5021964fe22d6f8e05b2463c9540ce96883fe3b278760f048f5189f2e6c452ae"_hex_v_u8;
            vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_signet), std::end(chainparams_seed_signet));
            vSeeds.emplace_back("seed.signet.bitcoinII.ddns.net.");
            

            consensus.nMinimumChainWork = uint256{"000000000000000000000000000000000000000000000000000002b517f3d1a1"};
            consensus.defaultAssumeValid = uint256{"000000895a110f46e59eb82bbc5bfb67fa314656009c295509c21b4999f5180a"}; // 237722
            m_assumed_blockchain_size = 9;
            m_assumed_chain_state_size = 1;
            chainTxData = ChainTxData{
                // Data from RPC: getchaintxstats 4096 000000895a110f46e59eb82bbc5bfb67fa314656009c295509c21b4999f5180a
                .nTime    = 1741019645,
                .tx_count = 16540736,
                .dTxRate  = 1.064918879911595,
            };
        } else {
            bin = *options.challenge;
            consensus.nMinimumChainWork = uint256{};
            consensus.defaultAssumeValid = uint256{};
            m_assumed_blockchain_size = 0;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                0,
                0,
                0,
            };
            LogPrintf("Signet with challenge %s\n", HexStr(bin));
        }

        if (options.seeds) {
            vSeeds = *options.seeds;
        }

        m_chain_type = ChainType::SIGNET;
        consensus.signet_blocks = true;
        consensus.signet_challenge.assign(bin.begin(), bin.end());
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 1;
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1815; // 90% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256{"00000377ae000000000000000000000000000000000000000000000000000000"};
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Activation of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        // message start is defined as the first 4 bytes of the sha256d of the block script
        HashWriter h{};
        h << consensus.signet_challenge;
        uint256 hash = h.GetHash();
        std::copy_n(hash.begin(), 4, pchMessageStart.begin());

        nDefaultPort = 38338;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1750495451, 2366523058, 0x1d00ffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"00000000b365b3110f1c1f6ab3c6bce212c8cbdfacd07cf68767425e916f330c"});
        assert(genesis.hashMerkleRoot == uint256{"80d1b4e9ca868f83b88b9301036205876072bdd3ded0ad4dc022e1f9266ddc49"});

        m_assumeutxo_data = {
            {
                .height = 160'000,
                .hash_serialized = AssumeutxoHash{uint256{"fe0a44309b74d6b5883d246cb419c6221bcccf0b308c9b59b7d70783dbdf928a"}},
                .m_chain_tx_count = 2289496,
                .blockhash = consteval_ctor(uint256{"0000003ca3c99aff040f2563c2ad8f8ec88bd0fd6b8f0895cfaf1ef90353a62c"}),
            }
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tb";

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;
    }
};

/**
 * Regression test: intended for private networks only. Has minimal difficulty to ensure that
 * blocks can be found instantly.
 */
class CRegTestParams : public CChainParams
{
public:
    explicit CRegTestParams(const RegTestOptions& opts)
    {
        m_chain_type = ChainType::REGTEST;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP34Height = 1; // Always active unless overridden
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1;  // Always active unless overridden
        consensus.BIP66Height = 1;  // Always active unless overridden
        consensus.CSVHeight = 1;    // Always active unless overridden
        consensus.SegwitHeight = 0; // Always active unless overridden
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256{"7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.nPowTargetTimespan = 24 * 60 * 60; // one day
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = opts.enforce_bip94;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{};

        pchMessageStart[0] = 0xfb;
        pchMessageStart[1] = 0xbb;
        pchMessageStart[2] = 0xb6;
        pchMessageStart[3] = 0xdb;
        nDefaultPort = 18448;
        nPruneAfterHeight = opts.fastprune ? 100 : 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        for (const auto& [dep, height] : opts.activation_heights) {
            switch (dep) {
            case Consensus::BuriedDeployment::DEPLOYMENT_SEGWIT:
                consensus.SegwitHeight = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_HEIGHTINCB:
                consensus.BIP34Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_DERSIG:
                consensus.BIP66Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CLTV:
                consensus.BIP65Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CSV:
                consensus.CSVHeight = int{height};
                break;
            }
        }

        for (const auto& [deployment_pos, version_bits_params] : opts.version_bits_parameters) {
            consensus.vDeployments[deployment_pos].nStartTime = version_bits_params.start_time;
            consensus.vDeployments[deployment_pos].nTimeout = version_bits_params.timeout;
            consensus.vDeployments[deployment_pos].min_activation_height = version_bits_params.min_activation_height;
        }

        genesis = CreateGenesisBlock(1750495460, 3948631054, 0x1d00ffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"00000000379eca4e51372f2b154c8287e189c461335a8b98aff26eeb74bca0a4"});
        assert(genesis.hashMerkleRoot == uint256{"80d1b4e9ca868f83b88b9301036205876072bdd3ded0ad4dc022e1f9266ddc49"});

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();
        vSeeds.emplace_back("dummySeed.invalid.");

        fDefaultConsistencyChecks = true;
        m_is_mockable_chain = true;

        checkpointData = {
            {
                {0, uint256{"00000000379eca4e51372f2b154c8287e189c461335a8b98aff26eeb74bca0a4"}},
            }
        };

        m_assumeutxo_data = {
            {   // For use by unit tests
                .height = 110,
                .hash_serialized = AssumeutxoHash{uint256{"6657b736d4fe4db0cbc796789e812d5dba7f5c143764b1b6905612f1830609d1"}},
                .m_chain_tx_count = 111,
                .blockhash = consteval_ctor(uint256{"696e92821f65549c7ee134edceeeeaaa4105647a3c4fd9f298c0aec0ab50425c"}),
            },
            {
                // For use by fuzz target src/test/fuzz/utxo_snapshot.cpp
                .height = 200,
                .hash_serialized = AssumeutxoHash{uint256{"4f34d431c3e482f6b0d67b64609ece3964dc8d7976d02ac68dd7c9c1421738f2"}},
                .m_chain_tx_count = 201,
                .blockhash = consteval_ctor(uint256{"5e93653318f294fb5aa339d00bbf8cf1c3515488ad99412c37608b139ea63b27"}),
            },
            {
                // For use by test/functional/feature_assumeutxo.py
                .height = 299,
                .hash_serialized = AssumeutxoHash{uint256{"a4bf3407ccb2cc0145c49ebba8fa91199f8a3903daf0883875941497d2493c27"}},
                .m_chain_tx_count = 334,
                .blockhash = consteval_ctor(uint256{"3bb7ce5eba0be48939b7a521ac1ba9316afee2c7bada3a0cca24188e6d7d96c0"}),
            },
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "bcrt";
    }
};

std::unique_ptr<const CChainParams> CChainParams::SigNet(const SigNetOptions& options)
{
    return std::make_unique<const SigNetParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::RegTest(const RegTestOptions& options)
{
    return std::make_unique<const CRegTestParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::Main()
{
    return std::make_unique<const CMainParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet()
{
    return std::make_unique<const CTestNetParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet4()
{
    return std::make_unique<const CTestNet4Params>();
}

std::vector<int> CChainParams::GetAvailableSnapshotHeights() const
{
    std::vector<int> heights;
    heights.reserve(m_assumeutxo_data.size());

    for (const auto& data : m_assumeutxo_data) {
        heights.emplace_back(data.height);
    }
    return heights;
}

std::optional<ChainType> GetNetworkForMagic(const MessageStartChars& message)
{
    const auto mainnet_msg = CChainParams::Main()->MessageStart();
    const auto testnet_msg = CChainParams::TestNet()->MessageStart();
    const auto testnet4_msg = CChainParams::TestNet4()->MessageStart();
    const auto regtest_msg = CChainParams::RegTest({})->MessageStart();
    const auto signet_msg = CChainParams::SigNet({})->MessageStart();

    if (std::ranges::equal(message, mainnet_msg)) {
        return ChainType::MAIN;
    } else if (std::ranges::equal(message, testnet_msg)) {
        return ChainType::TESTNET;
    } else if (std::ranges::equal(message, testnet4_msg)) {
        return ChainType::TESTNET4;
    } else if (std::ranges::equal(message, regtest_msg)) {
        return ChainType::REGTEST;
    } else if (std::ranges::equal(message, signet_msg)) {
        return ChainType::SIGNET;
    }
    return std::nullopt;
}
