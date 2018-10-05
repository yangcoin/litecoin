// Copyright (c) 2018-2018 The QCityCoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "poo.h"

#include "chain.h"
#include "chainparams.h"
#include "clientversion.h"
#include "coins.h"
#include "hash.h"
#include "primitives/transaction.h"
#include "uint256.h"
#include "util.h"
#include "validation.h"
#include <stdio.h>
#include "consensus/consensus.h"
#include "wallet/wallet.h"
using namespace std;


bool GetBlockPublicKey(const CBlock& block, std::vector<unsigned char>& vchPubKey)
{
    if (block.IsProofOfWork())
        return false;

    if (block.vchBlockSig.empty())
        return false;

    
    vector<vector<unsigned char> > vSolutions;
    txnouttype whichType;
    
    const CTxOut& txout = block.IsProofOfOnline()? block.vtx[0]->vout[1]:block.vtx[1]->vout[1];

    if (!Solver(txout.scriptPubKey, whichType, vSolutions))
        return false;

    if (whichType == TX_PUBKEY)
    {
        vchPubKey = vSolutions[0];
        return true;
    }
    else
    {
        // Block signing key also can be encoded in the nonspendable output
        // This allows to not pollute UTXO set with useless outputs e.g. in case of multisig staking

        const CScript& script = txout.scriptPubKey;
        CScript::const_iterator pc = script.begin();
        opcodetype opcode;
        vector<unsigned char> vchPushValue;

        if (!script.GetOp(pc, opcode, vchPubKey))
            return false;
        if (opcode != OP_RETURN)
            return false;
        if (!script.GetOp(pc, opcode, vchPubKey))
            return false;
        if (!IsCompressedOrUncompressedPubKey(vchPubKey))
            return false;
        return true;
    }

    return false;
}

/**
 *  블럭의 서명을 확인한다.
 * 
 */
bool CheckBlockSignature(const CBlock& block)//, const uint256& hash)
{
    if (block.IsProofOfWork()){//online block 이 아니면
        
        return block.vchBlockSig.empty();
    }
    
    std::vector<unsigned char> vchPubKey;
    if(!GetBlockPublicKey(block, vchPubKey))
    {
        DbgMsg("GetBlockPubKey fail... ");
        return false;
    }
    
    if(block.IsProofOfOnline() ) {
        DbgMsg("must provided key..... !!!!!!!!!! TODOTODODO");
    }
    DbgMsg("check sign... ");
    return CPubKey(vchPubKey).Verify(block.GetHashWithoutSign(), block.vchBlockSig);
}

// Check kernel hash target and coinonline signature TODO
bool CheckProofOfOnline(CBlockIndex* pindexPrev, const CTransaction& tx, unsigned int nBits, CValidationState& state)
{
    // vin size()==0 and vin prevout == null and vout.size()==1 
    if (!tx.IsCoinOnline())
        return error("CheckProofOfOnline() : called on non-coinstake %s", tx.GetHash().ToString());

    // TODO check onlone...
    DbgMsg("/// TODO Check ProofOfOnline");
    return true;
}


// BlackCoin kernel protocol
// coinstake must meet hash target according to the protocol:
// kernel (input 0) must meet the formula
//     hash(nStakeModifier + blockFrom.nTime + txPrev.vout.hash + txPrev.vout.n + nTime) < bnTarget * nWeight
// this ensures that the chance of getting a coinstake is proportional to the
// amount of coins one owns.
// The reason this hash is chosen is the following:
//   nStakeModifier: scrambles computation to make it very difficult to precompute
//                   future proof-of-stake
//   blockFrom.nTime: slightly scrambles computation
//   txPrev.vout.hash: hash of txPrev, to reduce the chance of nodes
//                     generating coinstake at the same time
//   txPrev.vout.n: output number of txPrev, to reduce the chance of nodes
//                  generating coinstake at the same time
//   nTime: current timestamp
//   block/tx hash should not be used here as they can be generated in vast
//   quantities so as to generate blocks faster, degrading the system back into
//   a proof-of-work situation.
//
bool CheckOnlineKernelHash(CBlockIndex* pindexPrev, unsigned int nBits, uint32_t blockFromTime, CAmount prevoutValue, const COutPoint& prevout, unsigned int nTimeBlock, uint256& hashProofOfStake, uint256& targetProofOfStake, bool fPrintProofOfStake)
{
    if (nTimeBlock < blockFromTime)  // Transaction timestamp violation
        return error("CheckOnlineKernelHash() : nTime violation");

    // Base target
    arith_uint256 bnTarget;
    bnTarget.SetCompact(nBits);

    // Weighted target
    int64_t nValueIn = prevoutValue;
    arith_uint256 bnWeight = arith_uint256(nValueIn);
    bnTarget *= bnWeight;

    targetProofOfStake = ArithToUint256(bnTarget);

    uint256 nStakeModifier = pindexPrev->nStakeModifier;

    // Calculate hash
    CDataStream ss(SER_GETHASH, 0);
    ss << nStakeModifier;
    ss << blockFromTime << prevout.hash << prevout.n << nTimeBlock;
    hashProofOfStake = Hash(ss.begin(), ss.end());

    if (fPrintProofOfStake)
    {
        LogPrintf("CheckOnlineKernelHash() : check modifier=%s nTimeBlockFrom=%u nPrevout=%u nTimeBlock=%u hashProof=%s\n",
            nStakeModifier.GetHex().c_str(),
            blockFromTime, prevout.n, nTimeBlock,
            hashProofOfStake.ToString());
    }

    // Now check if proof-of-stake hash meets target protocol
    if (UintToArith256(hashProofOfStake) > bnTarget)
        return false;

     

    return true;
}


/**
 * 최소 비트를 확인한다.
 * 이전 블럭.
 * bit.
 * 이전에 사용한 코인.
 * 블럭시간
 */
bool CheckPoOKernel(const CBlockIndex *pindexPrev, unsigned int nBits, uint32_t nTime,  uint32_t *pBlockTime)
{
    // TODO 
    // 경쟁 관계를 만들어야 한다.
    // n개의 online miner 들간의 경쟁관계... 
    // 최소 시간...
    return true;
    // uint256 hashProofOfStake, targetProofOfStake;

    // CAmount amount = 0;
    // return CheckOnlineKernelHash(pindexPrev, nBits, *pBlockTime,
    //     amount, prevout, nTime, hashProofOfStake, targetProofOfStake);
}


// Check whether the coinstake timestamp meets protocol
bool CheckCoinOnlineTimestamp(int64_t nTimeBlock, int64_t nTimeTx)
{
    const Consensus::Params& params = Params().GetConsensus();

    if(! (nTimeBlock == nTimeTx) && ((nTimeTx & params.nStakeTimestampMask) == 0)){
        DbgMsg( "%d %d %d " ,nTimeBlock ,nTimeTx ,nTimeTx & params.nStakeTimestampMask);
        return false;
    }else{
        return true;
    }
}

// Simplified version of CheckCoinOnlineTimestamp() to check header-only timestamp
bool CheckCoinOnlineTimestamp(int64_t nTimeBlock)
{
    return CheckCoinOnlineTimestamp(nTimeBlock, nTimeBlock);
}