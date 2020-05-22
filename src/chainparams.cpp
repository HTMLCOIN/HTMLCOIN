// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>

#include <chainparamsseeds.h>
#include <consensus/merkle.h>
#include <consensus/consensus.h>
#include <key_io.h>
#include <script/standard.h>
#include <tinyformat.h>
#include <util/system.h>
#include <util/strencodings.h>
#include <util/convert.h>
#include <versionbitsinfo.h>

#include <assert.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

///////////////////////////////////////////// // qtum
#include <libdevcore/SHA3.h>
#include <libdevcore/RLP.h>
#include "arith_uint256.h"
/////////////////////////////////////////////

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 00 << 488804799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
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
    genesis.hashStateRoot = uint256(h256Touint(dev::h256("e965ffd002cd6ad0e2dc402b8044de833e06b23127ea8c3d80aec91410771495"))); // qtum
    genesis.hashUTXORoot = uint256(h256Touint(dev::sha3(dev::rlp("")))); // qtum
    return genesis;
}

static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "BBC 9/24/2017 Germany election Merkel wins fourth term";
    const CScript genesisOutputScript = CScript() << ParseHex("04e67225ab32299deaf6312b5b77f0cd2a5264f3757c9663f8dc401ff8b3ad8b012fde713be690ab819f977f84eaef078767168aeb1cb1287941b6319b76d8e582") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 7680000; // HTMLCOIN halving every 14.6 years
        consensus.BIP16Exception = uint256();
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 0;
        consensus.BIP66Height = 0;
        consensus.CSVHeight = 60480;
        consensus.SegwitHeight = 60480;
        consensus.MinBIP9WarningHeight = 80640;
        consensus.QIP5Height = 1277000;
        consensus.QIP6Height = 1277000;
        consensus.QIP7Height = 1277000;
        consensus.QIP9Height = 1277000;
        consensus.powLimit = uint256S("0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.QIP9PosLimit = uint256S("0000000000001fffffffffffffffffffffffffffffffffffffffffffffffffff"); // The new POS-limit activated after QIP9
        consensus.nPosTargetTimespan = 15 * 60;
        consensus.nPosTargetTimespanV2 = 4000;
        consensus.nPowTargetTimespan = 120;
        consensus.nPowTargetSpacing = 120;
        consensus.checkpointPubKey = "041dd4a81665947a5f077732779b8bc304f5425f2977a99ef6d58ccd71967661b8b778db9f542778fbd7354563524272ab764ee8feab08240b4ebd324c33cd80d5";
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.fPoSNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 15120; // 95% of 2016
        consensus.nMinerConfirmationWindow = 20160; // nPowTargetTimespan / nPowTargetSpacing
        consensus.nDiffAdjustChange = 7700;
        consensus.nDiffDamping = 106000;
        consensus.nDiffChange = 626000;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000007c1000e6995955fe7f91a");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00000000000f9073f18564f8d0c3059e61d0617735bc500d9837799a0354f8d4");

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0x1f;
        pchMessageStart[1] = 0x2e;
        pchMessageStart[2] = 0x3d;
        pchMessageStart[3] = 0x4c;
        nDefaultPort = 4888;
        nPruneAfterHeight = 100000;
        m_assumed_blockchain_size = 2;
        m_assumed_chain_state_size = 1;

        genesis = CreateGenesisBlock(1506211200, 94371, 0x1f00ffff, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x0000bf23c6424c270a24a17a3db723361c349e0f966d7b55a6bca4bfb2d951b0"));
        assert(genesis.hashMerkleRoot == uint256S("0xb07b60977e6f1ebfc23c074fb319c654e38dba5d7db16902863a4a98dd981f68"));

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as a oneshot if they don't support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.
        vSeeds.emplace_back("seed1.htmlcoin.com"); // mainnet
        vSeeds.emplace_back("seed2.htmlcoin.com"); // mainnet
        vSeeds.emplace_back("seed3.htmlcoin.com"); // mainnet
        vSeeds.emplace_back("seed4.htmlcoin.com"); // mainnet

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,41);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,100);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,169);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x13, 0x97, 0xC1, 0x0D};
        base58Prefixes[EXT_SECRET_KEY] = {0x13, 0x97, 0xBC, 0xF3};

        bech32_hrp = "hc";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        m_is_test_chain = false;

        checkpointData = {
            {
                {       0, uint256S("0000bf23c6424c270a24a17a3db723361c349e0f966d7b55a6bca4bfb2d951b0")},
                {     798, uint256S("00002847d05b6fe46570b754815309123bedcb84a5ac2ae58fa1d38957ccb772")},
                {  211401, uint256S("00000000000a2142cf5781b89170e7fd2d1fb22b92a7f3878e8199378e32a54b")},
                {  308971, uint256S("000000000002e13479422a602499ceff5699ae3bb21bc5ebf2b12257d3da7b4e")},
                {  526446, uint256S("00000000000eb8bd8570a6249d2592a2747bb1c8d3f9f1a7e9d668353825e7b2")},
                {  950000, uint256S("1cceb9762ceae24c7a1e5b67e6e22122f561fc95effee4ae94a730763b04c10c")},
                { 1231400, uint256S("00000000000f9073f18564f8d0c3059e61d0617735bc500d9837799a0354f8d4")},
            }
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 00000000000f9073f18564f8d0c3059e61d0617735bc500d9837799a0354f8d4
            1589500983, // * UNIX timestamp of last known number of transactions
            2198623, // * total number of transactions between genesis and that timestamp
            //   (the tx=... number in the SetBestChain debug.log lines)
            0.02387811940904145 // * estimated number of transactions per second after that timestamp
        };

        consensus.nFixUTXOCacheHFHeight=251000;
        consensus.nEnableHeaderSignatureHeight = 997655;
        consensus.nCheckpointSpan = COINBASE_MATURITY;
    }
};

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 7680000; // HTMLCOIN halving every 14.6 years
        consensus.BIP16Exception = uint256();
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 0;
        consensus.BIP66Height = 0;
        consensus.CSVHeight = 60480;
        consensus.SegwitHeight = 60480;
        consensus.MinBIP9WarningHeight = 80640;
        consensus.QIP5Height = std::numeric_limits<int>::max();
        consensus.QIP6Height = std::numeric_limits<int>::max();
        consensus.QIP7Height = std::numeric_limits<int>::max();
        consensus.QIP9Height = std::numeric_limits<int>::max();
        consensus.powLimit = uint256S("0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.QIP9PosLimit = uint256S("0000000000001fffffffffffffffffffffffffffffffffffffffffffffffffff"); // The new POS-limit activated after QIP9
        consensus.nPosTargetTimespan = 15 * 60;
        consensus.nPosTargetTimespanV2 = 4000;
        consensus.nPowTargetTimespan = 10; // 16 minutes
        consensus.nPowTargetSpacing = 10;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.fPoSNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.nDiffAdjustChange = 0;
        consensus.nDiffDamping = 0;
        consensus.nDiffChange = -1;
        consensus.checkpointPubKey = "0480459f232f2d35f250708d162571a0450bc5b56967c721970a0f53bd74ae2977c16d87fae1b4e4b5cc0e90584cd3c38b6e0e2bfb8c123e750b3007c62fabe78b";
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0x2f;
        pchMessageStart[1] = 0x3e;
        pchMessageStart[2] = 0x4d;
        pchMessageStart[3] = 0x5c;
        nDefaultPort = 14888;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 1;
        m_assumed_chain_state_size = 1;

        genesis = CreateGenesisBlock(1506212200, 102232, 0x1f00ffff, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x000013694772f8aeb88efeb2829fe5d71fbca3e23d5043baa770726f204f528c"));
        assert(genesis.hashMerkleRoot == uint256S("0xb07b60977e6f1ebfc23c074fb319c654e38dba5d7db16902863a4a98dd981f68"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("testnet-seed1.htmlcoin.com"); // Qtum testnet

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,100);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,110);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tq";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        m_is_test_chain = true;


        checkpointData = {
            {
                {1146000, uint256S("9920f63f4fe6d1ee164b0313f702405d790440357b4cfd7e9242a960ac16275b")},
            }
        };

        chainTxData = ChainTxData{
            // Data as of block 9920f63f4fe6d1ee164b0313f702405d790440357b4cfd7e9242a960ac16275b (height 1146000)
            1571178976,
            2139045,
            0.03200413608657793
        };

        consensus.nFixUTXOCacheHFHeight=340480;
        consensus.nEnableHeaderSignatureHeight = 1169889;
        consensus.nCheckpointSpan = COINBASE_MATURITY;
    }
};

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    explicit CRegTestParams(const ArgsManager& args) {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP16Exception = uint256();
        consensus.BIP34Height = 0; // BIP34 activated on regtest (Used in functional tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 0; // BIP65 activated on regtest (Used in functional tests)
        consensus.BIP66Height = 0; // BIP66 activated on regtest (Used in functional tests)
        consensus.CSVHeight = 432; // CSV activated on regtest (Used in rpc activation tests)
        consensus.SegwitHeight = 0; // SEGWIT is always activated on regtest unless overridden
        consensus.MinBIP9WarningHeight = 0;
        consensus.QIP5Height = 0;
        consensus.QIP6Height = 0;
        consensus.QIP7Height = 0;
        consensus.QIP9Height = 0;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.QIP9PosLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // The new POS-limit activated after QIP9
        consensus.nPosTargetTimespan = 15 * 60;
        consensus.nPosTargetTimespanV2 = 4000;
        consensus.nPowTargetTimespan = 60;
        consensus.nPowTargetSpacing = 60;
        consensus.nPowTargetSpacing = 60;
        consensus.checkpointPubKey = "048b4b3a2b6a072b0bfa7986d6ce73e2b9896a8559fca2063d4cabb83e05e9b8268cb00b8229edb4a2fd15fa32bd2238102268cc96b52e6727a4b23c339d5908ff";
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.fPoSNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.nDiffAdjustChange = 0;
        consensus.nDiffDamping = 0;
        consensus.nDiffChange = 1100;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0x3f;
        pchMessageStart[1] = 0x4e;
        pchMessageStart[2] = 0x5d;
        pchMessageStart[3] = 0x6c;
        nDefaultPort = 24888;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        UpdateActivationParametersFromArgs(args);

        genesis = CreateGenesisBlock(1506213200, 2, 0x207fffff, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x03c80d2399e1fe481a51e122ac55159a4e5fe635494a7fd368f3e440241fccb2"));
        assert(genesis.hashMerkleRoot == uint256S("0xb07b60977e6f1ebfc23c074fb319c654e38dba5d7db16902863a4a98dd981f68"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = true;
        m_is_test_chain = true;

        checkpointData = {
            {
                {0, uint256S("03c80d2399e1fe481a51e122ac55159a4e5fe635494a7fd368f3e440241fccb2")},
            }
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        consensus.nFixUTXOCacheHFHeight=0;
        consensus.nEnableHeaderSignatureHeight = 0;
        consensus.nCheckpointSpan = COINBASE_MATURITY;

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,120);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,110);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "qcrt";
    }

    /**
     * Allows modifying the Version Bits regtest parameters.
     */
    void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }
    void UpdateActivationParametersFromArgs(const ArgsManager& args);
};

