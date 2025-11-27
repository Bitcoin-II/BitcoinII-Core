// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The BitcoinII Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <consensus/amount.h>
#include <policy/feerate.h>
#include <tinyformat.h>

#include <cmath>

CFeeRate::CFeeRate(const CAmount& nFeePaid, uint32_t num_bytes)
{
    const int64_t nSize{num_bytes};

    if (nSize > 0) {
        nSatooshisPerK = nFeePaid * 1000 / nSize;
    } else {
        nSatooshisPerK = 0;
    }
}

CAmount CFeeRate::GetFee(uint32_t num_bytes) const
{
    const int64_t nSize{num_bytes};

    // Be explicit that we're converting from a double to int64_t (CAmount) here.
    // We've previously had issues with the silent double->int64_t conversion.
    CAmount nFee{static_cast<CAmount>(std::ceil(nSatooshisPerK * nSize / 1000.0))};

    if (nFee == 0 && nSize != 0) {
        if (nSatooshisPerK > 0) nFee = CAmount(1);
        if (nSatooshisPerK < 0) nFee = CAmount(-1);
    }

    return nFee;
}

std::string CFeeRate::ToString(const FeeEstimateMode& fee_estimate_mode) const
{
    switch (fee_estimate_mode) {
    case FeeEstimateMode::SAT2_VB: return strprintf("%d.%03d %s/vB", nSatooshisPerK / 1000, nSatooshisPerK % 1000, CURRENCY_ATOM);
    default:                      return strprintf("%d.%08d %s/kvB", nSatooshisPerK / COIN, nSatooshisPerK % COIN, CURRENCY_UNIT);
    }
}
