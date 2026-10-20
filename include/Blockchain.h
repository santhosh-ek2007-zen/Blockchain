#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <bits/stdc++.h>
#include "Block.h"
#include "Transaction.h"
using namespace std;

class Blockchain{
    // Data Members
    private:

    //1.Stores the ordered Blocks
    vector <Block> chain;

    //2.Stores the Transactions that are pending for commitment
    vector <Transaction> pendingTransactions;
    
    //3.Tracks account Balances 
    map<string,int64_t> balances; //int64_t is used to avoid floating-point rounding errors
    
    //4.Designs the maximum Transactions per block 
    size_t blockSize;

    //Member functions
    public:

    //1.Constructor
    Blockchain();

    //2.Transaction management
    bool addTransaction(const Transaction& transaction);
    bool isTransactionValid(const Transaction& transaction)const;

    // 3. Block creation
    bool minePendingTransactions();

    // 4. Chain validation
    bool isChainValid() const;

    // 5. Read-only queries
    std::int64_t getBalance(const std::string& address) const;
    const Block& getLatestBlock() const;
    const vector<Block>& getChain() const;
    const vector<Transaction>& getPendingTransactions() const;


};

#endif