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

#include "pos.h"

using namespace std;
 
/**
 * need PoO block
 */
bool checkNeedPoO();

 
// Check whether the coinstake timestamp meets protocol
bool CheckCoinOnlineTimestamp(int64_t nTimeBlock, int64_t nTimeTx);
bool CheckCoinOnlineTimestamp(int64_t nTimeBlock);
bool CheckPoOKernel(CBlockIndex* pindexPrev, unsigned int nBits, uint32_t nTime, const COutPoint& prevout, uint32_t* pBlockTime = NULL);

bool CheckOnlineKernelHash(const CBlockIndex* pindexPrev, unsigned int nBits, CBlockIndex& blockFrom,  const CCoins* txPrev, const COutPoint& prevout, unsigned int nTimeTx);
bool IsConfirmedInNPrevBlocks(const CDiskTxPos& txindex, const CBlockIndex* pindexFrom, int nMaxDepth, int& nActualDepth);
bool VerifySignature(const CTransaction& txFrom, const CTransaction& txTo, unsigned int nIn, unsigned int flags, int nHashType);
bool CheckProofOfOnline(CBlockIndex* pindexPrev, const CTransaction& tx, unsigned int nBits, CValidationState &state);
void CachePoOKernel(std::map<COutPoint, CStakeCache>& cache, const COutPoint& prevout);
bool CheckPoOKernel(const CBlockIndex *pindexPrev, unsigned int nBits, uint32_t nTime,  uint32_t *pBlockTime);


/** check block sign by org **/
bool CheckBlockSignature(const CBlock& block);
#endif