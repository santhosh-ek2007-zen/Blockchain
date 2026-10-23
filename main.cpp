#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>
#include "include/Blockchain.h"
#include "include/Block.h"
#include "include/Transaction.h"

// Read a complete line and validate integer input.
bool readInteger(const std::string& prompt, std::int64_t& value) {
    std::string line;

    std::cout << prompt;

    if (!std::getline(std::cin, line)) {
        return false;
    }

    std::stringstream input(line);
    std::int64_t enteredValue;
    char extra;

    if (!(input >> enteredValue) || (input >> extra)) {
        std::cout << "Invalid input. Please enter a whole number.\n";
        return true;
    }

    value = enteredValue;
    return true;
}

// Display the available operations.
void showMenu() {
    std::cout << "\n========== C++ BLOCKCHAIN MVP ==========\n";
    std::cout << "1. Create a transaction\n";
    std::cout << "2. Mine pending transactions / create a block\n";
    std::cout << "3. Display the blockchain\n";
    std::cout << "4. Check account balance\n";
    std::cout << "5. Validate the blockchain\n";
    std::cout << "6. Demonstrate hash changes after tampering\n";
    std::cout << "7. View pending transactions\n";
    std::cout << "0. Exit\n";
    std::cout << "========================================\n";
}

// Read transaction details and submit them to Blockchain.
void handleTransactionInput(Blockchain& blockchain) {
    std::string sender;
    std::string receiver;
    std::int64_t amount = 0;

    std::cout << "\n--- Create Transaction ---\n";

    std::cout << "Enter sender account: ";
    if (!std::getline(std::cin, sender)) {
        return;
    }

    std::cout << "Enter receiver account: ";
    if (!std::getline(std::cin, receiver)) {
        return;
    }

    if (!readInteger("Enter amount: ", amount)) {
        return;
    }

    if (amount <= 0) {
        std::cout << "Rejected: amount must be positive.\n";
        return;
    }

    Transaction transaction(sender, receiver, amount);

    if (blockchain.addTransaction(transaction)) {
        std::cout << "Transaction accepted into the pending pool.\n";
        std::cout << "Sender: " << sender << '\n';
        std::cout << "Receiver: " << receiver << '\n';
        std::cout << "Amount: " << amount << '\n';
    } else {
        std::cout << "Transaction rejected.\n";
        std::cout << "Check the account names, transfer rules, "
                     "and available funds.\n";
    }
}

// Commit pending transactions into a new block.
void handleBlockCreation(Blockchain& blockchain) {
    std::cout << "\n--- Create Block ---\n";

    if (blockchain.getPendingTransactions().empty()) {
        std::cout << "No pending transactions. Create a transaction first.\n";
        return;
    }

    if (blockchain.minePendingTransactions()) {
        std::cout << "Block created and committed successfully.\n";

        const Block& latestBlock = blockchain.getLatestBlock();

        std::cout << "Block index: "
                  << latestBlock.getIndex() << '\n';
        std::cout << "Previous hash: "
                  << latestBlock.getPreviousHash() << '\n';
        std::cout << "Block hash: "
                  << latestBlock.getHash() << '\n';
    } else {
        std::cout << "Block creation failed. Check the blockchain rules.\n";
    }
}

// Display all committed blocks.
void handleChainDisplay(const Blockchain& blockchain) {
    std::cout << "\n--- Committed Blockchain ---\n";

    const std::vector<Block>& chain = blockchain.getChain();

    if (chain.empty()) {
        std::cout << "The chain is empty.\n";
        return;
    }

    for (const Block& block : chain) {
        std::cout << block.toString() << '\n';
        std::cout << "----------------------------------------\n";
    }

    std::cout << "Total blocks: " << chain.size() << '\n';
}

// Query an account balance.
void handleBalanceQuery(const Blockchain& blockchain) {
    std::string account;

    std::cout << "\n--- Account Balance ---\n";
    std::cout << "Enter account name: ";

    if (!std::getline(std::cin, account)) {
        return;
    }

    std::cout << "Balance of " << account << ": "
              << blockchain.getBalance(account) << '\n';
}

