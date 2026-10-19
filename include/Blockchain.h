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
    

};