
#include "Blockchain.h"

#include <limits>
#include <map>
#include <string>
#include <vector>

// Creates the blockchain and adds the genesis block.
Blockchain::Blockchain() : blockSize(10) {
    // The genesis block has index 0 and no transactions.
    std::vector<Transaction> genesisTransactions;

    // The genesis block has no previous block, so its previous hash is "0".
    Block genesisBlock(0, genesisTransactions, "0");

    // Store the genesis block as the first block in the chain.
    chain.push_back(genesisBlock);
}

// Checks whether a transaction is valid before accepting it.
bool Blockchain::isTransactionValid(const Transaction& transaction) const {
    // Read the transaction details.
    const std::string sender = transaction.getSender();
    const std::string receiver = transaction.getReceiver();
    const std::int64_t amount = transaction.getAmount();

    // Addresses must not be empty.
    if (sender.empty() || receiver.empty()) {
        return false;
    }

    // An account cannot transfer money to itself.
    if (sender == receiver) {
        return false;
    }

    // The transfer amount must be positive.
    if (amount <= 0) {
        return false;
    }

    // Find the sender's confirmed balance.
    auto senderIt = balances.find(sender);

    // An address that is not present has a balance of zero.
    std::int64_t senderBalance = 0;

    if (senderIt != balances.end()) {
        senderBalance = senderIt->second;
    }

    // A negative balance indicates an invalid ledger state.
    if (senderBalance < 0) {
        return false;
    }

    // Calculate how much the sender has already committed
    // to transactions that are still waiting to be mined.
    std::int64_t pendingOutgoing = 0;

    for (const Transaction& pending : pendingTransactions) {
        if (pending.getSender() == sender) {
            const std::int64_t pendingAmount = pending.getAmount();

            // Pending outgoing transactions must be positive.
            if (pendingAmount <= 0) {
                return false;
            }

            // Check for overflow before adding the amount.
            if (pendingOutgoing >
                std::numeric_limits<std::int64_t>::max() - pendingAmount) {
                return false;
            }

            pendingOutgoing += pendingAmount;
        }
    }

    // Pending transfers must not exceed the sender's balance.
    if (pendingOutgoing > senderBalance) {
        return false;
    }

    // Calculate the balance that remains available to spend.
    const std::int64_t availableBalance =
        senderBalance - pendingOutgoing;

    // Reject a transfer that exceeds the available balance.
    if (amount > availableBalance) {
        return false;
    }

    // Find the receiver's confirmed balance.
    auto receiverIt = balances.find(receiver);

    std::int64_t receiverBalance = 0;

    if (receiverIt != balances.end()) {
        receiverBalance = receiverIt->second;
    }

    // A negative receiver balance indicates an invalid ledger state.
    if (receiverBalance < 0) {
        return false;
    }

    // Calculate the receiver's pending incoming transactions.
    std::int64_t pendingIncoming = 0;

    for (const Transaction& pending : pendingTransactions) {
        if (pending.getReceiver() == receiver) {
            const std::int64_t pendingAmount = pending.getAmount();

            // Incoming pending transactions must also be positive.
            if (pendingAmount <= 0) {
                return false;
            }

            // Prevent overflow while calculating pending incoming funds.
            if (pendingIncoming >
                std::numeric_limits<std::int64_t>::max() - pendingAmount) {
                return false;
            }

            pendingIncoming += pendingAmount;
        }
    }

    const std::int64_t maximum =
        std::numeric_limits<std::int64_t>::max();

    // Make sure the receiver's projected balance does not overflow.
    if (pendingIncoming > maximum - receiverBalance) {
        return false;
    }

    const std::int64_t projectedReceiverBalance =
        receiverBalance + pendingIncoming;

    // Check whether this new transfer would overflow the receiver's balance.
    if (amount > maximum - projectedReceiverBalance) {
        return false;
    }

    // All validation checks passed.
    return true;
}

// Adds a valid transaction to the pending transaction list.
bool Blockchain::addTransaction(const Transaction& transaction) {
    // A block must have room for at least one transaction.
    if (blockSize == 0) {
        return false;
    }

    // Do not allow more pending transactions than the block can hold.
    if (pendingTransactions.size() >= blockSize) {
        return false;
    }

    // Reject invalid transactions before storing them.
    if (!isTransactionValid(transaction)) {
        return false;
    }

    // Store the transaction until the next block is mined.
    pendingTransactions.push_back(transaction);

    return true;
}

