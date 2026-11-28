#include "VelocityRule.h"
#include <stdexcept>
#include <string>

namespace {
bool parseTimestamp(const std::string& raw, std::int64_t& out) {
    try {
        out = std::stoll(raw);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool parseTimestamp(std::int64_t raw, std::int64_t& out) {
    out = raw;
    return true;
}
} // namespace

VelocityRule::VelocityRule(std::size_t maxTransactions, std::int64_t windowSeconds, int riskScore)
    : maxTransactions(maxTransactions), windowSeconds(windowSeconds), riskScore(riskScore) {
    if (maxTransactions == 0) {
        throw std::invalid_argument("VelocityRule maxTransactions must be greater than zero.");
    }
    if (windowSeconds <= 0) {
        throw std::invalid_argument("VelocityRule windowSeconds must be positive.");
    }
    if (riskScore < 0) {
        throw std::invalid_argument("VelocityRule risk score cannot be negative.");
    }
}

RuleResult VelocityRule::evaluate(
    const Transaction& tx,
    const RiskContext& context
) const {
    std::size_t recentCount = 0;

    for (const Transaction& pastTx : context.recentTransactions) {
        if (pastTx.getSender() != tx.getSender()) {
            continue;
        }

        std::int64_t pastTime = 0;
        if (!parseTimestamp(pastTx.getTimestamp(), pastTime)) {
            continue;
        }

        if (pastTime <= context.evaluationTime &&
            (context.evaluationTime - pastTime) <= windowSeconds) {
            ++recentCount;
        }
    }

    // Count includes the current transaction being evaluated
    std::size_t totalInWindow = recentCount + 1;

    if (totalInWindow > maxTransactions) {
        return {
            true,
            riskScore,
            getName(),
            "Sender '" + tx.getSender() + "' has " + std::to_string(totalInWindow) +
            " transfers within " + std::to_string(windowSeconds) +
            "s (limit: " + std::to_string(maxTransactions) + ")."
        };
    }

    return {
        false,
        0,
        getName(),
        "Sender transfer frequency is within normal limits."
    };
}

std::string VelocityRule::getName() const {
    return "VelocityRule";
}

std::size_t VelocityRule::getMaxTransactions() const {
    return maxTransactions;
}

std::int64_t VelocityRule::getWindowSeconds() const {
    return windowSeconds;
}