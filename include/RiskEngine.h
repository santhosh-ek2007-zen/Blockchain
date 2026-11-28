#ifndef RISKENGINE_H
#define RISKENGINE_H

#include <cstddef>
#include <memory>
#include <vector>
#include "RiskRule.h"

class RiskEngine {
private:
    std::vector<std::unique_ptr<RiskRule>> rules;
    int flagThreshold;
    int rejectThreshold;

public:
    RiskEngine(int flagThreshold = 20, int rejectThreshold = 60);

    void addRule(std::unique_ptr<RiskRule> rule);

    RiskAssessment evaluate(
        const Transaction& tx,
        const RiskContext& context
    ) const;

    void setThresholds(int flagScore, int rejectScore);
    int getFlagThreshold() const;
    int getRejectThreshold() const;
    std::size_t getRuleCount() const;
};

#endif