void CRegTestParams::UpdateActivationParametersFromArgs(const ArgsManager& args)
{
    if (gArgs.IsArgSet("-segwitheight")) {
        int64_t height = gArgs.GetArg("-segwitheight", consensus.SegwitHeight);
        if (height < -1 || height >= std::numeric_limits<int>::max()) {
            throw std::runtime_error(strprintf("Activation height %ld for segwit is out of valid range. Use -1 to disable segwit.", height));
        } else if (height == -1) {
            LogPrintf("Segwit disabled for testing\n");
            height = std::numeric_limits<int>::max();
        }
        consensus.SegwitHeight = static_cast<int>(height);
    }

    if (!args.IsArgSet("-vbparams")) return;

    for (const std::string& strDeployment : args.GetArgs("-vbparams")) {
        std::vector<std::string> vDeploymentParams;
        boost::split(vDeploymentParams, strDeployment, boost::is_any_of(":"));
        if (vDeploymentParams.size() != 3) {
            throw std::runtime_error("Version bits parameters malformed, expecting deployment:start:end");
        }
        int64_t nStartTime, nTimeout;
        if (!ParseInt64(vDeploymentParams[1], &nStartTime)) {
            throw std::runtime_error(strprintf("Invalid nStartTime (%s)", vDeploymentParams[1]));
        }
        if (!ParseInt64(vDeploymentParams[2], &nTimeout)) {
            throw std::runtime_error(strprintf("Invalid nTimeout (%s)", vDeploymentParams[2]));
        }
        bool found = false;
        for (int j=0; j < (int)Consensus::MAX_VERSION_BITS_DEPLOYMENTS; ++j) {
            if (vDeploymentParams[0] == VersionBitsDeploymentInfo[j].name) {
                UpdateVersionBitsParameters(Consensus::DeploymentPos(j), nStartTime, nTimeout);
                found = true;
                LogPrintf("Setting version bits activation parameters for %s to start=%ld, timeout=%ld\n", vDeploymentParams[0], nStartTime, nTimeout);
                break;
            }
        }
        if (!found) {
            throw std::runtime_error(strprintf("Invalid deployment (%s)", vDeploymentParams[0]));
        }
    }
}

