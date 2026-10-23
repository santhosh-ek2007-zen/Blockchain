#include "../include/TerminalUI.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#define ISATTY _isatty
#define FILENO _fileno
#else
#include <unistd.h>
#define ISATTY isatty
#define FILENO fileno
#endif

namespace {
    bool g_colorEnabled = false;

    // ANSI Color and Style Codes
    namespace ANSI {
        const char* const Reset        = "\033[0m";
        const char* const Bold         = "\033[1m";
        const char* const Dim          = "\033[2m";
        const char* const Red          = "\033[31m";
        const char* const BrightRed    = "\033[91m";
        const char* const Green        = "\033[32m";
        const char* const BrightGreen  = "\033[92m";
        const char* const Yellow       = "\033[33m";
        const char* const BrightYellow = "\033[93m";
        const char* const Blue         = "\033[34m";
        const char* const BrightBlue   = "\033[94m";
        const char* const Magenta      = "\033[35m";
        const char* const BrightMag    = "\033[95m";
        const char* const Cyan         = "\033[36m";
        const char* const BrightCyan   = "\033[96m";
        const char* const White        = "\033[37m";
        const char* const BrightWhite  = "\033[97m";
        const char* const Gray         = "\033[90m";
    }

    std::string style(const std::string& text, const char* code) {
        if (!g_colorEnabled || text.empty()) {
            return text;
        }
        return std::string(code) + text + ANSI::Reset;
    }

    std::string repeatChar(char ch, std::size_t count) {
        return std::string(count, ch);
    }

    // Prints a labeled field, wrapping value words if it exceeds maxWidth.
    void printWrappedField(const std::string& label,
                            const std::string& value,
                            const char* valueColor = nullptr,
                            std::size_t innerWidth = 72) {
        std::size_t labelVisLen = TerminalUI::visibleLength(label);
        std::size_t availWidth = (innerWidth > labelVisLen) ? innerWidth - labelVisLen : 20;

        std::istringstream words(value);
        std::string word;
        std::string currentLine;
        bool isFirst = true;

        while (words >> word) {
            if (currentLine.empty()) {
                currentLine = word;
            } else if (currentLine.size() + 1 + word.size() <= availWidth) {
                currentLine += " " + word;
            } else {
                std::string styledValue = valueColor ? style(currentLine, valueColor) : currentLine;
                if (isFirst) {
                    std::cout << TerminalUI::padLine(label + styledValue, innerWidth) << '\n';
                    isFirst = false;
                } else {
                    std::cout << TerminalUI::padLine(repeatChar(' ', labelVisLen) + styledValue, innerWidth) << '\n';
                }
                currentLine = word;
            }
        }

        if (!currentLine.empty() || isFirst) {
            std::string styledValue = valueColor ? style(currentLine, valueColor) : currentLine;
            if (isFirst) {
                std::cout << TerminalUI::padLine(label + styledValue, innerWidth) << '\n';
            } else {
                std::cout << TerminalUI::padLine(repeatChar(' ', labelVisLen) + styledValue, innerWidth) << '\n';
            }
        }
    }
}

void TerminalUI::initialize() {
    bool supported = false;

    // Check if NO_COLOR environment variable is set
    const char* noColor = std::getenv("NO_COLOR");
    if (noColor != nullptr && noColor[0] != '\0') {
        g_colorEnabled = false;
        return;
    }

#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (SetConsoleMode(hOut, dwMode)) {
                supported = true;
            }
        }
    }
#else
    if (ISATTY(FILENO(stdout))) {
        const char* term = std::getenv("TERM");
        if (term != nullptr && std::string(term) != "dumb") {
            supported = true;
        }
    }
#endif

    g_colorEnabled = supported;
}

void TerminalUI::setColorEnabled(bool enabled) {
    g_colorEnabled = enabled;
}

bool TerminalUI::isColorEnabled() {
    return g_colorEnabled;
}

