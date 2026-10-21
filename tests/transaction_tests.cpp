#include<iostream>
#include<cassert>
#include "../include/Transaction.h"

using namespace std;

int main() 
{
    cout << "Running Transaction Tests..." << endl;

    // Test 1: Valid transaction and accessors
    Transaction t1("Alice", "Bob", 150);
    assert(t1.isValid() == true);
    assert(t1.getSender() == "Alice");
    assert(t1.getReceiver() == "Bob");
    assert(t1.getAmount() == 150);

    // Test 2: Zero or negative amount rejection
    Transaction t2("Alice", "Bob", 0);
    assert(t2.isValid() == false);
    
    Transaction t3("Alice", "Bob", -50);
    assert(t3.isValid() == false);

    // Test 3: Empty sender or receiver rejection
    Transaction t4("", "Bob", 100);
    assert(t4.isValid() == false);

    Transaction t5("Alice", "", 100);
    assert(t5.isValid() == false);

    // Test 4: Self-transfer rejection
    Transaction t6("Alice", "Alice", 100);
    assert(t6.isValid() == false);

    // Test 5: Serialization consistency
    Transaction t7("Charlie", "Dave", 200);
    string serialized1 = t7.toString();
    string serialized2 = t7.toString();
    assert(serialized1 == serialized2);
    
    // Test 6: Transaction ID uniqueness
    Transaction t8("Eve", "Frank", 50);
    assert(t7.getTransactionId() != t8.getTransactionId());

    cout << "All Transaction tests passed successfully!" << endl;
    return 0;
}