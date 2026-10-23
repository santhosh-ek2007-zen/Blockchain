#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>
#include "include/Blockchain.h"
#include "include/Block.h"
#include "include/Transaction.h"
#include "include/TerminalUI.h"

// Safely read a complete line and parse a 64-bit integer.
bool readIntegerInput(const std::string& prompt, std::int64_t& value, bool& parseSuccess) {
    std::string line;
    parseSuccess = false;

    std::cout << prompt;

    if (!std::getline(std::cin, line)) {
        return false;
    }

    std::stringstream input(line);
    std::int64_t enteredValue;
    char extra;

    if ((input >> enteredValue) && !(input >> extra)) {
        value = enteredValue;
        parseSuccess = true;
    }

    return true;
}

// Read transaction details and submit them to Blockchain.
void handleTransactionInput(Blockchain& blockchain) {
    std::string sender;
    std::string receiver;
    std::int64_t amount = 0;
    bool parseSuccess = false;

    TerminalUI::printSection("CREATE NEW TRANSACTION");

    std::cout << "  Enter sender account   : ";
    if (!std::getline(std::cin, sender)) {
        return;
    }

    std::cout << "  Enter receiver account : ";
    if (!std::getline(std::cin, receiver)) {
        return;
    }

    if (!readIntegerInput("  Enter transfer amount  : ", amount, parseSuccess)) {
        return;
    }

    if (!parseSuccess) {
        TerminalUI::displayTransactionResult(
            false, sender, receiver, 0,
            "Invalid amount format. Please enter a valid positive integer."
        );
        TerminalUI::promptContinue();
        return;
    }

    if (amount <= 0) {
        TerminalUI::displayTransactionResult(
            false, sender, receiver, amount,
            "Transfer amount must be a positive integer greater than zero."
        );
        TerminalUI::promptContinue();
        return;
    }

    if (sender.empty() || receiver.empty()) {
        TerminalUI::displayTransactionResult(
            false, sender, receiver, amount,
            "Sender and receiver account names cannot be empty."
        );
        TerminalUI::promptContinue();
        return;
    }

    if (sender == receiver) {
        TerminalUI::displayTransactionResult(
            false, sender, receiver, amount,
            "Self-transfer not allowed: sender and receiver accounts must differ."
        );
        TerminalUI::promptContinue();
        return;
    }

    // Inspect available balance for diagnostic explanation if rejected
    std::int64_t senderBalance = blockchain.getBalance(sender);
    std::int64_t pendingOutgoing = 0;
    for (const Transaction& pending : blockchain.getPendingTransactions()) {
        if (pending.getSender() == sender) {
            pendingOutgoing += pending.getAmount();
        }
    }
    std::int64_t availableBalance = senderBalance - pendingOutgoing;

    Transaction transaction(sender, receiver, amount);

    if (blockchain.addTransaction(transaction)) {
        TerminalUI::displayTransactionResult(
            true, sender, receiver, amount,
            "Transaction accepted and added to the pending transaction pool."
        );
    } else {
        std::string explanation;
        if (amount > availableBalance) {
            explanation = "Insufficient funds: sender '" + sender + "' has confirmed balance of " +
                          std::to_string(senderBalance) + " and " + std::to_string(pendingOutgoing) +
                          " pending outgoing (available: " + std::to_string(availableBalance) + " coins).";
        } else if (blockchain.getPendingTransactions().size() >= 10) {
            explanation = "Block capacity limit reached (10 txs). Mine pending transactions first.";
        } else {
            explanation = "Transaction rejected by blockchain validation rules.";
        }

        TerminalUI::displayTransactionResult(
            false, sender, receiver, amount, explanation
        );
    }

    TerminalUI::promptContinue();
}

// Commit pending transactions into a new block.
void handleBlockCreation(Blockchain& blockchain) {
    TerminalUI::printSection("MINE PENDING TRANSACTIONS / CREATE BLOCK");

    const std::vector<Transaction>& pending = blockchain.getPendingTransactions();

    if (pending.empty()) {
        TerminalUI::displayBlockCreationResult(
            false, 0, "", "", 0,
            "No pending transactions in pool. Create and queue a transaction first."
        );
        TerminalUI::promptContinue();
        return;
    }

    std::size_t pendingCount = pending.size();

    if (blockchain.minePendingTransactions()) {
        const Block& latestBlock = blockchain.getLatestBlock();
        TerminalUI::displayBlockCreationResult(
            true,
            latestBlock.getIndex(),
            latestBlock.getPreviousHash(),
            latestBlock.getHash(),
            latestBlock.getTransactions().size(),
            "Block successfully created, linked to chain, and ledger balances updated (" +
            std::to_string(pendingCount) + " transaction(s) committed)."
        );
    } else {
        TerminalUI::displayBlockCreationResult(
            false, 0, "", "", 0,
            "Block creation failed. Verify pending transaction batch integrity and capacity."
        );
    }

    TerminalUI::promptContinue();
}