std::size_t TerminalUI::visibleLength(const std::string& str) {
    std::size_t len = 0;
    bool inEscape = false;
    for (std::size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\033') {
            inEscape = true;
        } else if (inEscape) {
            if (str[i] == 'm') {
                inEscape = false;
            }
        } else {
            len++;
        }
    }
    return len;
}

std::string TerminalUI::padLine(const std::string& content, std::size_t innerWidth) {
    std::size_t vLen = visibleLength(content);
    std::string result = "| " + content;
    if (vLen < innerWidth) {
        result.append(innerWidth - vLen, ' ');
    }
    result += " |";
    return result;
}

void TerminalUI::printDivider(int width, char ch) {
    std::string line = "+" + repeatChar(ch, static_cast<std::size_t>(width - 2)) + "+";
    std::cout << style(line, ANSI::Gray) << '\n';
}

void TerminalUI::printDoubleDivider(int width) {
    std::string line = "+" + repeatChar('=', static_cast<std::size_t>(width - 2)) + "+";
    std::cout << style(line, ANSI::Cyan) << '\n';
}

void TerminalUI::printHeader(const std::string& title, const std::string& subtitle) {
    constexpr std::size_t innerWidth = 72;
    printDoubleDivider(76);

    std::size_t tLen = title.size();
    std::size_t tPad = (tLen < innerWidth) ? (innerWidth - tLen) / 2 : 0;
    std::string tLine = repeatChar(' ', tPad) + style(title, ANSI::BrightWhite);
    std::cout << padLine(tLine, innerWidth) << '\n';

    if (!subtitle.empty()) {
        std::size_t sLen = subtitle.size();
        std::size_t sPad = (sLen < innerWidth) ? (innerWidth - sLen) / 2 : 0;
        std::string sLine = repeatChar(' ', sPad) + style(subtitle, ANSI::BrightCyan);
        std::cout << padLine(sLine, innerWidth) << '\n';
    }

    printDoubleDivider(76);
}

void TerminalUI::printSection(const std::string& title) {
    std::cout << '\n';
    printDivider(76, '-');
    std::string styledTitle = " " + style(title, ANSI::BrightCyan);
    std::cout << padLine(styledTitle, 72) << '\n';
    printDivider(76, '-');
}

void TerminalUI::printStatus(const std::string& label, const std::string& message, StatusType type) {
    std::string tag;
    switch (type) {
        case StatusType::Success:
            tag = style("[ SUCCESS ]", ANSI::BrightGreen);
            break;
        case StatusType::Warning:
            tag = style("[ WARNING ]", ANSI::BrightYellow);
            break;
        case StatusType::Error:
            tag = style("[ ERROR ]  ", ANSI::BrightRed);
            break;
        case StatusType::Info:
            tag = style("[ INFO ]   ", ANSI::BrightCyan);
            break;
    }

    std::cout << "  " << tag << " " << style(label, ANSI::Bold) << " " << message << '\n';
}

std::string TerminalUI::formatTimestamp(std::int64_t timestamp) {
    std::time_t t = static_cast<std::time_t>(timestamp);
    std::tm* tmInfo = std::gmtime(&t);
    std::ostringstream ss;
    if (tmInfo != nullptr) {
        ss << std::put_time(tmInfo, "%Y-%m-%d %H:%M:%S UTC");
    } else {
        ss << timestamp;
    }
    return ss.str();
}