// Creates a block from the pending transactions and updates the ledger.
bool Blockchain::minePendingTransactions() {
    // The chain must contain a genesis block.
    if (chain.empty()) {
        return false;
    }

    // There is nothing to mine if no transactions are pending.
    if (pendingTransactions.empty()) {
        return false;
    }

    // Ensure the pending transaction count fits in one block.
    if (pendingTransactions.size() > blockSize) {
        return false;
    }

    // Work on a copy so failed validation does not partially
    // update the actual account balances.
    std::map<std::string, std::int64_t> updatedBalances = balances;

    const std::int64_t maximum =
        std::numeric_limits<std::int64_t>::max();

    // Validate and apply each transaction to the temporary ledger.
    for (const Transaction& transaction : pendingTransactions) {
        const std::string sender = transaction.getSender();
        const std::string receiver = transaction.getReceiver();
        const std::int64_t amount = transaction.getAmount();

        // Recheck basic transaction conditions before committing.
        if (sender.empty() || receiver.empty()) {
            return false;
        }

        if (sender == receiver || amount <= 0) {
            return false;
        }

        // Find the sender's balance in the temporary ledger.
        auto senderIt = updatedBalances.find(sender);

        // The sender must have sufficient confirmed funds.
        if (senderIt == updatedBalances.end()) {
            return false;
        }

        if (senderIt->second < amount) {
            return false;
        }

        // Find the receiver's balance.
        auto receiverIt = updatedBalances.find(receiver);

        std::int64_t receiverBalance = 0;

        if (receiverIt != updatedBalances.end()) {
            receiverBalance = receiverIt->second;
        }

        // Do not allow negative balances or integer overflow.
        if (receiverBalance < 0) {
            return false;
        }

        if (amount > maximum - receiverBalance) {
            return false;
        }

        // Apply the transfer to the temporary ledger.
        senderIt->second -= amount;

        // Insert a receiver entry if this address is new.
        if (receiverIt == updatedBalances.end()) {
            updatedBalances.emplace(receiver, amount);
        } else {
            receiverIt->second += amount;
        }
    }

    // Get the last block to link the new block to it.
    const Block& latestBlock = chain.back();

    // Prevent the block index from overflowing.
    if (latestBlock.getIndex() ==
        std::numeric_limits<std::size_t>::max()) {
        return false;
    }

    // Create the new block with the pending transactions.
    Block newBlock(
        latestBlock.getIndex() + 1,
        pendingTransactions,
        latestBlock.getHash()
    );

    // Append the block before changing the actual ledger.
    // If this operation fails, the balances remain unchanged.
    chain.push_back(newBlock);

    // Commit the updated ledger.
    balances.swap(updatedBalances);

    // Remove transactions that have now been committed.
    pendingTransactions.clear();

    return true;
}

// Checks the hashes, links, indexes, and basic chain structure.
bool Blockchain::isChainValid() const {
    // A valid blockchain must contain at least its genesis block.
    if (chain.empty()) {
        return false;
    }

    // The first block must have index zero.
    if (chain[0].getIndex() != 0) {
        return false;
    }

    // The genesis block must not point to a previous block.
    if (chain[0].getPreviousHash() != "0") {
        return false;
    }

    // Check every block in the chain.
    for (std::size_t i = 0; i < chain.size(); ++i) {
        const Block& currentBlock = chain[i];

        // Recalculate the hash and compare it with the stored hash.
        if (currentBlock.getHash() != currentBlock.calculateHash()) {
            return false;
        }

        // The genesis block has no previous block to compare against.
        if (i == 0) {
            continue;
        }

        const Block& previousBlock = chain[i - 1];

        // The current block must point to the previous block's hash.
        if (currentBlock.getPreviousHash() != previousBlock.getHash()) {
            return false;
        }

        // Prevent overflow before checking the consecutive index.
        if (previousBlock.getIndex() ==
            std::numeric_limits<std::size_t>::max()) {
            return false;
        }

        // Block indexes must increase by exactly one.
        if (currentBlock.getIndex() != previousBlock.getIndex() + 1) {
            return false;
        }
    }

    // All implemented structural checks passed.
    return true;
}

// Returns the confirmed balance for an address.
std::int64_t Blockchain::getBalance(const std::string& address) const {
    // Search for the account in the balance ledger.
    auto it = balances.find(address);

    // An address not found in the ledger has a balance of zero.
    if (it == balances.end()) {
        return 0;
    }

    return it->second;
}

// Returns a reference to the most recently added block.
const Block& Blockchain::getLatestBlock() const {
    // The constructor creates the genesis block, so the chain
    // should not be empty during normal operation.
    return chain.back();
}

// Returns the complete chain as a read-only reference.
const std::vector<Block>& Blockchain::getChain() const {
    return chain;
}

// Returns the pending transactions as a read-only reference.
const std::vector<Transaction>& Blockchain::getPendingTransactions() const {
    return pendingTransactions;
}
