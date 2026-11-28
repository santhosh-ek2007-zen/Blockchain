#include "Block.h"
#include "HashUtil.h"

#include <ctime>
#include <sstream>
#include <stdexcept>

// Constructor: Initializes index, timestamp, transactions, previousHash, and calculates the initial hash
Block::Block(std::size_t index, const std::vector<Transaction>& transactions, const std::string& previousHash)
    : index(index),
      timestamp(static_cast<std::int64_t>(std::time(nullptr))),
      transactions(transactions),
      previousHash(previousHash)
{
    hash = calculateHash();
}

// Adds a valid transaction while assembling a block and updates the block's hash
void Block::addTransaction(const Transaction& transaction) {
    if (transaction.isValid()) {
        transactions.push_back(transaction);
        hash = calculateHash();
    }
}

// Hashes the canonical representation of the block's contents (excludes its own stored hash)
std::string Block::calculateHash() const {
    std::stringstream ss;

    // Unambiguous serialization with fixed field order:
    // index | timestamp | previousHash | transaction_count | ordered transactions
    ss << index << "|" << timestamp << "|" << previousHash << "|" << transactions.size() << "|";

    for (const Transaction& tx : transactions) {
        ss << tx.toString() << "|";
    }

    const std::string rawData = ss.str();

    HashUtil hasher;
    std::string digest;
    std::string errorMessage;

    if (!hasher.sha256(rawData, digest, errorMessage)) {
        throw std::runtime_error("Block SHA-256 calculation failed: " + errorMessage);
    }

    return digest;
}

// Read-Only Accessors
std::string Block::getHash() const {
    return hash;
}

std::string Block::getPreviousHash() const {
    return previousHash;
}

const std::vector<Transaction>& Block::getTransactions() const {
    return transactions;
}

std::size_t Block::getIndex() const {
    return index;
}

std::int64_t Block::getTimestamp() const {
    return timestamp;
}

// Formats block and transaction details for console display
std::string Block::toString() const {
    std::stringstream ss;
    ss << "Block #" << index << "\n"
       << "Timestamp: " << timestamp << "\n"
       << "Previous Hash: " << previousHash << "\n"
       << "Hash: " << hash << "\n"
       << "Transactions (" << transactions.size() << "):\n";

    if (transactions.empty()) {
        ss << "  [No Transactions - Genesis/Empty Block]\n";
    } else {
        for (const Transaction& tx : transactions) {
            ss << "  - " << tx.toString() << "\n";
        }
    }

    return ss.str();
}