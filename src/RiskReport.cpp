    #include "RiskReport.h"
#include <sstream>

RiskReport::RiskReport(const std::string& transactionId, const RiskAssessment& assessment)
    : transactionId(transactionId), assessment(assessment) {}

const std::string& RiskReport::getTransactionId() const {
    return transactionId;
}

const RiskAssessment& RiskReport::getAssessment() const {
    return assessment;
}

std::string RiskReport::decisionToString(RiskDecision decision) {
    switch (decision) {
        case RiskDecision::APPROVED: return "APPROVED";
        case RiskDecision::FLAGGED:  return "FLAGGED";
        case RiskDecision::REJECTED: return "REJECTED";
    }
    return "UNKNOWN";
}

std::string RiskReport::toString() const {
    std::stringstream ss;
    ss << "Risk Report for Transaction: " << transactionId << "\n"
       << "Decision   : " << decisionToString(assessment.decision) << "\n"
       << "Total Score: " << assessment.totalScore << "\n"
       << "Rule Breakdown:\n";

    if (assessment.ruleResults.empty()) {
        ss << "  [No rules configured]\n";
    } else {
        for (const RuleResult& res : assessment.ruleResults) {
            ss << "  - [" << (res.triggered ? "TRIGGERED" : "OK") << "] "
               << res.ruleName << " (+" << res.score << "): " << res.reason << "\n";
        }
    }

    return ss.str();
}