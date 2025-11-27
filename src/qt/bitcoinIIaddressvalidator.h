// Copyright (c) 2011-2020 The BitcoinII Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOINII_QT_BITCOINADDRESSVALIDATOR_H
#define BITCOINII_QT_BITCOINADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class BitcoinIIAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit BitcoinIIAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

/** BitcoinII address widget validator, checks for a valid bitcoinII address.
 */
class BitcoinIIAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit BitcoinIIAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

#endif // BITCOINII_QT_BITCOINADDRESSVALIDATOR_H
