// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"

#include "arith_uint256.h"
#include "chain.h"
#include "primitives/block.h"
#include "uint256.h"
#include "util.h"

unsigned int GetNextWorkRequiredORG(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // Only change once per difficulty adjustment interval
    if ((pindexLast->nHeight+1) % params.DifficultyAdjustmentInterval() != 0)
    {
        if (params.fPowAllowMinDifficultyBlocks)
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 10 minutes
            // then allow mining of a min-difficulty block.
            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
                return nProofOfWorkLimit;
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit)
                    pindex = pindex->pprev;
                return pindex->nBits;
            }
        }
        return pindexLast->nBits;
    }

    // Go back by what we want to be 14 days worth of blocks
    // Yangcoin: This fixes an issue where a 51% attack can change difficulty at will.
    // Go back the full period unless it's the first retarget after genesis. Code courtesy of Art Forz
    int blockstogoback = params.DifficultyAdjustmentInterval()-1;
    if ((pindexLast->nHeight+1) != params.DifficultyAdjustmentInterval())
        blockstogoback = params.DifficultyAdjustmentInterval();

    // Go back by what we want to be 14 days worth of blocks
    const CBlockIndex* pindexFirst = pindexLast;
    for (int i = 0; pindexFirst && i < blockstogoback; i++)
        pindexFirst = pindexFirst->pprev;

    assert(pindexFirst);

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // Genesis block
    if (pindexLast == NULL||pindexLast->nHeight < ( BLOCK_HEIGHT_INIT +1 )) { 
        
        return nProofOfWorkLimit;
    }
    
    // Only change once per difficulty adjustment interval
    // DifficultyAdjustmentInterval = nPowTargetTimespan (1day, 86400) / nPowTargetSpacing (1min, 60 )  = 1440
    // 하루의 시간 만큼 예상해서 구동된다.
    // 난이도 조절이 되지 않는다.
    // if ((pindexLast->nHeight+1) % params.DifficultyAdjustmentInterval() != 0)//특정주기에 도달하지 않으면... 매번 동작한다.
    // {
    //     if (params.fPowAllowMinDifficultyBlocks) //mainnet false, only test,regnet
    //     {
    //         // Special difficulty rule for testnet:
    //         // we use MinDiffculty for mainnet
    //         // If the new block's timestamp is more than 2* 10 minutes
    //         // then allow mining of a min-difficulty block.
    //         if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*4)//4분... 아차피 동작하지 않지만...
    //             return nProofOfWorkLimit;
    //         else
    //         {
    //             // Return the last non-special-min-difficulty-rules-block
    //             const CBlockIndex* pindex = pindexLast;
    //             while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit)
    //                 pindex = pindex->pprev;
    //             return pindex->nBits;
    //         }
    //     }
        
    //     return pindexLast->nBits;
    // }

    //every time set retarget...
    //
    if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*5){//1min *5 no block, will reset difficulty
        return nProofOfWorkLimit;
    }
    //too fast
    if (pblock->GetBlockTime() <  ( pindexLast->GetBlockTime() +  params.nPowTargetSpacing/3)){
        unsigned int ret =pindexLast->nBits / 2;
        
        return ret;
    }
    

    // Go back by what we want to be 14 days worth of blocks
    // Yangcoin: This fixes an issue where a 51% attack can change difficulty at will.
    // Go back the full period unless it's the first retarget after genesis. Code courtesy of Art Forz
    //
    int blockstogoback = params.DifficultyAdjustmentInterval()-1;
    if ((pindexLast->nHeight+1) != params.DifficultyAdjustmentInterval()) // nPowTargetTimespan / nPowTargetSpacing;
        blockstogoback = params.DifficultyAdjustmentInterval(); 

    // Go back by what we want to be 1 days worth of blocks
    const CBlockIndex* pindexFirst = pindexLast;
    
    for (int i = 0; pindexFirst && i < blockstogoback; i++){ 
        if(pindexFirst->nHeight<=1)//root
            break;
        pindexFirst = pindexFirst->pprev;
    }
    
    assert(pindexFirst);

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    if (nActualTimespan < params.nPowTargetTimespan/4)
        nActualTimespan = params.nPowTargetTimespan/4;
    if (nActualTimespan > params.nPowTargetTimespan*4)
        nActualTimespan = params.nPowTargetTimespan*4;

    // Retarget
    arith_uint256 bnNew;
    arith_uint256 bnOld;
    bnNew.SetCompact(pindexLast->nBits);
    bnOld = bnNew;
    // Yangcoin: intermediate uint256 can overflow by 1 bit
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    bool fShift = bnNew.bits() > bnPowLimit.bits() - 1;
    if (fShift)
        bnNew >>= 1;
    bnNew *= nActualTimespan;
    bnNew /= params.nPowTargetTimespan;
    if (fShift)
        bnNew <<= 1;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}


bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}