/**
 * Regression network parameters overwrites for unit testing
 */
class CUnitTestParams : public CRegTestParams
{
public:
    explicit CUnitTestParams(const ArgsManager& args)
    : CRegTestParams(args)
    {
        // Activate the the BIPs for regtest as in Bitcoin
        consensus.BIP16Exception = uint256();
        consensus.BIP34Height = 100000000; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1351; // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 1251; // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.QIP6Height = 1000;
        consensus.QIP7Height = 0; // QIP7 activated on regtest

        // QTUM have 500 blocks of maturity, increased values for regtest in unit tests in order to correspond with it
        consensus.nSubsidyHalvingInterval = 750;
        consensus.nRuleChangeActivationThreshold = 558; // 75% for testchains
        consensus.nMinerConfirmationWindow = 744; // Faster than normal for regtest (744 instead of 2016)
        consensus.nCheckpointSpan = 1000; // Increase the check point span for the reorganization tests from 500 to 1000
    }
};

static std::unique_ptr<const CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<const CChainParams> CreateChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams(gArgs));
    else if (chain == CBaseChainParams::UNITTEST)
        return std::unique_ptr<CChainParams>(new CUnitTestParams(gArgs));
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}

std::string CChainParams::EVMGenesisInfo(dev::eth::Network network) const
{
    std::string genesisInfo = dev::eth::genesisInfo(network);
    ReplaceInt(consensus.QIP7Height, "QIP7_STARTING_BLOCK", genesisInfo);
    ReplaceInt(consensus.QIP6Height, "QIP6_STARTING_BLOCK", genesisInfo);
    return genesisInfo;
}