void TerminalUI::printDashboard(std::size_t totalBlocks,
                                std::size_t pendingCount,
                                bool isChainValid,
                                std::size_t blockCapacity) {
    constexpr std::size_t innerWidth = 72;

    std::cout << '\n';
    printDoubleDivider(76);

    // Title banner
    std::string title = "C++ BLOCKCHAIN LEDGER";
    std::string subtitle = "Transaction-Based Blockchain Simulator";
    std::size_t tPad = (title.size() < innerWidth) ? (innerWidth - title.size()) / 2 : 0;
    std::cout << padLine(repeatChar(' ', tPad) + style(title, ANSI::BrightWhite), innerWidth) << '\n';

    std::size_t sPad = (subtitle.size() < innerWidth) ? (innerWidth - subtitle.size()) / 2 : 0;
    std::cout << padLine(repeatChar(' ', sPad) + style(subtitle, ANSI::BrightCyan), innerWidth) << '\n';

    printDoubleDivider(76);

    // Section 1: Real Ledger Statistics
    std::string dashHead = style("LEDGER DASHBOARD", ANSI::BrightWhite);
    std::cout << padLine(" " + dashHead, innerWidth) << '\n';

    // Row 1: Committed Blocks & Pending Transactions
    // Left half = 37 chars, Right half = 35 chars. Total = 72 chars.
    std::string bVal = std::to_string(totalBlocks);
    std::string bField = "   " + style("Committed Blocks : ", ANSI::Gray) + style(bVal, ANSI::BrightWhite);
    std::size_t bVis = visibleLength(bField);
    if (bVis < 37) {
        bField.append(37 - bVis, ' ');
    }

    std::string pVal = std::to_string(pendingCount);
    const char* pColor = (pendingCount > 0) ? ANSI::BrightYellow : ANSI::BrightWhite;
    std::string pField = style("Pending Txs      : ", ANSI::Gray) + style(pVal, pColor);
    std::size_t pVis = visibleLength(pField);
    if (pVis < 35) {
        pField.append(35 - pVis, ' ');
    }
    std::cout << padLine(bField + pField, innerWidth) << '\n';

    // Row 2: Validation Status & Block Capacity
    std::string vTag = isChainValid
        ? style("[ VALID ]", ANSI::BrightGreen)
        : style("[ COMPROMISED ]", ANSI::BrightRed);
    std::string vField = "   " + style("Chain Validation : ", ANSI::Gray) + vTag;
    std::size_t vVis = visibleLength(vField);
    if (vVis < 37) {
        vField.append(37 - vVis, ' ');
    }

    std::string capVal = std::to_string(blockCapacity) + " txs/block";
    std::string capField = style("Block Capacity   : ", ANSI::Gray) + style(capVal, ANSI::BrightWhite);
    std::size_t capVis = visibleLength(capField);
    if (capVis < 35) {
        capField.append(35 - capVis, ' ');
    }
    std::cout << padLine(vField + capField, innerWidth) << '\n';

    printDivider(76, '-');

    // Section 2: Available Operations
    std::string opsHead = style("AVAILABLE OPERATIONS", ANSI::BrightWhite);
    std::cout << padLine(" " + opsHead, innerWidth) << '\n';

    const struct {
        const char* key;
        const char* desc;
    } menuItems[] = {
        {"1", "Create a transaction"},
        {"2", "Mine pending transactions / create a block"},
        {"3", "Display the blockchain explorer"},
        {"4", "Check account balance"},
        {"5", "Validate the blockchain integrity"},
        {"6", "Demonstrate hash changes after tampering"},
        {"7", "View pending transactions pool"},
        {"0", "Exit simulator"}
    };

    for (const auto& item : menuItems) {
        std::ostringstream itemLine;
        itemLine << "   " << style("[" + std::string(item.key) + "]", ANSI::BrightCyan)
                 << " " << item.desc;
        std::cout << padLine(itemLine.str(), innerWidth) << '\n';
    }

    printDoubleDivider(76);
}

