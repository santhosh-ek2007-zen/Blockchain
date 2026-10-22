#include <iostream>
#include <vector>
#include "../include/Transaction.h"
#include "../include/Block.h"

int main() {
    std::cout << "--- Testing the Block Class ---\n\n";

    // 1. Create a couple of dummy transactions
    Transaction tx1("Alice", "Bob", 50);
    Transaction tx2("Bob", "Charlie", 15);

    // 2. Add them to a pending transactions list
    std::vector<Transaction> pendingTxs;
    pendingTxs.push_back(tx1);
    pendingTxs.push_back(tx2);

    // 3. Create your Block (Index 1, with the transactions, and a fake previous hash)
    std::string previousHash = "00000000000000000000000000000000";
    Block myBlock(1, pendingTxs, previousHash);

    // 4. Print the formatted block to the console
    std::cout << myBlock.toString() << std::endl;

    return 0;
}