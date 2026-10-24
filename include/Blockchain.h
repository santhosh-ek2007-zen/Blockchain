#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H
#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "Block.h"
#include "Transaction.h"

class Blockchain {
private:
    // Stores blocks in the order they are added.
    std::vector<Block> chain;

    // Stores transactions waiting to be mined.
    std::vector<Transaction> pendingTransactions;

    // Stores the confirmed balance of each address.
    std::map<std::string, std::int64_t> balances;

    // Maximum number of transactions allowed in one block.
    std::size_t blockSize;

public:
    // Creates a blockchain and its genesis block.
    Blockchain();

    // Adds a transaction if it passes validation.
    bool addTransaction(const Transaction& transaction);

    // Checks whether a transaction is valid against the current ledger.
    bool isTransactionValid(const Transaction& transaction) const;

    // Mines pending transactions and updates confirmed balances.
    bool minePendingTransactions();

    // Checks block hashes, links, indexes, and transaction integrity.
    bool isChainValid() const;

    // Returns the confirmed balance of an address.
    std::int64_t getBalance(const std::string& address) const;

    // Returns the latest block.
    const Block& getLatestBlock() const;

    // Returns the complete chain without allowing modification.
    const std::vector<Block>& getChain() const;

    // Returns pending transactions without allowing modification.
    const std::vector<Transaction>& getPendingTransactions() const;

    // Set the initial balances
    void initializeAccount(const std::string& account, long long amount);
};

#endif
