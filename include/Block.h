#ifndef BLOCK_H
#define BLOCK_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "Transaction.h"

class Block {
private:
    std::size_t index;                       // Position of the block in the chain
    std::int64_t timestamp;                  // Block creation time (Integer time value)
    std::vector<Transaction> transactions;   // Ordered transfers included in the block
    std::string previousHash;                // Hash reference to the preceding block
    std::string hash;                        // Stored hash for this block

public:
    // Constructor: Initializes index, timestamp, transactions, and previous hash
    Block(std::size_t index, const std::vector<Transaction>& transactions, const std::string& previousHash);

    // Adds transactions while assembling a block
    void addTransaction(const Transaction& transaction);

    // Hashes the canonical representation of the block's contents (excludes its own stored hash)
    std::string calculateHash() const;

    // Read-Only Accessors for chain validation
    std::string getHash() const;
    std::string getPreviousHash() const;
    std::vector<Transaction> getTransactions() const;
    std::size_t getIndex() const;
    std::int64_t getTimestamp() const;

    // Formats block and transaction details for console display
    std::string toString() const;
};

#endif 
