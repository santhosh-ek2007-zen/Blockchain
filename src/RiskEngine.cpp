#include "RiskEngine.h"
#include <stdexcept>
#include <utility>

RiskEngine::RiskEngine(int flagThreshold, int rejectThreshold)
    : flagThreshold(0), rejectThreshold(0) {
    setThresholds(flagThreshold, rejectThreshold);
}

void RiskEngine::addRule(std::unique_ptr<RiskRule> rule) {
    if (!rule) {
        throw std::invalid_argument("Cannot add a null RiskRule to RiskEngine.");
    }
    rules.push_back(std::move(rule));
}

RiskAssessment RiskEngine::evaluate(
    const Transaction& tx,
    const RiskContext& context
) const {
    RiskAssessment assessment;
    assessment.totalScore = 0;

    for (const std::unique_ptr<RiskRule>& rule : rules) {
        RuleResult result = rule->evaluate(tx, context);
        if (result.triggered) {
            assessment.totalScore += result.score;
        }
        assessment.ruleResults.push_back(std::move(result));
    }

    if (assessment.totalScore >= rejectThreshold) {
        assessment.decision = RiskDecision::REJECTED;
    } else if (assessment.totalScore >= flagThreshold) {
        assessment.decision = RiskDecision::FLAGGED;
    } else {
        assessment.decision = RiskDecision::APPROVED;
    }

    return assessment;
}

void RiskEngine::setThresholds(int flagScore, int rejectScore) {
    if (flagScore <= 0 || rejectScore <= flagScore) {
        throw std::invalid_argument(
            "Invalid thresholds: require 0 < flagThreshold < rejectThreshold."
        );
    }
    flagThreshold = flagScore;
    rejectThreshold = rejectScore;
}

int RiskEngine::getFlagThreshold() const {
    return flagThreshold;
}

int RiskEngine::getRejectThreshold() const {
    return rejectThreshold;
}

std::size_t RiskEngine::getRuleCount() const {
    return rules.size();
}