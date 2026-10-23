#ifndef TERMINAL_UI_H
#define TERMINAL_UI_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include "Block.h"
#include "Transaction.h"

class TerminalUI {
public:
    enum class StatusType {
        Success,
        Warning,
        Error,
        Info
    };

    // Terminal initialization and color controls
    static void initialize();
    static void setColorEnabled(bool enabled);
    static bool isColorEnabled();

    // Box and line drawing helpers
    static void printDashboard(std::size_t totalBlocks,
                               std::size_t pendingCount,
                               bool isChainValid,
                               std::size_t blockCapacity = 10);

    static void printHeader(const std::string& title, const std::string& subtitle = "");
    static void printSection(const std::string& title);
    static void printDivider(int width = 76, char ch = '-');
    static void printDoubleDivider(int width = 76);
    static void printStatus(const std::string& label, const std::string& message, StatusType type);

    // Operation result displays
    static void displayTransactionResult(bool accepted,
                                         const std::string& sender,
                                         const std::string& receiver,
                                         std::int64_t amount,
                                         const std::string& explanation);

    static void displayBlockCreationResult(bool success,
                                           std::size_t index,
                                           const std::string& prevHash,
                                           const std::string& hash,
                                           std::size_t txCount,
                                           const std::string& explanation);

    static void displayBlockchain(const std::vector<Block>& chain);

    static void displayBalance(const std::string& account,
                               std::int64_t confirmedBalance,
                               std::int64_t pendingOutgoing = 0,
                               std::int64_t pendingIncoming = 0);

    static void displayValidationResult(bool isValid,
                                        std::size_t blockCount,
                                        bool genesisValid,
                                        bool hashesValid,
                                        bool linksValid,
                                        bool indexesValid);

    static void displayTamperingDemo(const Block& originalBlock,
                                    const std::string& originalStoredHash,
                                    const Block& alteredBlock,
                                    const std::string& alteredHash,
                                    bool chainStillValid);

    static void displayPendingTransactions(const std::vector<Transaction>& pending,
                                           std::size_t capacity = 10);

    // Messages and prompts
    static void displayErrorMessage(const std::string& message);
    static void displaySuccessMessage(const std::string& message);
    static void displayWarningMessage(const std::string& message);
    static void displayInfoMessage(const std::string& message);
    static void displayExitBanner();
    static void promptContinue();

    // String formatting utilities
    static std::string formatTimestamp(std::int64_t timestamp);
    static std::size_t visibleLength(const std::string& str);
    static std::string padLine(const std::string& content, std::size_t innerWidth = 72);
};

#endif
