// Copyright (c) 2023 BitcoinII Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "nontrivial-threadlocal.h"

#include <clang-tidy/ClangTidyModule.h>
#include <clang-tidy/ClangTidyModuleRegistry.h>

class BitcoinIIModule final : public clang::tidy::ClangTidyModule
{
public:
    void addCheckFactories(clang::tidy::ClangTidyCheckFactories& CheckFactories) override
    {
        CheckFactories.registerCheck<bitcoinII::NonTrivialThreadLocal>("bitcoinII-nontrivial-threadlocal");
    }
};

static clang::tidy::ClangTidyModuleRegistry::Add<BitcoinIIModule>
    X("bitcoinII-module", "Adds bitcoinII checks.");

volatile int BitcoinIIModuleAnchorSource = 0;
