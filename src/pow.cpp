// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <chainparams.h>
#include <primitives/block.h>
#include <uint256.h>

namespace {
    // returns a * exp(p/q) where |p/q| is small
    arith_uint256 mul_exp(arith_uint256 a, int64_t p, int64_t q)
    {
        bool isNegative = p < 0;
        uint64_t abs_p = p >= 0 ? p : -p;
        arith_uint256 result = a;
        uint64_t n = 0;
        while (a > 0) {
            ++n;
            a = a * abs_p / q / n;
            if (isNegative && (n % 2 == 1)) {
                result -= a;
            } else {
                result += a;
            }
        }
        return result;
    }
}

// ppcoin: find last block index up to pindex
const CBlockIndex* GetLastBlockIndex(const CBlockIndex* pindex, bool fProofOfStake)
{
    //CBlockIndex will be updated with information about the proof type later
    while (pindex && pindex->pprev && (pindex->IsProofOfStake() != fProofOfStake))
        pindex = pindex->pprev;
    return pindex;
}

inline arith_uint256 GetLimit(int nHeight, const Consensus::Params& params, bool fProofOfStake)
{
    if(fProofOfStake) {
        if(nHeight < params.QIP9Height) {
            return UintToArith256(params.posLimit);
        } else {
            return UintToArith256(params.QIP9PosLimit);
        }
    } else {
        return UintToArith256(params.powLimit);
    }
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params, bool fProofOfStake)
{
    const unsigned int nHeight = pindexLast ? pindexLast->nHeight + 1 : 0;
    unsigned int  nTargetLimit = GetLimit(nHeight, params, fProofOfStake).GetCompact();

    // genesis block
    if (pindexLast == nullptr)
        return nTargetLimit;

    // first block
    const CBlockIndex* pindexPrev = GetLastBlockIndex(pindexLast, fProofOfStake);
    if (pindexPrev->pprev == nullptr)
        return nTargetLimit;

    // second block
    const CBlockIndex* pindexPrevPrev = GetLastBlockIndex(pindexPrev->pprev, fProofOfStake);
    if (pindexPrevPrev->pprev == nullptr)
        return nTargetLimit;

    // Return min difficulty on regtest
    if (params.fPowAllowMinDifficultyBlocks)
        return nTargetLimit;

    if (nHeight >= params.nDiffChange)
    {
        if (fProofOfStake)
        {
            return CalculateNextWorkRequired_QTUM(pindexPrev, pindexPrevPrev->GetBlockTime(), params);
        }
        else
        {
            return CalculateNextWorkRequired_Dash(pindexLast, params);
        }
    }

    return CalculateNextWorkRequired(pindexPrev, params, fProofOfStake);
}

/**
 * eHRC (enhanced Hash Rate Compensation)
 * Short, medium and long samples averaged together and compared against the target time span.
 * Adjust every block but limted to 9% change maximum.
 * Difficulty is calculated separately for PoW and PoS blocks in that PoW skips PoS blocks and vice versa.
 */
unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, const Consensus::Params& params, bool fProofOfStake)
{
    if(fProofOfStake){
        if (params.fPoSNoRetargeting)
            return pindexLast->nBits;
    }else{
        if (params.fPowNoRetargeting)
            return pindexLast->nBits;
    }

    int nHeight = pindexLast->nHeight + 1;
    const arith_uint256 nTargetLimit = GetLimit(pindexLast ? pindexLast->nHeight+1 : 0, params, fProofOfStake);
    int nPowTargetTimespan = params.nPowTargetTimespan;

    int pindexFirstShortTime = 0, pindexFirstMediumTime = 0, pindexFirstLongTime = 0;
    int shortSample = 15, mediumSample = 200, longSample = 1000;
    int nActualTimespan = 0, nActualTimespanShort = 0, nActualTimespanMedium = 0, nActualTimespanLong = 0;

    // Set testnet time to be the same as mainnet
    if (Params().NetworkIDString() == CBaseChainParams::TESTNET && nHeight >= params.nFixUTXOCacheHFHeight)
        nPowTargetTimespan = 60;

    const CBlockIndex* pindexFirstLong = pindexLast;

    // i tracks sample height, j counts number of blocks of required type
    for (int i = 0, j = 0; j <= longSample + 1;) {
        bool skip = false;

        // Hit the start of the chain before finding enough blocks
        if (pindexFirstLong->pprev == nullptr)
            return nTargetLimit.GetCompact();

        // Only increment j if we have a block of the current type
        if (fProofOfStake) {
            if (pindexFirstLong->IsProofOfStake())
                j++;
            if (pindexFirstLong->pprev->IsProofOfWork())
                skip = true;
        } else {
            if (pindexFirstLong->IsProofOfWork())
                j++;
            if (pindexFirstLong->pprev->IsProofOfStake())
                skip = true;
        }

        pindexFirstLong = pindexFirstLong->pprev;

        // Do not sample on longSample - 1 due to nDiffAdjustChange bug
        if (i < longSample)
            pindexFirstLongTime = pindexFirstLong->GetBlockTime();

        if (skip) {
            // Incorrectly increment i before nDiffAdjustChange
            if (nHeight <= params.nDiffAdjustChange)
                i++;
            continue;
        }

        if (i == shortSample - 1)
            pindexFirstShortTime = pindexFirstLong->GetBlockTime();

        if (i == mediumSample - 1)
            pindexFirstMediumTime = pindexFirstLong->GetBlockTime();

        i++;
    }

    if (pindexLast->GetBlockTime() - pindexFirstShortTime != 0)
        nActualTimespanShort = (pindexLast->GetBlockTime() - pindexFirstShortTime) / shortSample;

    if (pindexLast->GetBlockTime() - pindexFirstMediumTime != 0)
        nActualTimespanMedium = (pindexLast->GetBlockTime() - pindexFirstMediumTime) / mediumSample;

    if (pindexLast->GetBlockTime() - pindexFirstLongTime != 0)
        nActualTimespanLong = (pindexLast->GetBlockTime() - pindexFirstLongTime) / longSample;

    int nActualTimespanSum = nActualTimespanShort + nActualTimespanMedium + nActualTimespanLong;

    if (nActualTimespanSum != 0)
        nActualTimespan = nActualTimespanSum / 3;

    if (pindexLast->nHeight >= params.nDiffDamping) {
        // Apply .25 damping
        nActualTimespan = nActualTimespan + (3 * nPowTargetTimespan);
        nActualTimespan /= 4;
    }

    // 9% difficulty limiter
    int nActualTimespanMax = nPowTargetTimespan * 494 / 453;
    int nActualTimespanMin = nPowTargetTimespan * 453 / 494;

    if(nActualTimespan < nActualTimespanMin)
        nActualTimespan = nActualTimespanMin;

    if(nActualTimespan > nActualTimespanMax)
        nActualTimespan = nActualTimespanMax;

    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= nPowTargetTimespan;

    if (bnNew <= 0 || bnNew > nTargetLimit)
        bnNew = nTargetLimit;

    return bnNew.GetCompact();
}