void TerminalUI::displayTransactionResult(bool accepted,
                                         const std::string& sender,
                                         const std::string& receiver,
                                         std::int64_t amount,
                                         const std::string& explanation) {
    constexpr std::size_t innerWidth = 72;

    std::cout << '\n';
    printDoubleDivider(76);

    std::string statusTag = accepted
        ? style("[ ACCEPTED ]", ANSI::BrightGreen)
        : style("[ REJECTED ]", ANSI::BrightRed);

    std::string header = "TRANSACTION RESULT: " + statusTag;
    std::cout << padLine(" " + header, innerWidth) << '\n';
    printDivider(76, '-');

    std::string sLine = "   " + style("Sender Account   : ", ANSI::Gray) + style(sender, ANSI::BrightWhite);
    std::cout << padLine(sLine, innerWidth) << '\n';

    std::string rLine = "   " + style("Receiver Account : ", ANSI::Gray) + style(receiver, ANSI::BrightWhite);
    std::cout << padLine(rLine, innerWidth) << '\n';

    std::string aLine = "   " + style("Transfer Amount  : ", ANSI::Gray) + style(std::to_string(amount) + " coins", ANSI::BrightWhite);
    std::cout << padLine(aLine, innerWidth) << '\n';

    std::string eLabel = "   " + style("Explanation      : ", ANSI::Gray);
    printWrappedField(eLabel, explanation, accepted ? ANSI::Green : ANSI::BrightRed, innerWidth);

    printDoubleDivider(76);
}

void TerminalUI::displayBlockCreationResult(bool success,
                                           std::size_t index,
                                           const std::string& prevHash,
                                           const std::string& hash,
                                           std::size_t txCount,
                                           const std::string& explanation) {
    constexpr std::size_t innerWidth = 72;

    std::cout << '\n';
    printDoubleDivider(76);

    std::string statusTag = success
        ? style("[ BLOCK COMMITTED ]", ANSI::BrightGreen)
        : style("[ CREATION FAILED ]", ANSI::BrightRed);

    std::string header = "BLOCK MINING & CREATION RESULT: " + statusTag;
    std::cout << padLine(" " + header, innerWidth) << '\n';
    printDivider(76, '-');

    if (success) {
        std::string iLine = "   " + style("Block Index      : ", ANSI::Gray) + style("#" + std::to_string(index), ANSI::BrightCyan);
        std::cout << padLine(iLine, innerWidth) << '\n';

        std::string tLine = "   " + style("Transactions     : ", ANSI::Gray) + style(std::to_string(txCount) + " transaction(s) committed", ANSI::BrightWhite);
        std::cout << padLine(tLine, innerWidth) << '\n';

        std::string pLine = "   " + style("Previous Hash    : ", ANSI::Gray) + style(prevHash, ANSI::Magenta);
        std::cout << padLine(pLine, innerWidth) << '\n';

        std::string hLine = "   " + style("Current Hash     : ", ANSI::Gray) + style(hash, ANSI::BrightMag);
        std::cout << padLine(hLine, innerWidth) << '\n';
    }

    std::string eLabel = "   " + style("Summary          : ", ANSI::Gray);
    printWrappedField(eLabel, explanation, success ? ANSI::Green : ANSI::BrightRed, innerWidth);

    printDoubleDivider(76);
}

