#ifndef RISKREPORT_H
#define RISKREPORT_H

#include <string>
#include "RiskRule.h"

class RiskReport {
private:
    std::string transactionId;
    RiskAssessment assessment;

public:
    RiskReport(const std::string& transactionId, const RiskAssessment& assessment);

    const std::string& getTransactionId() const;
    const RiskAssessment& getAssessment() const;
    std::string toString() const;

    static std::string decisionToString(RiskDecision decision);
};

#endif