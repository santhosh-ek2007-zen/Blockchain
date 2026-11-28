#include "RecipientRule.h"
#include <stdexcept>

RecipientRule::RecipientRule(const std::set<std::string>& watchlist, int riskScore)
    : watchlist(watchlist), riskScore(riskScore) {
    if (riskScore < 0) {
        throw std::invalid_argument("RecipientRule risk score cannot be negative.");
    }
}

RuleResult RecipientRule::evaluate(
    const Transaction& tx,
    const RiskContext& /*context*/
) const {
    if (watchlist.find(tx.getReceiver()) != watchlist.end()) {
        return {
            true,
            riskScore,
            getName(),
            "Receiver '" + tx.getReceiver() + "' is on the risk watchlist."
        };
    }

    return {
        false,
        0,
        getName(),
        "Receiver is not on the watchlist."
    };
}

std::string RecipientRule::getName() const {
    return "RecipientRule";
}

void RecipientRule::addToWatchlist(const std::string& address) {
    if (address.empty()) {
        throw std::invalid_argument("Watchlist address cannot be empty.");
    }
    watchlist.insert(address);
}

void RecipientRule::removeFromWatchlist(const std::string& address) {
    watchlist.erase(address);
}

const std::set<std::string>& RecipientRule::getWatchlist() const {
    return watchlist;
}