void TerminalUI::displayBlockchain(const std::vector<Block>& chain) {
    constexpr std::size_t innerWidth = 72;

    std::cout << '\n';
    printDoubleDivider(76);
    std::string header = "COMMITTED BLOCKCHAIN EXPLORER (" + std::to_string(chain.size()) + " Blocks)";
    std::cout << padLine(" " + style(header, ANSI::BrightWhite), innerWidth) << '\n';
    printDoubleDivider(76);

    if (chain.empty()) {
        std::cout << padLine("   " + style("The blockchain is empty.", ANSI::BrightYellow), innerWidth) << '\n';
        printDoubleDivider(76);
        return;
    }

    for (std::size_t i = 0; i < chain.size(); ++i) {
        const Block& block = chain[i];
        const std::vector<Transaction>& txs = block.getTransactions();

        if (i > 0) {
            std::cout << "                                      " << style("|", ANSI::Cyan) << '\n';
            std::cout << "                          " << style("v [ Cryptographic Link Valid ] v", ANSI::BrightCyan) << '\n';
            std::cout << "                                      " << style("|", ANSI::Cyan) << '\n';
        }

        printDivider(76, '-');

        std::ostringstream blockTitle;
        blockTitle << " BLOCK #" << block.getIndex();
        if (block.getIndex() == 0) {
            blockTitle << " (Genesis Block)";
        }
        std::cout << padLine(style(blockTitle.str(), ANSI::BrightCyan), innerWidth) << '\n';
        printDivider(76, '-');

        std::ostringstream timeLine;
        timeLine << "   " << style("Timestamp     : ", ANSI::Gray)
                 << block.getTimestamp() << " (" << formatTimestamp(block.getTimestamp()) << ")";
        std::cout << padLine(timeLine.str(), innerWidth) << '\n';

        std::ostringstream prevLine;
        prevLine << "   " << style("Previous Hash : ", ANSI::Gray)
                 << style(block.getPreviousHash(), ANSI::Magenta);
        std::cout << padLine(prevLine.str(), innerWidth) << '\n';

        std::ostringstream currLine;
        currLine << "   " << style("Block Hash    : ", ANSI::Gray)
                 << style(block.getHash(), ANSI::BrightMag);
        std::cout << padLine(currLine.str(), innerWidth) << '\n';

        std::ostringstream txCountLine;
        txCountLine << "   " << style("Transactions  : ", ANSI::Gray)
                    << txs.size() << " recorded transaction(s)";
        std::cout << padLine(txCountLine.str(), innerWidth) << '\n';

        if (txs.empty()) {
            std::cout << padLine("   " + style("[No transactions included - Genesis / Empty Block]", ANSI::Gray), innerWidth) << '\n';
        } else {
            std::cout << "  " << style("+-----+------------------+------------------+------------+-----------------+", ANSI::Gray) << '\n';
            std::cout << "  " << style("|  #  | Sender           | Receiver         |     Amount | Tx ID           |", ANSI::Bold) << '\n';
            std::cout << "  " << style("+-----+------------------+------------------+------------+-----------------+", ANSI::Gray) << '\n';

            for (std::size_t t = 0; t < txs.size(); ++t) {
                const Transaction& tx = txs[t];
                std::string sSender = tx.getSender().substr(0, 16);
                std::string sReceiver = tx.getReceiver().substr(0, 16);
                std::string sTxId = tx.getTransactionId();
                if (sTxId.size() > 15) {
                    sTxId = sTxId.substr(0, 12) + "...";
                }

                std::cout << "  | "
                          << style(std::string(3 - std::min<std::size_t>(3, std::to_string(t + 1).size()), ' ') + std::to_string(t + 1), ANSI::Cyan)
                          << " | "
                          << std::left << std::setw(16) << sSender
                          << " | "
                          << std::left << std::setw(16) << sReceiver
                          << " | "
                          << std::right << std::setw(10) << tx.getAmount()
                          << " | "
                          << std::left << std::setw(15) << sTxId
                          << " |\n";
            }
            std::cout << "  " << style("+-----+------------------+------------------+------------+-----------------+", ANSI::Gray) << '\n';
        }

        printDivider(76, '-');
    }

    std::cout << '\n';
    printStatus("Summary", "Total committed blocks in chain: " + std::to_string(chain.size()), StatusType::Info);
}

