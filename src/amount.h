// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_AMOUNT_H
#define BITCOIN_AMOUNT_H

#include <stdint.h>

/** Amount in satoshis (Can be negative) */
typedef int64_t CAmount;

static const CAmount COIN = 100000000;
static const CAmount CENT = 1000000;

/** No amount larger than this (in satoshi) is valid.
 *
 * Note that this constant is *not* the total money supply, which in Bitcoin
 * currently happens to be less than 21,000,000 BTC for various reasons, but
 * rather a sanity check. As this sanity check is used by consensus-critical
 * validation code, the exact value of the MAX_MONEY constant is consensus
 * critical; in unusual circumstances like a(nother) overflow bug that allowed
 * for the creation of coins out of thin air modification could lead to a fork.
 * */
static const CAmount MAX_MONEY =           92000000000 * COIN;
static const int BLOCK_HEIGHT_INIT = 10;
static const int BLOCK_HEIGHT_45 = 70500;
static const int BLOCK_NTIME_40 = 1524547800;//2018-04-24 5:30  korea 2018-05-24 1:30
static const CAmount PREMINE_MONEY_COIN =  18400000000 * COIN ;
static const CAmount BLOCK_REWARD_COIN = 50 * COIN; 
static const CAmount BLOCK_REWARD_COIN_45 = 45 * COIN; 
static const CAmount BLOCK_REWARD_COIN_40 = 40 * COIN;
inline bool MoneyRange(const CAmount& nValue) { return (nValue >= 0 && nValue <= MAX_MONEY); }

#endif //  BITCOIN_AMOUNT_H
