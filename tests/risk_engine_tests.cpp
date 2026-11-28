#include <cassert>
#include <ctime>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

#include "Transaction.h"
#include "RiskContext.h"
#include "RiskEngine.h"
#include "AmountRule.h"
#include "VelocityRule.h"
#include "RecipientRule.h"
#include "RiskReport.h"

int main() {
    std::cout << "--- Running Risk Engine Tests ---\n\n";

    RiskEngine engine(20, 60);
    engine.addRule(std::make_unique<AmountRule>(100, 30));
    engine.addRule(std::make_unique<VelocityRule>(2, 60, 30));
    engine.addRule(std::make_unique<RecipientRule>(std::set<std::string>{"SuspiciousAcc"}, 60));

    std::int64_t now = static_cast<std::int64_t>(std::time(nullptr));
    std::vector<Transaction> history;
    RiskContext ctx{history, now};

    // 1. Normal small transfer -> APPROVED (score 0)
    Transaction txSmall("Alice", "Bob", 25);
    RiskAssessment r1 = engine.evaluate(txSmall, ctx);
    assert(r1.decision == RiskDecision::APPROVED);
    assert(r1.totalScore == 0);
    std::cout << RiskReport(txSmall.getTransactionId(), r1).toString() << "\n";

    // 2. High-value transfer -> FLAGGED (score 30)
    Transaction txLarge("Alice", "Bob", 250);
    RiskAssessment r2 = engine.evaluate(txLarge, ctx);
    assert(r2.decision == RiskDecision::FLAGGED);
    assert(r2.totalScore == 30);
    std::cout << RiskReport(txLarge.getTransactionId(), r2).toString() << "\n";

    // 3. Velocity trigger -> FLAGGED (score 30)
    history.push_back(Transaction("Alice", "Bob", 10));
    history.push_back(Transaction("Alice", "Charlie", 10));
    Transaction txThird("Alice", "Dave", 10);
    RiskAssessment r3 = engine.evaluate(txThird, ctx);
    assert(r3.decision == RiskDecision::FLAGGED);
    assert(r3.totalScore == 30);
    std::cout << RiskReport(txThird.getTransactionId(), r3).toString() << "\n";

    // 4. Watchlisted receiver -> REJECTED (score 60)
    Transaction txWatch("Bob", "SuspiciousAcc", 15);
    RiskAssessment r4 = engine.evaluate(txWatch, ctx);
    assert(r4.decision == RiskDecision::REJECTED);
    assert(r4.totalScore == 60);
    std::cout << RiskReport(txWatch.getTransactionId(), r4).toString() << "\n";

    // 5. Invalid threshold configuration throws
    bool threw = false;
    try {
        engine.setThresholds(60, 20);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);

    std::cout << "All Risk Engine tests passed!\n";
    return 0;
}