void TerminalUI::displayBalance(const std::string& account,
                               std::int64_t confirmedBalance,
                               std::int64_t pendingOutgoing,
                               std::int64_t pendingIncoming) {
    constexpr std::size_t innerWidth = 72;

    std::cout << '\n';
    printDoubleDivider(76);
    std::string header = "ACCOUNT BALANCE QUERY";
    std::cout << padLine(" " + style(header, ANSI::BrightWhite), innerWidth) << '\n';
    printDivider(76, '-');

    std::string accLine = "   " + style("Account Name      : ", ANSI::Gray) + style(account, ANSI::BrightCyan);
    std::cout << padLine(accLine, innerWidth) << '\n';

    const char* cColor = (confirmedBalance > 0) ? ANSI::BrightGreen : ANSI::BrightWhite;
    std::string confLine = "   " + style("Confirmed Balance : ", ANSI::Gray) + style(std::to_string(confirmedBalance) + " coins", cColor);
    std::cout << padLine(confLine, innerWidth) << '\n';

    if (pendingOutgoing > 0 || pendingIncoming > 0) {
        std::string outLine = "   " + style("Pending Outgoing  : ", ANSI::Gray) + style("-" + std::to_string(pendingOutgoing) + " coins", ANSI::BrightYellow);
        std::cout << padLine(outLine, innerWidth) << '\n';

        std::string inLine = "   " + style("Pending Incoming  : ", ANSI::Gray) + style("+" + std::to_string(pendingIncoming) + " coins", ANSI::Cyan);
        std::cout << padLine(inLine, innerWidth) << '\n';

        std::int64_t spendable = confirmedBalance - pendingOutgoing;
        const char* sColor = (spendable > 0) ? ANSI::BrightGreen : ANSI::BrightYellow;
        std::string spLine = "   " + style("Spendable Balance : ", ANSI::Gray) + style(std::to_string(spendable) + " coins", sColor);
        std::cout << padLine(spLine, innerWidth) << '\n';
    }

    printDoubleDivider(76);
}

void TerminalUI::displayValidationResult(bool isValid,
                                        std::size_t blockCount,
                                        bool genesisValid,
                                        bool hashesValid,
                                        bool linksValid,
                                        bool indexesValid) {
    constexpr std::size_t innerWidth = 72;

    std::cout << '\n';
    printDoubleDivider(76);
    std::string statusTag = isValid
        ? style("[ LEDGER INTEGRITY VALID ]", ANSI::BrightGreen)
        : style("[ INTEGRITY COMPROMISED ]", ANSI::BrightRed);

    std::string header = "BLOCKCHAIN VALIDATION AUDIT: " + statusTag;
    std::cout << padLine(" " + header, innerWidth) << '\n';
    printDivider(76, '-');

    std::cout << padLine(" " + style("STRUCTURAL & CRYPTOGRAPHIC VERIFICATION:", ANSI::BrightWhite), innerWidth) << '\n';

    auto printCheck = [&](const std::string& name, bool pass) {
        std::string mark = pass ? style("[PASS] ", ANSI::BrightGreen) : style("[FAIL] ", ANSI::BrightRed);
        std::cout << padLine("   " + mark + name, innerWidth) << '\n';
    };

    printCheck("Non-empty chain (Committed blocks: " + std::to_string(blockCount) + ")", blockCount > 0);
    printCheck("Genesis block: index 0 with predecessor hash '0'", genesisValid);
    printCheck("Index sequence: consecutive numbering without gaps", indexesValid);
    printCheck("Hash integrity: stored hash matches computed digest", hashesValid);
    printCheck("Link integrity: previousHash matches parent hash", linksValid);

    printDivider(76, '-');
    std::cout << padLine(" " + style("AUDIT SCOPE & METHODOLOGY NOTE:", ANSI::BrightWhite), innerWidth) << '\n';
    std::cout << padLine("   isChainValid() verifies cryptographic block hashes and links.", innerWidth) << '\n';
    std::cout << padLine("   It detects corrupted blocks, data tampering, and broken links.", innerWidth) << '\n';
    std::cout << padLine("   Transaction replaying and signatures are not part of scope.", innerWidth) << '\n';
    printDoubleDivider(76);
}

