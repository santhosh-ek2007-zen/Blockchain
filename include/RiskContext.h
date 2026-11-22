#ifndef RISKCONTEXT_H
#define RISKCONTEXT_H

#include <vector>
#include <cstdint>
#include "Transaction.h"

struct RiskContext
{
    const std::vector<Transaction> &recentTransactions;
    std::int64_t evaluationTime;
};

#endif