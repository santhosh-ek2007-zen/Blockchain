#ifndef RISKRULE_H
#define RISKRULE_H

#include <string>
#include <vector>
#include "Transaction.h"
#include "RiskContext.h"

enum class RiskDecision {
    APPROVED,
    FLAGGED,
    REJECTED
};

struct RuleResult {
    bool triggered;
    int score;
    std::string ruleName;
    std::string reason;
};

struct RiskAssessment {
    RiskDecision decision;
    int totalScore;
    std::vector<RuleResult> ruleResults;
};

class RiskRule {
public:
    virtual RuleResult evaluate(
        const Transaction& tx,
        const RiskContext& context
    ) const = 0;

    virtual std::string getName() const = 0;
    virtual ~RiskRule() = default;
};

#endif