void TerminalUI::displayTamperingDemo(const Block& originalBlock,
                                     const std::string& originalStoredHash,
                                     const Block& alteredBlock,
                                     const std::string& alteredHash,
                                     bool chainStillValid) {
    constexpr std::size_t innerWidth = 72;

    std::cout << '\n';
    printDoubleDivider(76);
    std::string header = "CONTROLLED BLOCKCHAIN TAMPERING DEMONSTRATION";
    std::cout << padLine(" " + style(header, ANSI::BrightYellow), innerWidth) << '\n';
    printDoubleDivider(76);

    // Section 1: Original Committed Block
    std::cout << padLine(" " + style("1. ORIGINAL COMMITTED BLOCK IN MEMORY", ANSI::BrightWhite), innerWidth) << '\n';
    std::string oIdx = "   " + style("Block Index      : ", ANSI::Gray) + "#" + std::to_string(originalBlock.getIndex());
    std::cout << padLine(oIdx, innerWidth) << '\n';

    std::string oTx = "   " + style("Original Txs     : ", ANSI::Gray) + std::to_string(originalBlock.getTransactions().size()) + " transaction(s)";
    std::cout << padLine(oTx, innerWidth) << '\n';

    std::string oHash = "   " + style("Stored Hash      : ", ANSI::Gray) + style(originalStoredHash, ANSI::BrightGreen);
    std::cout << padLine(oHash, innerWidth) << '\n';

    printDivider(76, '-');

    // Section 2: Modified Copy
    std::cout << padLine(" " + style("2. MODIFIED LOCAL COPY (TAMPER ATTEMPT)", ANSI::BrightWhite), innerWidth) << '\n';
    std::string aTx = "   " + style("Modified Txs     : ", ANSI::Gray) + std::to_string(alteredBlock.getTransactions().size()) + style(" (Injected unauthorized transfer)", ANSI::BrightRed);
    std::cout << padLine(aTx, innerWidth) << '\n';

    std::string aHash = "   " + style("Recalculated Hash: ", ANSI::Gray) + style(alteredHash, ANSI::BrightRed);
    std::cout << padLine(aHash, innerWidth) << '\n';

    printDivider(76, '-');

    // Section 3: Hash Comparison
    std::cout << padLine(" " + style("3. AVALANCHE EFFECT & HASH MISMATCH ANALYSIS", ANSI::BrightWhite), innerWidth) << '\n';
    if (originalStoredHash != alteredHash) {
        std::cout << padLine("   " + style("[RESULT] Cryptographic hash mismatch confirmed!", ANSI::BrightRed), innerWidth) << '\n';
        std::cout << padLine("   Any unauthorized byte alteration completely scrambles block hash.", innerWidth) << '\n';
        std::cout << padLine("   Nodes recalculating block hash will reject the tampered copy.", innerWidth) << '\n';
    } else {
        std::cout << padLine("   " + style("[UNEXPECTED] Hashes matched unexpectedly.", ANSI::BrightRed), innerWidth) << '\n';
    }

    printDivider(76, '-');

    // Section 4: Live Chain Verification
    std::cout << padLine(" " + style("4. LIVE CHAIN STATE IN MEMORY", ANSI::BrightWhite), innerWidth) << '\n';
    if (chainStillValid) {
        std::cout << padLine("   " + style("[PASS] The actual committed blockchain remains 100% VALID.", ANSI::BrightGreen), innerWidth) << '\n';
        std::cout << padLine("   The tampering was performed on an isolated block copy.", innerWidth) << '\n';
        std::cout << padLine("   The committed chain vector was untouched and integrity holds.", innerWidth) << '\n';
    } else {
        std::cout << padLine("   " + style("[FAIL] The live blockchain was compromised.", ANSI::BrightRed), innerWidth) << '\n';
    }

    printDoubleDivider(76);
}

