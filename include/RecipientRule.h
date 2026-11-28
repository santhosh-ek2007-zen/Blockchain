#ifndef RECIPIENTRULE_H
#define RECIPIENTRULE_H

#include <set>
#include <string>
#include "RiskRule.h"

class RecipientRule : public RiskRule {
private:
    std::set<std::string> watchlist;
    int riskScore;

public:
    RecipientRule(const std::set<std::string>& watchlist, int riskScore);

    RuleResult evaluate(
        const Transaction& tx,
        const RiskContext& context
    ) const override;

    std::string getName() const override;

    void addToWatchlist(const std::string& address);
    void removeFromWatchlist(const std::string& address);
    const std::set<std::string>& getWatchlist() const;
};

#endif