// Display all committed blocks.
void handleChainDisplay(const Blockchain& blockchain) {
    TerminalUI::displayBlockchain(blockchain.getChain());
    TerminalUI::promptContinue();
}

// Query an account balance.
void handleBalanceQuery(const Blockchain& blockchain) {
    std::string account;

    TerminalUI::printSection("CHECK ACCOUNT BALANCE");
    std::cout << "  Enter account name: ";

    if (!std::getline(std::cin, account)) {
        return;
    }

    if (account.empty()) {
        TerminalUI::displayWarningMessage("Account name cannot be empty.");
        TerminalUI::promptContinue();
        return;
    }

    std::int64_t confirmedBalance = blockchain.getBalance(account);
    std::int64_t pendingOutgoing = 0;
    std::int64_t pendingIncoming = 0;

    for (const Transaction& tx : blockchain.getPendingTransactions()) {
        if (tx.getSender() == account) {
            pendingOutgoing += tx.getAmount();
        }
        if (tx.getReceiver() == account) {
            pendingIncoming += tx.getAmount();
        }
    }

    TerminalUI::displayBalance(account, confirmedBalance, pendingOutgoing, pendingIncoming);
    TerminalUI::promptContinue();
}

// Validate block hashes and previous-hash links.
void handleValidation(const Blockchain& blockchain) {
    const std::vector<Block>& chain = blockchain.getChain();
    bool overallValid = blockchain.isChainValid();

    bool genesisValid = !chain.empty() && chain[0].getIndex() == 0 && chain[0].getPreviousHash() == "0";
    bool indexesValid = true;
    bool hashesValid = true;
    bool linksValid = true;

    for (std::size_t i = 0; i < chain.size(); ++i) {
        if (chain[i].getHash() != chain[i].calculateHash()) {
            hashesValid = false;
        }
        if (i > 0) {
            if (chain[i].getPreviousHash() != chain[i - 1].getHash()) {
                linksValid = false;
            }
            if (chain[i].getIndex() != chain[i - 1].getIndex() + 1) {
                indexesValid = false;
            }
        }
    }

    TerminalUI::displayValidationResult(
        overallValid, chain.size(), genesisValid, hashesValid, linksValid, indexesValid
    );
    TerminalUI::promptContinue();
}

// Display transactions waiting to be committed.
void handlePendingTransactions(const Blockchain& blockchain) {
    TerminalUI::displayPendingTransactions(blockchain.getPendingTransactions());
    TerminalUI::promptContinue();
}

// Demonstrate how changing a block copy changes its hash.
// The committed blockchain remains untouched.
void handleTamperingDemo(const Blockchain& blockchain) {
    const std::vector<Block>& chain = blockchain.getChain();

    if (chain.size() < 2) {
        TerminalUI::printSection("CONTROLLED TAMPERING DEMONSTRATION");
        TerminalUI::displayWarningMessage(
            "Requires at least one committed block after the genesis block (Block #1)."
        );
        TerminalUI::displayInfoMessage(
            "Please create a transaction [Option 1] and mine a block [Option 2] first."
        );
        TerminalUI::promptContinue();
        return;
    }

    const Block& originalBlock = chain[1];
    const std::string originalStoredHash = originalBlock.getHash();

    // Create an isolated local copy of Block #1
    Block alteredBlock = originalBlock;

    Transaction fakeTransaction(
        "TamperDemoSender",
        "TamperDemoReceiver",
        1
    );

    // Tamper with the copy by injecting the unauthorized transaction
    alteredBlock.addTransaction(fakeTransaction);
    const std::string alteredHash = alteredBlock.calculateHash();

    // Check live chain validity
    bool chainStillValid = blockchain.isChainValid();

    TerminalUI::displayTamperingDemo(
        originalBlock, originalStoredHash,
        alteredBlock, alteredHash,
        chainStillValid
    );
    TerminalUI::promptContinue();
}

int main() {
    TerminalUI::initialize();

    Blockchain blockchain;

    bool running = true;

    while (running) {
        TerminalUI::printDashboard(
            blockchain.getChain().size(),
            blockchain.getPendingTransactions().size(),
            blockchain.isChainValid()
        );

        std::int64_t choice = -1;
        bool parseSuccess = false;

        if (!readIntegerInput("\n  Select operation [0-7]: ", choice, parseSuccess)) {
            break;
        }

        if (!parseSuccess) {
            TerminalUI::displayErrorMessage("Invalid input format. Please enter a whole number from 0 to 7.");
            TerminalUI::promptContinue();
            continue;
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
                TerminalUI::displayExitBanner();
                break;

            default:
                TerminalUI::displayErrorMessage("Invalid menu selection. Please choose an option between 0 and 7.");
                TerminalUI::promptContinue();
                break;
        }
    }

    return 0;
}
