#include "../include/Transaction.h"
#include<ctime>
#include<random>
#include<sstream>
using namespace std;

Transaction::Transaction(const string &sender, const string &receiver, int64_t amount): sender(sender), receiver(receiver), amount(amount)
{
    //Converting Unix time to string
    timestamp=to_string(time(nullptr));

    //Random number generator for the transactionId
    random_device rd;  //random seed from hardware
    mt19937_64 gen(rd());  //64 bit Mersenne Twister engine
    uniform_int_distribution<uint64_t> dis;  //Distribution

    stringstream ss;
    //Combine "tx_", the timestamp, and a random hex number to create the ID
    ss<<"tx_"<<timestamp<<"_"<<hex<<dis(gen);
    transactionId=ss.str();
}