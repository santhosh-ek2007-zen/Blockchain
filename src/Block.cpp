#include "../include/Block.h"
// Uncomment the line below once HashUtil.h is added to the include/ folder:
// #include "../include/HashUtil.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <functional>

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

    // -------------------------------------------------------------------------
    // OPTION A: Once HashUtil is ready, uncomment this line and remove Option B:
    // return HashUtil::sha256(rawData);
    // -------------------------------------------------------------------------

    // OPTION B: Temporary deterministic hex hash fallback (so it compiles without HashUtil)
    std::size_t h1 = std::hash<std::string>{}(rawData);
    std::size_t h2 = std::hash<std::string>{}(rawData + "salt_block_256");
    std::stringstream hexStream;
    hexStream << std::hex << std::setfill('0') 
              << std::setw(16) << h1 
              << std::setw(16) << h2;
    return hexStream.str();
}

// Read-Only Accessors
std::string Block::getHash() const {
    return hash;
}

std::string Block::getPreviousHash() const {
    return previousHash;
}

std::vector<Transaction> Block::getTransactions() const {
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