// Validate block hashes and previous-hash links.
void handleValidation(const Blockchain& blockchain) {
    std::cout << "\n--- Blockchain Validation ---\n";

    if (blockchain.isChainValid()) {
        std::cout << "Chain is valid.\n";
        std::cout << "Stored block hashes and chain links passed validation.\n";
    } else {
        std::cout << "WARNING: Chain validation failed.\n";
        std::cout << "A stored hash or block link may be inconsistent.\n";
    }
}

// Display transactions waiting to be committed.
void handlePendingTransactions(const Blockchain& blockchain) {
    std::cout << "\n--- Pending Transactions ---\n";

    const std::vector<Transaction>& pending =
        blockchain.getPendingTransactions();

    if (pending.empty()) {
        std::cout << "No pending transactions.\n";
        return;
    }

    for (std::size_t i = 0; i < pending.size(); ++i) {
        std::cout << "Transaction " << i + 1 << '\n';
        std::cout << "  Sender: "
                  << pending[i].getSender() << '\n';
        std::cout << "  Receiver: "
                  << pending[i].getReceiver() << '\n';
        std::cout << "  Amount: "
                  << pending[i].getAmount() << '\n';
    }

    std::cout << "Pending count: " << pending.size() << '\n';
}

// Demonstrate how changing a block copy changes its hash.
// The committed blockchain remains untouched.
void handleTamperingDemo(const Blockchain& blockchain) {
    std::cout << "\n--- Controlled Tampering Demonstration ---\n";

    const std::vector<Block>& chain = blockchain.getChain();

    if (chain.size() < 2) {
        std::cout << "Mine at least one block after the genesis block first.\n";
        return;
    }

    const Block& originalBlock = chain[1];
    const std::string originalStoredHash = originalBlock.getHash();

    Block alteredBlock = originalBlock;

    Transaction fakeTransaction(
        "TamperDemoSender",
        "TamperDemoReceiver",
        1
    );

    alteredBlock.addTransaction(fakeTransaction);

    const std::string alteredHash = alteredBlock.calculateHash();

    std::cout << "Original block index: "
              << originalBlock.getIndex() << '\n';
    std::cout << "Original stored hash: "
              << originalStoredHash << '\n';
    std::cout << "Hash after changing the copy: "
              << alteredHash << '\n';

    if (originalStoredHash != alteredHash) {
        std::cout << "Result: changing the block contents changed its hash.\n";
        std::cout << "The original committed chain remains untouched.\n";
    } else {
        std::cout << "Unexpected result: hashes match. Inspect Block hashing.\n";
    }

    std::cout << "\nValidating the actual chain again...\n";

    if (blockchain.isChainValid()) {
        std::cout << "The original chain remains valid.\n";
    } else {
        std::cout << "The original chain is invalid.\n";
    }
}

int main() {
    Blockchain blockchain;

    std::cout << "Welcome to the C++ Transaction-Based Blockchain!\n";
    std::cout << "The blockchain has been initialized.\n";

    bool running = true;

    while (running) {
        showMenu();

        std::int64_t choice = -1;

        if (!readInteger("Choose an option: ", choice)) {
            break;
        }

        switch (choice) {
            case 1:
                handleTransactionInput(blockchain);
                break;

            case 2:
                handleBlockCreation(blockchain);
                break;

            case 3:
                handleChainDisplay(blockchain);
                break;

            case 4:
                handleBalanceQuery(blockchain);
                break;

            case 5:
                handleValidation(blockchain);
                break;

            case 6:
                handleTamperingDemo(blockchain);
                break;

            case 7:
                handlePendingTransactions(blockchain);
                break;

            case 0:
                running = false;
                std::cout << "Exiting the blockchain application.\n";
                break;

            default:
                std::cout << "Invalid menu option. Choose 0 to 7.\n";
                break;
        }
    }

    return 0;
}