// Use QTUM's difficulty adjust for PoS blocks only
unsigned int CalculateNextWorkRequired_QTUM(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    if (params.fPoSNoRetargeting) {
        return pindexLast->nBits;
    }
    // Limit adjustment step
    int64_t nTargetSpacing = params.nPowTargetSpacing;
    int64_t nActualSpacing = pindexLast->GetBlockTime() - nFirstBlockTime;
    // Retarget
    const arith_uint256 bnTargetLimit = GetLimit(pindexLast ? pindexLast->nHeight+1 : 0, params, true);
    // ppcoin: target change every block
    // ppcoin: retarget with exponential moving toward target spacing
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    int64_t nInterval = params.DifficultyAdjustmentInterval(pindexLast->nHeight + 1);

    if (pindexLast->nHeight + 1 < params.QIP9Height) {
        if (nActualSpacing < 0)
            nActualSpacing = nTargetSpacing;
        if (nActualSpacing > nTargetSpacing * 10)
            nActualSpacing = nTargetSpacing * 10;
        bnNew *= ((nInterval - 1) * nTargetSpacing + nActualSpacing + nActualSpacing);
        bnNew /= ((nInterval + 1) * nTargetSpacing);
    } else {
        if (nActualSpacing < 0)
            nActualSpacing = nTargetSpacing;
        if (nActualSpacing > nTargetSpacing * 20)
            nActualSpacing = nTargetSpacing * 20;
        bnNew = mul_exp(bnNew, 2 * (nActualSpacing - nTargetSpacing) / 16, (nInterval + 1) * nTargetSpacing / 16);
    }

    if (bnNew <= 0 || bnNew > bnTargetLimit)
        bnNew = bnTargetLimit;
    return bnNew.GetCompact();
}

// Use Dash's difficulty adjust for PoW blocks only
unsigned int CalculateNextWorkRequired_Dash(const CBlockIndex* pindexLast, const Consensus::Params& params)
{
    /* current difficulty formula, dash - DarkGravity v3, written by Evan Duffield - evan@dash.org */

    const arith_uint256 nTargetLimit = GetLimit(0, params, false);
    int64_t nPastBlocks = 30;
    const CBlockIndex *pindex = pindexLast;
    arith_uint256 bnPastTargetAvg;

    for (unsigned int nCountBlocks = 1; nCountBlocks <= nPastBlocks; nCountBlocks++) {
        arith_uint256 bnTarget = arith_uint256().SetCompact(pindex->nBits);
        if (nCountBlocks == 1) {
            bnPastTargetAvg = bnTarget;
        } else {
            // NOTE: that's not an average really...
            bnPastTargetAvg = (bnPastTargetAvg * nCountBlocks + bnTarget) / (nCountBlocks + 1);
        }

        if(nCountBlocks != nPastBlocks) {
            // If we hit start of chain return min diff
            if (pindex->pprev == nullptr)
                return nTargetLimit.GetCompact();

            pindex = GetLastBlockIndex(pindex->pprev, false);
        }
    }

    arith_uint256 bnNew(bnPastTargetAvg);

    int64_t nActualTimespan = pindexLast->GetBlockTime() - pindex->GetBlockTime();
    int64_t nTargetTimespan = nPastBlocks * params.nPowTargetSpacing;

    if (nActualTimespan < nTargetTimespan/3)
        nActualTimespan = nTargetTimespan/3;
    if (nActualTimespan > nTargetTimespan*3)
        nActualTimespan = nTargetTimespan*3;

    // Retarget
    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

    if (bnNew > nTargetLimit) {
        bnNew = nTargetLimit;
    }

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params, bool fProofOfStake)
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
