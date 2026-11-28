#include "AmountRule.h"
#include <stdexcept>
#include <string>

AmountRule::AmountRule(std::int64_t threshold, int riskScore)
    : threshold(threshold), riskScore(riskScore) {
    if (threshold <= 0) {
        throw std::invalid_argument("AmountRule threshold must be positive.");
    }
    if (riskScore < 0) {
        throw std::invalid_argument("AmountRule risk score cannot be negative.");
    }
}

RuleResult AmountRule::evaluate(
    const Transaction& tx,
    const RiskContext& /*context*/
) const {
    if (tx.getAmount() > threshold) {
        return {
            true,
            riskScore,
            getName(),
            "Transfer amount (" + std::to_string(tx.getAmount()) +
            ") exceeds high-value threshold (" + std::to_string(threshold) + ")."
        };
    }

    return {
        false,
        0,
        getName(),
        "Transfer amount is within configured threshold."
    };
}

std::string AmountRule::getName() const {
    return "AmountRule";
}

std::int64_t AmountRule::getThreshold() const {
    return threshold;
}

void AmountRule::setThreshold(std::int64_t newThreshold) {
    if (newThreshold <= 0) {
        throw std::invalid_argument("AmountRule threshold must be positive.");
    }
    threshold = newThreshold;
}