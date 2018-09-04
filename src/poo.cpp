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
// TODO
static bool CheckBlockSignature(const CBlock& block)//, const uint256& hash)
{
    if (block.IsProofOfOnline()){
        
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

    if (!Solver(txout.scriptPubKey, whichType, vSolutions)){ 
        LogPrintf("CheckBlockSignature: Bad Block (Pos) - wrong signature\n");
        return false;
    }

    if (whichType == TX_PUBKEY)
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

// Check kernel hash target and coinstake signature
bool CheckProofOfStake(CBlockIndex* pindexPrev, const CTransaction& tx, unsigned int nBits, CValidationState& state)
{
    if (!tx.IsCoinOnline())
        return error("CheckProofOfStake() : called on non-coinstake %s", tx.GetHash().ToString());

    // Kernel (input 0) must match the stake hash target per coin age (nBits)
    const CTxIn& txin = tx.vin[0];
    
    
    CTransactionRef txPrev;
    uint256 hashBlock = uint256();
    if (!GetTransaction(txin.prevout.hash, txPrev, Params().GetConsensus(), hashBlock, true))
        return error("CheckProofOfStake() : INFO: read txPrev failed %s",txin.prevout.hash.GetHex());
    

    CDiskTxPos txindex;

    // Verify signature
    if (!VerifySignature(*txPrev, tx, 0, SCRIPT_VERIFY_NONE, 0))
        return state.DoS(100, error("CheckProofOfStake() : VerifySignature failed on coinstake %s", tx.GetHash().ToString()));
 

    if (mapBlockIndex.count(hashBlock) == 0)
        return fDebug? error("CheckProofOfStake() : read block failed") : false; // unable to read block of previous transaction

    
    CBlockIndex* pblockindex = mapBlockIndex[hashBlock];
    if (!CheckStakeKernelHash(pindexPrev, nBits, *pblockindex , new CCoins(*txPrev, pindexPrev->nHeight), txin.prevout, tx.nTime))
        return state.DoS(1, error("CheckProofOfStake() : INFO: check kernel failed on coinstake %s", tx.GetHash().ToString())); // may occur during initial download or if behind on block chain sync

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
bool CheckStakeKernelHash(const CBlockIndex* pindexPrev, 
                            unsigned int nBits,  
                            CBlockIndex& blockFrom, 
                            const CCoins* txPrev, 
                            const COutPoint& prevout, 
                            unsigned int nTimeTx)
{
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

    return true;
}