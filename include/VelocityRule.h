#ifndef VELOCITYRULE_H
#define VELOCITYRULE_H

#include <cstddef>
#include <cstdint>
#include <string>
#include "RiskRule.h"

class VelocityRule : public RiskRule {
private:
    std::size_t maxTransactions;
    std::int64_t windowSeconds;
    int riskScore;

public:
    VelocityRule(std::size_t maxTransactions, std::int64_t windowSeconds, int riskScore);

    RuleResult evaluate(
        const Transaction& tx,
        const RiskContext& context
    ) const override;

    std::string getName() const override;

    std::size_t getMaxTransactions() const;
    std::int64_t getWindowSeconds() const;
};

#endif