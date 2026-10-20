#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <cstdint>

class Transaction
{
    private:
    std::string sender; //account sending the funds
    std::string receiver; //account getting the funds
    std::int64_t amount; //transfer value
    std::string timestamp; //time when the transfer was created
    std::string transactionId; //unique identifier for this specific transaction

    public:

    //Constructor
    Transaction(const std::string &sender, const std::string &receiver, std::int64_t amount);

    //Checking whether the transaction is valid or not
    bool isValid() const;
    
    //Read-Only Accessors
    std::string getSender() const;
    std::string getReceiver() const;
    std::int64_t getAmount() const;
    std::string getTimestamp() const;
    std::string getTransactionId() const;

    //formats the transaction into a consistent string representation so it can be displayed on the console or hashed later
    std::string toString() const;
};

#endif