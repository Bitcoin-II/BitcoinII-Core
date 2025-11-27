// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The BitcoinII Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOINII_POLICY_FEERATE_H
#define BITCOINII_POLICY_FEERATE_H

#include <consensus/amount.h>
#include <serialize.h>


#include <cstdint>
#include <string>
#include <type_traits>

const std::string CURRENCY_UNIT = "BC2"; // One formatted unit
const std::string CURRENCY_ATOM = "sat2"; // One indivisible minimum value unit

/* Used to determine type of fee estimation requested */
enum class FeeEstimateMode {
    UNSET,        //!< Use default settings based on other criteria
    ECONOMICAL,   //!< Force estimateSmartFee to use non-conservative estimates
    CONSERVATIVE, //!< Force estimateSmartFee to use conservative estimates
    BC2_KVB,      //!< Use BC2/kvB fee rate unit
    SAT2_VB,       //!< Use sat2/vB fee rate unit
};

/**
 * Fee rate in satooshis per kilovirtualbyte: CAmount / kvB
 */
class CFeeRate
{
private:
    /** Fee rate in sat2/kvB (satooshis per 1000 virtualbytes) */
    CAmount nSatooshisPerK;

public:
    /** Fee rate of 0 satooshis per kvB */
    CFeeRate() : nSatooshisPerK(0) { }
    template<std::integral I> // Disallow silent float -> int conversion
    explicit CFeeRate(const I _nSatooshisPerK): nSatooshisPerK(_nSatooshisPerK) {
    }

    /**
     * Construct a fee rate from a fee in satooshis and a vsize in vB.
     *
     * param@[in]   nFeePaid    The fee paid by a transaction, in satooshis
     * param@[in]   num_bytes   The vsize of a transaction, in vbytes
     */
    CFeeRate(const CAmount& nFeePaid, uint32_t num_bytes);

    /**
     * Return the fee in satooshis for the given vsize in vbytes.
     * If the calculated fee would have fractional satooshis, then the
     * returned fee will always be rounded up to the nearest satooshi.
     */
    CAmount GetFee(uint32_t num_bytes) const;

    /**
     * Return the fee in satooshis for a vsize of 1000 vbytes
     */
    CAmount GetFeePerK() const { return nSatooshisPerK; }
    friend bool operator<(const CFeeRate& a, const CFeeRate& b) { return a.nSatooshisPerK < b.nSatooshisPerK; }
    friend bool operator>(const CFeeRate& a, const CFeeRate& b) { return a.nSatooshisPerK > b.nSatooshisPerK; }
    friend bool operator==(const CFeeRate& a, const CFeeRate& b) { return a.nSatooshisPerK == b.nSatooshisPerK; }
    friend bool operator<=(const CFeeRate& a, const CFeeRate& b) { return a.nSatooshisPerK <= b.nSatooshisPerK; }
    friend bool operator>=(const CFeeRate& a, const CFeeRate& b) { return a.nSatooshisPerK >= b.nSatooshisPerK; }
    friend bool operator!=(const CFeeRate& a, const CFeeRate& b) { return a.nSatooshisPerK != b.nSatooshisPerK; }
    CFeeRate& operator+=(const CFeeRate& a) { nSatooshisPerK += a.nSatooshisPerK; return *this; }
    std::string ToString(const FeeEstimateMode& fee_estimate_mode = FeeEstimateMode::BC2_KVB) const;
    friend CFeeRate operator*(const CFeeRate& f, int a) { return CFeeRate(a * f.nSatooshisPerK); }
    friend CFeeRate operator*(int a, const CFeeRate& f) { return CFeeRate(a * f.nSatooshisPerK); }

    SERIALIZE_METHODS(CFeeRate, obj) { READWRITE(obj.nSatooshisPerK); }
};

#endif // BITCOINII_POLICY_FEERATE_H