void TerminalUI::displayPendingTransactions(const std::vector<Transaction>& pending,
                                           std::size_t capacity) {
    constexpr std::size_t innerWidth = 72;

    std::cout << '\n';
    printDoubleDivider(76);
    std::ostringstream headStream;
    headStream << "PENDING TRANSACTIONS POOL (" << pending.size() << " / " << capacity << " Capacity)";
    std::cout << padLine(" " + style(headStream.str(), ANSI::BrightWhite), innerWidth) << '\n';
    printDoubleDivider(76);

    if (pending.empty()) {
        std::cout << padLine("   " + style("The pending transaction pool is currently empty.", ANSI::BrightYellow), innerWidth) << '\n';
        std::cout << padLine("   Use menu option [1] to submit transactions before mining a block.", innerWidth) << '\n';
        printDoubleDivider(76);
        return;
    }

    std::cout << "  " << style("+-----+------------------+------------------+------------+-----------------+", ANSI::Gray) << '\n';
    std::cout << "  " << style("|  #  | Sender           | Receiver         |     Amount | Tx ID           |", ANSI::Bold) << '\n';
    std::cout << "  " << style("+-----+------------------+------------------+------------+-----------------+", ANSI::Gray) << '\n';

    for (std::size_t i = 0; i < pending.size(); ++i) {
        const Transaction& tx = pending[i];
        std::string sSender = tx.getSender().substr(0, 16);
        std::string sReceiver = tx.getReceiver().substr(0, 16);
        std::string sTxId = tx.getTransactionId();
        if (sTxId.size() > 15) {
            sTxId = sTxId.substr(0, 12) + "...";
        }

        std::cout << "  | "
                  << style(std::string(3 - std::min<std::size_t>(3, std::to_string(i + 1).size()), ' ') + std::to_string(i + 1), ANSI::Cyan)
                  << " | "
                  << std::left << std::setw(16) << sSender
                  << " | "
                  << std::left << std::setw(16) << sReceiver
                  << " | "
                  << std::right << std::setw(10) << tx.getAmount()
                  << " | "
                  << std::left << std::setw(15) << sTxId
                  << " |\n";
    }

    std::cout << "  " << style("+-----+------------------+------------------+------------+-----------------+", ANSI::Gray) << '\n';
    printDivider(76, '-');

    std::string summaryLine = "   " + style("Total Pending  : ", ANSI::Gray) + std::to_string(pending.size()) + " transaction(s) ready to mine.";
    std::cout << padLine(summaryLine, innerWidth) << '\n';

    std::string capLine = "   " + style("Slots Remaining: ", ANSI::Gray) + std::to_string(capacity > pending.size() ? capacity - pending.size() : 0) + " available.";
    std::cout << padLine(capLine, innerWidth) << '\n';

    printDoubleDivider(76);
}

void TerminalUI::displayErrorMessage(const std::string& message) {
    std::cout << "  " << style("[ ERROR ] ", ANSI::BrightRed) << message << '\n';
}

void TerminalUI::displaySuccessMessage(const std::string& message) {
    std::cout << "  " << style("[ SUCCESS ] ", ANSI::BrightGreen) << message << '\n';
}

void TerminalUI::displayWarningMessage(const std::string& message) {
    std::cout << "  " << style("[ WARNING ] ", ANSI::BrightYellow) << message << '\n';
}

void TerminalUI::displayInfoMessage(const std::string& message) {
    std::cout << "  " << style("[ INFO ] ", ANSI::BrightCyan) << message << '\n';
}

void TerminalUI::displayExitBanner() {
    std::cout << '\n';
    printDoubleDivider(76);
    std::cout << padLine(" " + style("Shutting down C++ Blockchain Simulator. Goodbye!", ANSI::BrightCyan), 72) << '\n';
    printDoubleDivider(76);
    std::cout << '\n';
}

void TerminalUI::promptContinue() {
    std::cout << '\n' << style("  Press [Enter] to return to the main menu...", ANSI::Gray);
    std::string line;
    std::getline(std::cin, line);
}