std::string CChainParams::EVMGenesisInfo(dev::eth::Network network, int nHeight) const
{
    std::string genesisInfo = dev::eth::genesisInfo(network);
    ReplaceInt(nHeight, "QIP7_STARTING_BLOCK", genesisInfo);
    ReplaceInt(nHeight, "QIP6_STARTING_BLOCK", genesisInfo);
    return genesisInfo;
}

void CChainParams::UpdateOpSenderBlockHeight(int nHeight)
{
    consensus.QIP5Height = nHeight;
}

void UpdateOpSenderBlockHeight(int nHeight)
{
    const_cast<CChainParams*>(globalChainParams.get())->UpdateOpSenderBlockHeight(nHeight);
}

void CChainParams::UpdateBtcEcrecoverBlockHeight(int nHeight)
{
    consensus.QIP6Height = nHeight;
}

void UpdateBtcEcrecoverBlockHeight(int nHeight)
{
    const_cast<CChainParams*>(globalChainParams.get())->UpdateBtcEcrecoverBlockHeight(nHeight);
}

void CChainParams::UpdateConstantinopleBlockHeight(int nHeight)
{
    consensus.QIP7Height = nHeight;
}

void UpdateConstantinopleBlockHeight(int nHeight)
{
    const_cast<CChainParams*>(globalChainParams.get())->UpdateConstantinopleBlockHeight(nHeight);
}

void CChainParams::UpdateDifficultyChangeBlockHeight(int nHeight)
{
    consensus.nSubsidyHalvingInterval = 985500; // qtum halving every 4 years
    consensus.posLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    consensus.QIP9Height = nHeight;
    consensus.fPowAllowMinDifficultyBlocks = false;
    consensus.fPowNoRetargeting = true;
    consensus.fPoSNoRetargeting = false;
}

void UpdateDifficultyChangeBlockHeight(int nHeight)
{
    const_cast<CChainParams*>(globalChainParams.get())->UpdateDifficultyChangeBlockHeight(nHeight);
}

CScript CChainParams::GetRewardScript() const {
    CTxDestination destination;
    if (Params().NetworkIDString() == CBaseChainParams::MAIN)
        destination = DecodeDestination("HXsXRP1smr1pgb23eYV1fjN6ZB8EWfXj6J");

    assert(IsValidDestination(destination));
    return GetScriptForDestination(destination);
}
