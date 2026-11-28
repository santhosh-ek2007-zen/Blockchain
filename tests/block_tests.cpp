#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "Block.h"
#include "HashUtil.h"
#include "Transaction.h"

int main() {
    std::cout << "--- Running Block & SHA-256 Tests ---\n\n";

    // 1. Known SHA-256 test vector ("abc")
    HashUtil hasher;
    std::string digest;
    std::string errorMessage;
    assert(hasher.sha256("abc", digest, errorMessage));
    assert(digest == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");

    // 2. Genesis / empty block
    const std::string zeroHash(64, '0');
    Block genesis(0, {}, zeroHash);
    assert(genesis.getIndex() == 0);
    assert(genesis.getPreviousHash() == zeroHash);
    assert(genesis.getTransactions().empty());
    assert(genesis.getHash().size() == 64);
    assert(genesis.getHash() == genesis.calculateHash());

    // 3. Block with transactions (canonical SHA-256 hash)
    Transaction tx1("Alice", "Bob", 50);
    Transaction tx2("Bob", "Charlie", 15);
    Block b1(1, {tx1, tx2}, genesis.getHash());
    assert(b1.getHash().size() == 64);
    assert(b1.getHash() == b1.calculateHash());

    // 4. Transaction ordering changes the hash
    Block bReordered(1, {tx2, tx1}, genesis.getHash());
    assert(b1.getHash() != bReordered.getHash());

    // 5. Previous hash changes the hash
    Block bDifferentPrev(1, {tx1, tx2}, std::string(64, 'f'));
    assert(b1.getHash() != bDifferentPrev.getHash());

    // 6. Modifying contents via addTransaction() updates the hash
    std::string hashBefore = b1.getHash();
    Transaction tx3("Charlie", "Dave", 20);
    b1.addTransaction(tx3);
    assert(b1.getTransactions().size() == 3);
    assert(b1.getHash() != hashBefore);
    assert(b1.getHash() == b1.calculateHash());

    // 7. Invalid transaction is ignored and does not alter hash
    std::string hashAfterValid = b1.getHash();
    Transaction invalidTx("Alice", "Alice", 0);
    b1.addTransaction(invalidTx);
    assert(b1.getTransactions().size() == 3);
    assert(b1.getHash() == hashAfterValid);

    std::cout << b1.toString() << "\n";
    std::cout << "All Block & SHA-256 tests passed!\n";
    return 0;
}