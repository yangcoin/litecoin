// Copyright (c) 2018-2018 The YangCoin Core developer yangchigi@yangchigi.com
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_POO_H
#define BITCOIN_POO_H
#include "txdb.h"
#include "validation.h"
#include "arith_uint256.h"
#include "consensus/validation.h"
#include "hash.h"
#include "timedata.h"
#include "chainparams.h"
#include "script/sign.h"
#include <stdint.h>

using namespace std;

/**
 * signed by foundation pubkey
 */
bool isSigedByFoundation(CBlock block);
/**
 * sign fundation private key
 */
bool signBlock();

/**
 * need PoO block
 */
bool checkNeedPoO();


struct CStakeCache{
    CStakeCache(CBlockHeader blockFrom_, CDiskTxPos txindex_, const CTransaction txPrev_) : blockFrom(blockFrom_), txindex(txindex_), txPrev(txPrev_){
    }
    CBlockHeader blockFrom;
    CDiskTxPos txindex;
    const CTransaction txPrev;
};
bool CheckStakeKernelHash(const CBlockIndex* pindexPrev, unsigned int nBits, CBlockIndex& blockFrom,  const CCoins* txPrev, const COutPoint& prevout, unsigned int nTimeTx);
bool IsConfirmedInNPrevBlocks(const CDiskTxPos& txindex, const CBlockIndex* pindexFrom, int nMaxDepth, int& nActualDepth);
bool VerifySignature(const CTransaction& txFrom, const CTransaction& txTo, unsigned int nIn, unsigned int flags, int nHashType);
bool CheckProofOfStake(CBlockIndex* pindexPrev, const CTransaction& tx, unsigned int nBits, CValidationState &state);
void CacheKernel(std::map<COutPoint, CStakeCache>& cache, const COutPoint& prevout);


#endif