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
/**
 *  블럭의 서명을 확인한다.
 * 
 */
static bool CheckBlockSignature(const CBlock& block)//, const uint256& hash)
{
    if (!block.IsProofOfOnline()){//online block 이 아니면
        
        return block.vchBlockSig.empty();
    }

    if (block.vchBlockSig.empty()){
        LogPrintf(" POS BLOCK \n%s" , block.ToString());
        LogPrintf("CheckBlockSignature: Bad Block (Pos)- vchBlockSig empty\n");
        return false;
    }

    vector<vector<unsigned char> > vSolutions;
    txnouttype whichType;

    const CTxOut& txout = block.vtx[1]->vout[1];

    // 출력필드의 공개키값을 확인한다.
    if (!Solver(txout.scriptPubKey, whichType, vSolutions)){ 
        LogPrintf("CheckBlockSignature: Bad Block (Pos) - wrong signature\n");
        return false;
    }

    if (whichType == TX_PUBKEY) //공개키이면 블럭의 서명자와 
    {
        vector<unsigned char>& vchPubKey = vSolutions[0];
        return CPubKey(vchPubKey).Verify(block.GetHash(), block.vchBlockSig);
        return true;
        //return CPubKey(vchPubKey).Verify(hash, block.vchBlockSig);
    }
    else
    {
        // Block signing key also can be encoded in the nonspendable output
        // This allows to not pollute UTXO set with useless outputs e.g. in case of multisig staking

        const CScript& script = txout.scriptPubKey;
        CScript::const_iterator pc = script.begin();
        opcodetype opcode;
        vector<unsigned char> vchPushValue;

        if (!script.GetOp(pc, opcode, vchPushValue))
            return false;
        if (opcode != OP_RETURN)
            return false;
        if (!script.GetOp(pc, opcode, vchPushValue))
            return false;
        // if (!IsCompressedOrUncompressedPubKey(vchPushValue))
        //     return false;
        return true;
        //return CPubKey(vchPushValue).Verify(hash, block.vchBlockSig);
    }

    return false;
}

// Check kernel hash target and coinonline signature
bool CheckProofOfOnline(CBlockIndex* pindexPrev, const CTransaction& tx, unsigned int nBits, CValidationState& state)
{
    // vin size()==0 and vin prevout == null and vout.size()==1 
    if (!tx.IsCoinOnline())
        return error("CheckProofOfOnline() : called on non-coinstake %s", tx.GetHash().ToString());

    // Kernel (input 0) must match the stake hash target per coin age (nBits)
    const CTxIn& txin = tx.vin[0];
    
    
    CTransactionRef txPrev;
    
    CDiskTxPos txindex;

    
    
    // CBlockIndex* pblockindex = mapBlockIndex[hashBlock];
    // if (!CheckStakeKernelHash(pindexPrev, nBits, *pblockindex , new CCoins(*txPrev, pindexPrev->nHeight), txin.prevout, tx.nTime))
    //     return state.DoS(1, error("CheckProofOfOnline() : INFO: check kernel failed on coinstake %s", tx.GetHash().ToString())); // may occur during initial download or if behind on block chain sync

    return true;
}

bool VerifySignature(const CTransaction& txFrom, const CTransaction& txTo, unsigned int nIn, unsigned int flags, int nHashType)
{
    assert(nIn < txTo.vin.size());
    const CTxIn& txin = txTo.vin[nIn];
    if (txin.prevout.n >= txFrom.vout.size())
        return false;
    const CTxOut& txout = txFrom.vout[txin.prevout.n];

    if (txin.prevout.hash != txFrom.GetHash())
        return false;

    return VerifyScript(txin.scriptSig, txout.scriptPubKey, &txin.scriptWitness, flags, TransactionSignatureChecker(&txTo, nIn, txout.nValue), NULL);
}

// BlackCoin kernel protocol v3
// coinstake must meet hash target according to the protocol:
// kernel (input 0) must meet the formula
//     hash(nStakeModifier + txPrev.nTime + txPrev.vout.hash + txPrev.vout.n + nTime) < bnTarget * nWeight
// this ensures that the chance of getting a coinstake is proportional to the
// amount of coins one owns.
// The reason this hash is chosen is the following:
//   nStakeModifier: scrambles computation to make it very difficult to precompute
//                   future proof-of-stake
//   txPrev.nTime: slightly scrambles computation
//   txPrev.vout.hash: hash of txPrev, to reduce the chance of nodes
//                     generating coinstake at the same time
//   txPrev.vout.n: output number of txPrev, to reduce the chance of nodes
//                  generating coinstake at the same time
//   nTime: current timestamp
//   block/tx hash should not be used here as they can be generated in vast
//   quantities so as to generate blocks faster, degrading the system back into
//   a proof-of-work situation.
//
// bool CheckStakeKernelHash(const CBlockIndex* pindexPrev, 
//                             unsigned int nBits,  
//                             CBlockIndex& blockFrom, 
//                             const CCoins* txPrev, 
//                             const COutPoint& prevout, 
//                             unsigned int nTimeTx)
// {
    // // Weight
    // int64_t nValueIn = txPrev->vout[prevout.n].nValue;
    // if (nValueIn == 0)
    //     return false;
    // if (blockFrom.GetBlockTime() + Params().GetConsensus().nStakeMinAge > nTimeTx){ // Min age requirement
    //     DbgMsg("too early? %d %d %d gap:%d" , blockFrom.GetBlockTime() ,Params().GetConsensus().nStakeMinAge ,nTimeTx,
    //             blockFrom.GetBlockTime() + Params().GetConsensus().nStakeMinAge - nTimeTx);
    //     return error("CheckStakeKernelHash() : min age violation");
    // }
    // // Base target
    // arith_uint256 bnTarget;
    // bnTarget.SetCompact(nBits);

    // // Calculate hash
    // CHashWriter ss(SER_GETHASH, 0);
    // ss << pindexPrev->nStakeModifier << txPrev->nTime << prevout.hash << prevout.n << nTimeTx;
    // uint256 hashProofOfStake = ss.GetHash();

    // // Now check if proof-of-stake hash meets target protocol
    // if (UintToArith256(hashProofOfStake) / nValueIn > bnTarget)
    //     return false;

//     return true;
// }

/**
 * 최소 비트를 확인한다.
 * 이전 블럭.
 * bit.
 * 이전에 사용한 코인.
 * 블럭시간
 */
bool CheckKernel(const CBlockIndex *pindexPrev, unsigned int nBits, uint32_t nTime,  uint32_t *pBlockTime)
{
    // TODO 
    // 경쟁 관계를 만들어야 한다.
    // n개의 online miner 들간의 경쟁관계... 
    // 최소 시간...
    return true;
    // uint256 hashProofOfStake, targetProofOfStake;

    // CAmount amount = 0;
    // return CheckStakeKernelHash(pindexPrev, nBits, *pBlockTime,
    //     amount, prevout, nTime, hashProofOfStake, targetProofOfStake);
}
