// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"
#include "arith_uint256.h"
#include "util.h"
#include "base58.h"
bool fSearchGenesis = true;

void searchGenesis(CBlock genesis){
    printf("Start Search Genesis\n");
    arith_uint256 hashTarget = arith_uint256().SetCompact(genesis.nBits);
    unsigned int startTime =  genesis.nTime;
    unsigned int startNonce =  genesis.nNonce;
    
    uint256 thash;
    while(true){
        thash = genesis.GetPoWHash();
        
        if(UintToArith256(thash)<hashTarget){
            printf("found...\n");
            break;
        }
        if(genesis.nNonce%5000==0){
            printf("nonce %08X: hash = %s (target = %s)\n", genesis.nNonce, thash.ToString().c_str(), hashTarget.ToString().c_str());
    
        }
        ++genesis.nNonce;
        if (genesis.nNonce == 0)
        {
            printf("NONCE WRAPPED, incrementing time\n");
            ++genesis.nTime;
        }
    }
    
    printf(" start block.nTime = %u \n",startTime );
    printf(" start block.nNonce = %u \n", startNonce);
    printf("block.nTime = %u \n", genesis.nTime);
    printf("block.nNonce = %u \n", genesis.nNonce);
    printf("block.GetHash = 0x%s\n", genesis.GetHash().ToString().c_str());
    printf("block.PoWHash = 0x%s\n", genesis.GetPoWHash().ToString().c_str());
    printf("block.hashMerkleRoot = 0x%s\n",   genesis.hashMerkleRoot.ToString().c_str());
    exit(0);
}
static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nTime = nTime;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 0 << CScriptNum(42) 
                                    << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].SetEmpty();                                    
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
    const char* pszTimestamp = "yangchigi.com regist at 7/10/2000 ... WOW!!!";
    const CScript genesisOutputScript = CScript() << 
        ParseHex("041f989cea5bb6e240acc9d417c7fcfa11ff771fd8e16481d5c164ffa4fc0968c4732834682d605f05c451d93f61508734b7632530db36e688963921daea7db924") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        uint32_t GEN_TIME = 1497484800;
        unsigned int NOUNCE = 39035;
        consensus.BIP34Height = 0;
        consensus.BIP65Height = 0; // bab3041e8977e0dc3eeff63fe707b92bde1dd449d8efafb248c27c8264cc311a
        consensus.BIP66Height = 0; // 7aceee012833fa8952f8835d8b1b3ae233cd6ab08fdb27a771d2bd7bdc491894
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); 
        consensus.nPowTargetTimespan = 1 * 1 * 60 * 60; // 1 hour  sec
        consensus.nPowTargetSpacing = 1 * 60;//1min
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 6048; // 75% of 8064
        consensus.nMinerConfirmationWindow = 5760; // nPowTargetTimespan / nPowTargetSpacing * 4
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 0; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0; // January 28, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL; // January 31st, 2018
        
        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 0; // January 28, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 0; // January 31st, 2018

        // The best chain should have at least this much work.
        // consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000000006805c7318ce2736c0");
        consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000000000000000620c097e54"); //
        // By default assume that the signatures in ancestors of this block are valid.
        //consensus.defaultAssumeValid = uint256S("0x1673fa904a93848eca83d5ca82c7af974511a7e640e22edc2976420744f2e56a"); //1155631
        consensus.defaultAssumeValid = uint256S("0x76b274d801f169653582571b51ef425298b9bcbdbf8c871c760886da4702b03a");//157000

        consensus.nStakeTimestampMask = 0xf; // 10
        consensus.nProofOfOnlineInterval = 10;// 
        consensus.nStakeMinAge = 8 * 60 * 60; // 8 hours


        consensus.POO_START_TIME = 1539583200; // 10/15/2018 @ 6:00:00   gmt +9( seoul 10/15/2018 15:00:00)
        consensus.POS_START_TIME = 1539680400; // 10/15/2018 @ 9:00:00   gmt +9( seoul 10/15/2018 18:00:00)
        
        
        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0x59;//
        pchMessageStart[1] = 0x41;//
        pchMessageStart[2] = 0x4e;//
        pchMessageStart[3] = 0x47;//
        nDefaultPort = 23001;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(GEN_TIME,NOUNCE, 0x1e0fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        uint256 hashGenesis = uint256S("0xbf36a3266be55cff28ff429bc066ac80fc2d05cb395ecef14dbb72d221395bbe");
        uint256 hashMerkelRoot = uint256S("0xb27efd689036a672667617d484edcd6be1a5778b7623dd42680d6eade89c2ad7");
        if(hashGenesis !=genesis.GetHash()){
            genesis.nNonce = 0;
            searchGenesis(genesis);
        }
        assert(consensus.hashGenesisBlock == hashGenesis);
        assert(genesis.hashMerkleRoot == hashMerkelRoot);

        vSeeds.clear();
        // Note that of those with the service bits flag, most only support a subset of possible options
        vSeeds.push_back(CDNSSeedData("seed", "seed-yng.bitchk.com",true));
        
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,78);//Y
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,140);//y
        
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,204);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        vOnlinePubKeys = std::vector<CBitcoinAddress *>();
        
        for(int i =0;i<ARRAYLEN( pnSeedOnline_test);i++){
            CBitcoinAddress* addr = new CBitcoinAddress(pnSeedOnline_main[i]);
            vOnlinePubKeys.push_back( addr);
        }

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (  0, uint256S("0xbf36a3266be55cff28ff429bc066ac80fc2d05cb395ecef14dbb72d221395bbe"))
            (  70000, uint256S("0xc40402d16ab1df7cc0a527b297e9e2754c8f9f902d7d696af56fe446066af824"))
            (  157000, uint256S("0x76b274d801f169653582571b51ef425298b9bcbdbf8c871c760886da4702b03a"))
        };

        chainTxData = ChainTxData{
            // Data as of block b44bc5ae41d1be67227ba9ad875d7268aa86c965b1d64b47c35be6e8d5c352f4 (height 1155626).
            GEN_TIME, // * UNIX timestamp of last known number of transactions
            0,  // * total number of transactions between genesis and that timestamp
                    //   (the tx=... number in the SetBestChain debug.log lines)
            0.00     // * estimated number of transactions per second after that timestamp
        };
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        uint32_t GEN_TIME = 1497484801;
        unsigned int NOUNCE = 2061590;
        consensus.BIP34Height = 0;
        // consensus.BIP34Hash = uint256S("8075c771ed8b495ffd943980a95f702ab34fce3c8c54e379548bda33cc8c0573");
        consensus.BIP65Height = 0; // 8075c771ed8b495ffd943980a95f702ab34fce3c8c54e379548bda33cc8c0573
        consensus.BIP66Height = 0; // 8075c771ed8b495ffd943980a95f702ab34fce3c8c54e379548bda33cc8c0573
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 1 * 1 * 60 * 60; // 1 hour  sec
        consensus.nPowTargetSpacing = 1 * 60;//1min
        
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0; // January 1, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL; // January 31st, 2018

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1; // January 1, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 0; // January 31st, 2018

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000001f1311043f");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0xec08e3f1961ffe8b15158c074301943ebae0aad099107b2faed4055c7c47ab74"); //8711
        
        // poo interval and timestamp mask
        consensus.nStakeTimestampMask = 0xf; // 15
        consensus.nProofOfOnlineInterval = 6;// 7 block interval every n block poo;  if n ==2  height 000,002,004, ... , 012, 014.
        consensus.nStakeMinAge = 1 * 60 * 60; // 1 hours

        consensus.POO_START_TIME = 1537237800 ;
        consensus.POS_START_TIME = 1538629136;
        
        pchMessageStart[0] = 0x79;
        pchMessageStart[1] = 0x61;
        pchMessageStart[2] = 0x6e;
        pchMessageStart[3] = 0x67;
        nDefaultPort = 23011;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(GEN_TIME,NOUNCE, 0x1e0fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        uint256 hashGenesis = uint256S("0x096ce3bbe32a7d43e1242feb97f517cd4506ec3eb03126dc6e8c34e6d3b18e20");
        uint256 hashMerkelRoot = uint256S("0x6b9ff8837771c47b088ac9304d01667a2f976a7e94531b5a77b4ddeb9a6a19fe");
        if(hashGenesis !=genesis.GetHash()){
            genesis.nNonce = 0;
            searchGenesis(genesis);
        }
        assert(consensus.hashGenesisBlock == hashGenesis);
        assert(genesis.hashMerkleRoot == hashMerkelRoot);

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.push_back(CDNSSeedData("dns001", "yang001.bitchk.com",true));
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,80);//Y
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,142);//y
        
        
        vOnlinePubKeys = std::vector<CBitcoinAddress *>();
        
        for(int i =0;i<ARRAYLEN( pnSeedOnline_test);i++){
            CBitcoinAddress* addr = new CBitcoinAddress(pnSeedOnline_test[i]);
            vOnlinePubKeys.push_back( addr);
        }

        
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (  0, uint256S("0x096ce3bbe32a7d43e1242feb97f517cd4506ec3eb03126dc6e8c34e6d3b18e20"))
            (  7733, uint256S("0xec08e3f1961ffe8b15158c074301943ebae0aad099107b2faed4055c7c47ab74"))
        };

        chainTxData = ChainTxData{
            // Data as of block b44bc5ae41d1be67227ba9ad875d7268aa86c965b1d64b47c35be6e8d5c352f4 (height 1155626).
            GEN_TIME, // * UNIX timestamp of last known number of transactions
            0,  // * total number of transactions between genesis and that timestamp
                    //   (the tx=... number in the SetBestChain debug.log lines)
            0.00     // * estimated number of transactions per second after that timestamp
        };

    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        uint32_t GEN_TIME = 1497484802;
        unsigned int NOUNCE = 164116;
        consensus.BIP34Height = 0; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        // consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 0; // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 0; // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 3.5 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 2.5 * 60;
     //   consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 0;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        consensus.nStakeTimestampMask = 0xf; // 10
        consensus.nProofOfOnlineInterval = 10;// 
        consensus.nStakeMinAge = 8 * 60 * 60; // 8 hours

        consensus.POO_START_TIME = 1537237800 ;
        consensus.POS_START_TIME = 1538629136;
        
        
        pchMessageStart[0] = 0x79;
        pchMessageStart[1] = 0x61;
        pchMessageStart[2] = 0x6e;
        pchMessageStart[3] = 0x67;
        nDefaultPort = 23021;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(GEN_TIME,NOUNCE, 0x1e0fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        uint256 hashGenesis = uint256S("0xa31f431eceadaa4be8056c888a5e53ff9f88aadb51f337f323a9b6fd3ceb4505");
        uint256 hashMerkelRoot = uint256S("0x9a95652dd2cbbe103550e625d35cb5bf955c0754596a12afc4b479869d47ac8d");
        if(hashGenesis !=genesis.GetHash()){
            genesis.nNonce = 0;
            searchGenesis(genesis);
        }
        assert(consensus.hashGenesisBlock == hashGenesis);
        assert(genesis.hashMerkleRoot == hashMerkelRoot);

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true; 

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("0xa31f431eceadaa4be8056c888a5e53ff9f88aadb51f337f323a9b6fd3ceb4505"))
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,80);//C
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,142);//c
        
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
    }

    void UpdateBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
            return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
            return testNetParams;
    else if (chain == CBaseChainParams::REGTEST)
            return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

void UpdateRegtestBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    regTestParams.UpdateBIP9Parameters(d, nStartTime, nTimeout);
}
