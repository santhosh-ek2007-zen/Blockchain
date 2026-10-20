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
    random_device rd;  //getting a random seed from the hardware
    mt19937_64 gen(rd());  //64 bit Mersenne Twister engine
    uniform_int_distribution<uint64_t> dis;  //for even distribution of the generated numbers across the 64 bit range

    stringstream ss;
    //Combine "tx_", the timestamp, and a random hex number to create the ID
    ss<<"tx_"<<timestamp<<"_"<<hex<<dis(gen);
    transactionId=ss.str();
}

//Transaction validity
bool Transaction::isValid() const
{
    if(amount<=0)
    {
        return false;
    }

    if(sender.empty() || receiver.empty())
    {
        return false;
    }

    if(sender==receiver)
    {
        return false;
    }

    return true;
}


string Transaction::getSender() const 
{
    return sender;
}

string Transaction::getReceiver() const 
{
    return receiver;
}

int64_t Transaction::getAmount() const 
{
    return amount;
}

string Transaction::getTimestamp() const 
{
    return timestamp;
}

string Transaction::getTransactionId() const 
{
    return transactionId;
}

//method for formatting the transaction details in a particular format which helps for hashing
string Transaction::toString() const
{
    stringstream ss;

    ss<<transactionId<<":"<<sender<<":"<<receiver<<":"<<amount<<":"<<timestamp;

    return ss.str();
}