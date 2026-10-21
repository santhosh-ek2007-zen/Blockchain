
#include "Blockchain.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

// Tests the initial state of a newly created blockchain.
void testBlockchainInitialization() {
    Blockchain blockchain;

    // A new blockchain should contain only the genesis block.
    assert(blockchain.getChain().size() == 1);

    // No transactions should be pending initially.
    assert(blockchain.getPendingTransactions().empty());

    // The genesis block must have index zero.
    assert(blockchain.getLatestBlock().getIndex() == 0);

    // The genesis block must have the expected previous hash.
    assert(blockchain.getLatestBlock().getPreviousHash() == "0");

    // A newly created blockchain should pass structural validation.
    assert(blockchain.isChainValid());

    std::cout << "Initialization test passed.\n";
}

// Tests the default balance of an account.
void testInitialBalances() {
    Blockchain blockchain;

    // Accounts that do not exist should have a balance of zero.
    assert(blockchain.getBalance("Alice") == 0);
    assert(blockchain.getBalance("Bob") == 0);

    std::cout << "Initial balance test passed.\n";
}

// Tests rejection of a transaction with an empty sender.
void testEmptySender() {
    Blockchain blockchain;

    Transaction transaction("", "Bob", 100);

    // An empty sender address must be rejected.
    assert(!blockchain.isTransactionValid(transaction));

    // An invalid transaction must not be added to the pending list.
    assert(!blockchain.addTransaction(transaction));

    assert(blockchain.getPendingTransactions().empty());

    std::cout << "Empty sender test passed.\n";
}

// Tests rejection of a transaction with an empty receiver.
void testEmptyReceiver() {
    Blockchain blockchain;

    Transaction transaction("Alice", "", 100);

    // An empty receiver address must be rejected.
    assert(!blockchain.isTransactionValid(transaction));
    assert(!blockchain.addTransaction(transaction));

    std::cout << "Empty receiver test passed.\n";
}

// Tests rejection of a transfer to the same account.
void testSelfTransaction() {
    Blockchain blockchain;

    Transaction transaction("Alice", "Alice", 100);

    // An account must not transfer money to itself.
    assert(!blockchain.isTransactionValid(transaction));
    assert(!blockchain.addTransaction(transaction));

    std::cout << "Self-transaction test passed.\n";
}

// Tests rejection of zero and negative transaction amounts.
void testInvalidAmounts() {
    Blockchain blockchain;

    Transaction zeroTransaction("Alice", "Bob", 0);
    Transaction negativeTransaction("Alice", "Bob", -10);

    // The transaction amount must be greater than zero.
    assert(!blockchain.isTransactionValid(zeroTransaction));
    assert(!blockchain.addTransaction(zeroTransaction));

    // Negative amounts must also be rejected.
    assert(!blockchain.isTransactionValid(negativeTransaction));
    assert(!blockchain.addTransaction(negativeTransaction));

    // Neither invalid transaction should become pending.
    assert(blockchain.getPendingTransactions().empty());

    std::cout << "Invalid amount test passed.\n";
}

// Tests rejection of a transaction when the sender has no funds.
void testInsufficientBalance() {
    Blockchain blockchain;

    Transaction transaction("Alice", "Bob", 50);

    // Alice has no funds in the initial ledger.
    assert(blockchain.getBalance("Alice") == 0);

    // A transfer requiring funds must be rejected.
    assert(!blockchain.isTransactionValid(transaction));
    assert(!blockchain.addTransaction(transaction));

    // No transaction should have been added.
    assert(blockchain.getPendingTransactions().empty());

    std::cout << "Insufficient balance test passed.\n";
}

// Tests mining when there are no pending transactions.
void testMiningWithoutPendingTransactions() {
    Blockchain blockchain;

    const std::size_t originalChainSize = blockchain.getChain().size();

    // Mining an empty pending list must fail.
    assert(!blockchain.minePendingTransactions());

    // The chain must remain unchanged.
    assert(blockchain.getChain().size() == originalChainSize);

    // The blockchain should remain valid.
    assert(blockchain.isChainValid());

    std::cout << "Empty mining test passed.\n";
}

// Tests that invalid transactions do not change the chain.
void testInvalidTransactionDoesNotChangeChain() {
    Blockchain blockchain;

    const std::size_t originalChainSize = blockchain.getChain().size();

    Transaction transaction("Alice", "Bob", 100);

    // This transaction must fail because Alice has no funds.
    assert(!blockchain.addTransaction(transaction));

    // The chain and pending transaction list must remain unchanged.
    assert(blockchain.getChain().size() == originalChainSize);
    assert(blockchain.getPendingTransactions().empty());

    // Account balances must remain unchanged.
    assert(blockchain.getBalance("Alice") == 0);
    assert(blockchain.getBalance("Bob") == 0);

    std::cout << "Invalid transaction state test passed.\n";
}

// Tests that the blockchain remains valid after rejected operations.
void testChainValidityAfterRejections() {
    Blockchain blockchain;

    Transaction transaction("Alice", "Bob", 100);

    // Attempt to add an invalid transaction.
    assert(!blockchain.addTransaction(transaction));

    // Failed operations must not corrupt the blockchain.
    assert(blockchain.isChainValid());

    std::cout << "Chain validity test passed.\n";
}

// Runs all blockchain tests.
int main() {
    testBlockchainInitialization();
    testInitialBalances();
    testEmptySender();
    testEmptyReceiver();
    testSelfTransaction();
    testInvalidAmounts();
    testInsufficientBalance();
    testMiningWithoutPendingTransactions();
    testInvalidTransactionDoesNotChangeChain();
    testChainValidityAfterRejections();

    std::cout << "\nAll blockchain tests passed.\n";

    return 0;
}
