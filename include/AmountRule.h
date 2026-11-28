#ifndef AMOUNTRULE_H
#define AMOUNTRULE_H

#include <cstdint>
#include <string>
#include "RiskRule.h"

class AmountRule : public RiskRule {
private:
    std::int64_t threshold;
    int riskScore;

public:
    AmountRule(std::int64_t threshold, int riskScore);

    RuleResult evaluate(
        const Transaction& tx,
        const RiskContext& context
    ) const override;

    std::string getName() const override;

    std::int64_t getThreshold() const;
    void setThreshold(std::int64_t newThreshold);
};

#endif