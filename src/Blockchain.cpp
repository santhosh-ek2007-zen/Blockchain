#include "Blockchain.h"
#include <limits>


// 1. Constructor
Blockchain::Blockchain() : blockSize(10){
    // Create the genesis block automatically.
    vector<Transaction> genesisTransactions;
    Block genesisBlock(0, genesisTransactions, "0"); // first block in block chain is called genesis block
    chain.push_back(genesisBlock);
}


// 2. Validate a transaction
bool Blockchain::isTransactionValid(const Transaction& transaction)const{
    string sender = transaction.getSender();
    string receiver = transaction.getReceiver();
    int64_t amount = transaction.getAmount();

    // Basic validation.
    if (sender.empty() || receiver.empty())
        return false;

    if (sender == receiver)
        return false;

    if (amount <= 0)
        return false;

    // Check the sender's confirmed balance.
    auto senderIt = balances.find(sender);
    int64_t senderBalance = 0;
    if (senderIt != balances.end())
        senderBalance = senderIt->second;

    // Recalculate pending outgoing transactions.
    int64_t pendingOutgoing = 0;
    for(const Transaction& pending : pendingTransactions){
        if(pending.getSender() == sender){
            int64_t pendingAmount = pending.getAmount();
            if (pendingAmount <= 0)
                return false;
            // Prevent integer overflow.
            if (pendingOutgoing > numeric_limits<int64_t>::max() - pendingAmount){
                return false;
            }
            pendingOutgoing += pendingAmount;
        }
    }

    // Reject inconsistent balance states.
    if (senderBalance < 0 || pendingOutgoing > senderBalance){
        return false;
    }

    int64_t availableBalance = senderBalance - pendingOutgoing;

    if (amount > availableBalance)
        return false;

    // Ensure the receiver's balance cannot overflow.
    auto receiverIt = balances.find(receiver);

    int64_t receiverBalance = 0;

    if (receiverIt != balances.end())
        receiverBalance = receiverIt->second;

    if (receiverBalance < 0)
        return false;

    int64_t pendingIncoming = 0;

    for(const Transaction& pending : pendingTransactions){
        if(pending.getReceiver() == receiver){
            int64_t pendingAmount = pending.getAmount();
            if (pendingAmount <= 0 || pendingIncoming >numeric_limits<int64_t>::max() - pendingAmount){
                return false;
            }
            pendingIncoming += pendingAmount;
        }
    }

    const int64_t maximum =
        numeric_limits<int64_t>::max();

    if(pendingIncoming > maximum - receiverBalance)
        return false;

    int64_t projectedReceiverBalance = receiverBalance + pendingIncoming;

    if (amount > maximum - projectedReceiverBalance)
        return false;
    return true;
}
// 3. Add a transaction

bool Blockchain::addTransaction(const Transaction& transaction){
    // A block must have a positive capacity.
    if (blockSize == 0)
        return false;

    // Do not accept more pending transactions than
    // the configured block capacity.
    if (pendingTransactions.size() >= blockSize)
        return false;
    if (!isTransactionValid(transaction))
        return false;
    pendingTransactions.push_back(transaction);
    return true;
}

// 4. Create and commit a block
bool Blockchain::minePendingTransactions(){
    if(chain.empty())
        return false;
    if(pendingTransactions.empty())
        return false;
    if(pendingTransactions.size() > blockSize)
        return false;

    // Prepare a separate ledger first so that failed
    // balance checks do not partially update balances.
    map<string, int64_t> updatedBalances = balances;

    const int64_t maximum = numeric_limits<int64_t>::max();

    for(const Transaction& transaction : pendingTransactions){
        string sender = transaction.getSender();
        string receiver = transaction.getReceiver();
        int64_t amount = transaction.getAmount();

        if (amount <= 0 || sender == receiver)
            return false;

        auto senderIt = updatedBalances.find(sender);

        if (senderIt == updatedBalances.end())
            return false;

        // Check funds before subtracting.
        if (senderIt->second < amount)
            return false;

        auto receiverIt = updatedBalances.find(receiver);

        int64_t receiverBalance = 0;

        if (receiverIt != updatedBalances.end())
            receiverBalance = receiverIt->second;

        // Check for overflow before adding.
        if (receiverBalance < 0 || amount > maximum - receiverBalance){
            return false;
        }

        // Apply changes to the temporary ledger.
        updatedBalances[sender] -= amount;
        updatedBalances[receiver] += amount;
    }

    // Construct the next block.
    const Block& latestBlock = chain.back();

    Block newBlock(latestBlock.getIndex() + 1,pendingTransactions,latestBlock.getHash());

    // Commit the block first.
    chain.push_back(newBlock);

    // Publish the updated ledger and clear committed
    // transactions only after the block is appended.
    balances.swap(updatedBalances);
    pendingTransactions.clear();

    return true;
}
// 5. Validate the blockchain
bool Blockchain::isChainValid() const
{
    if(chain.empty())
        return false;

    // Verify the genesis block convention.
    if(chain[0].getIndex() != 0)
        return false;

    if(chain[0].getPreviousHash() != "0")
        return false;

    for (size_t i = 0; i < chain.size(); i++){
        const Block& currentBlock = chain[i];

        // Recalculate and compare the block hash.
        if(currentBlock.getHash() != currentBlock.calculateHash()){
            return false;
        }

        if(i > 0){
            const Block& previousBlock = chain[i - 1];

            // Verify the previous-hash link.
            if (currentBlock.getPreviousHash() != previousBlock.getHash()){
                return false;
            }

            // Verify consecutive block indexes.
            if (currentBlock.getIndex() != previousBlock.getIndex() + 1){
                return false;
            }
        }
    }

    return true;
}

// 6. Get an account's balance
int64_t Blockchain::getBalance(const string& address)const{
    auto it = balances.find(address);
    if (it == balances.end())
        return 0;
    return it->second;
}

// 7. Get the latest block
const Block& Blockchain::getLatestBlock() const{
    return chain.back();
}

// 8. Get the entire chain (read-only)
const vector<Block>& Blockchain::getChain() const{
    return chain;
}

// 9. Get pending transactions (read-only)
const vector<Transaction>&Blockchain::getPendingTransactions() const{
    return pendingTransactions;
}