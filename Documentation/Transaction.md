# Transaction Class Documentation

## 1. Overview

`Transaction` represents one transfer between two accounts in the C++
transaction-based blockchain project. It stores the transfer's details,
supports basic field validation, provides access to its data, and can
produce a consistent representation for display or hashing.

A transaction describes a transfer; it does not manage the blockchain or
directly update account balances.

**Header:** `include/Transaction.h`\
**Implementation:** `src/Transaction.cpp`\
**Related components:** `Block`, `Blockchain`

> This documentation follows the project's architecture specification.
> Match the names and exact signatures to the implementation in your
> repository if they differ.

------------------------------------------------------------------------

## 2. Design Summary

The `Transaction` class models a single transfer and keeps its data
together in one object.

Its main responsibilities are to:

-   Store the sender and receiver account identifiers.
-   Store the amount being transferred.
-   Record the transaction's timestamp.
-   Store a transaction identifier.
-   Check basic transaction fields.
-   Provide controlled access to transaction data.
-   Produce a stable representation for display or hashing.

It should remain independent of the blockchain's authoritative balance
ledger. `Blockchain` is responsible for checking available funds and
applying accepted transactions consistently.

------------------------------------------------------------------------

## 3. Data Members

The project architecture specifies the following data members. Their
exact declarations and types should be checked against `Transaction.h`.

  -----------------------------------------------------------------------
  Data member             Suggested type          Responsibility
  ----------------------- ----------------------- -----------------------
  `sender`                `std::string`           Stores the account
                                                  initiating the
                                                  transfer.

  `receiver`              `std::string`           Stores the account
                                                  receiving the transfer.

  `amount`                `std::int64_t`          Stores the transfer
                                                  amount in the smallest
                                                  chosen unit.

  `timestamp`             Integer time value or   Records when the
                          string                  transaction was
                                                  created.

  `transactionId`         `std::string`           Stores the
                                                  transaction's
                                                  identifier.
  -----------------------------------------------------------------------

### 3.1 `sender`

Identifies the account from which the transfer is initiated. Basic
validation should reject an empty sender value.

### 3.2 `receiver`

Identifies the account that receives the transfer. Basic validation
should reject an empty receiver value.

### 3.3 `amount`

Stores how much is being transferred. The architecture recommends an
integer type such as `std::int64_t` to avoid floating-point rounding
errors when representing money-like values.

Basic validation should reject zero or negative amounts. Checking
whether the sender has enough available funds is a `Blockchain`
responsibility, not a `Transaction` responsibility.

### 3.4 `timestamp`

Records the time associated with the transaction. Choose a consistent
representation and serialization format so the same transaction state
can be represented deterministically.

### 3.5 `transactionId`

Identifies the transaction within the application's chosen identifier
strategy. The implementation should define how identifiers are generated
and how uniqueness is handled.

A transaction ID is an identifier; it is **not proof** that a
transaction is authorized, genuine, or cryptographically signed.

### Encapsulation

Keep the data members private and expose controlled operations through
the public interface. This helps protect the transaction's state from
uncontrolled external changes.

------------------------------------------------------------------------

## 4. Member Functions

The architecture specification identifies the following operations. Use
the exact declarations from your current `Transaction.h`.

### 4.1 Constructor

**Responsibility:** Initialize the transaction's fields and associated
metadata.

The constructor receives or establishes the sender, receiver, amount,
timestamp, and transaction identifier according to the implementation's
design. Ensure that the initialization behavior is consistent and that
invalid values can be detected by the validation function or constructor
policy.

### 4.2 `isValid()`

**Responsibility:** Check the transaction's basic fields.

The project specification recommends checking that:

1.  The sender is not empty.
2.  The receiver is not empty.
3.  The amount is positive.
4.  The sender and receiver are different if self-transfers are
    prohibited by the project rules.

This function checks basic transaction validity. It should not
independently update balances or replace the blockchain's
available-funds checks.

### 4.3 `getSender()`

**Responsibility:** Return the sender's account identifier.

It lets other classes inspect the sender without directly accessing the
private data member.

### 4.4 `getReceiver()`

**Responsibility:** Return the receiver's account identifier.

It provides controlled access to the destination account.

### 4.5 `getAmount()`

**Responsibility:** Return the transfer amount.

`Blockchain` can use the amount when checking available funds and
applying the transfer to the ledger.

### 4.6 `getTimestamp()`

**Responsibility:** Return the transaction's creation timestamp.

This provides access to transaction metadata.

### 4.7 `getTransactionId()`

**Responsibility:** Return the transaction identifier.

This allows the application and tests to identify or inspect a
transaction according to the project's identifier policy.

### 4.8 `toString()` / `serialize()`

**Responsibility:** Produce a consistent representation of the
transaction.

The architecture allows a display-oriented `toString()` or a
serialization function. The representation may be used for display,
debugging, or as part of a block's hash input.

If the representation is used for hashing, it must be deterministic and
unambiguous. For example, field boundaries must be clear so different
field combinations cannot accidentally produce the same serialized text.

### Constructors, destructors, and other helpers

Check `Transaction.h` for the exact constructors, destructor, helper
methods, and signatures in your implementation. The architecture
specification does not establish any additional methods beyond the
operations described above.

------------------------------------------------------------------------

## 5. Responsibility of Data Members and Member Functions

  -----------------------------------------------------------------------
  Element                             Responsibility
  ----------------------------------- -----------------------------------
  `sender`                            Identify the account initiating the
                                      transfer.

  `receiver`                          Identify the destination account.

  `amount`                            Store the transfer amount using an
                                      appropriate numeric representation.

  `timestamp`                         Record when the transaction was
                                      created.

  `transactionId`                     Identify the transaction according
                                      to the defined ID strategy.

  Constructor                         Initialize transaction details and
                                      metadata.

  `isValid()`                         Check basic fields and
                                      project-defined self-transfer
                                      rules.

  `getSender()`                       Provide read access to the sender.

  `getReceiver()`                     Provide read access to the
                                      receiver.

  `getAmount()`                       Provide read access to the amount.

  `getTimestamp()`                    Provide read access to the
                                      timestamp.

  `getTransactionId()`                Provide read access to the
                                      transaction ID.

  `toString()` / `serialize()`        Produce a consistent representation
                                      for display or hashing.
  -----------------------------------------------------------------------

------------------------------------------------------------------------

## 6. How `Transaction` Works With Other Classes

### With `Block`

A block contains an ordered collection of transactions. The transaction
provides the transfer details that become part of the block's contents.

### With `Blockchain`

`Blockchain` checks whether the transfer can be accepted under the
ledger rules, including available funds and pending transactions. After
a successful commit, the blockchain updates balances consistently.

### With hashing

The transaction's stable representation can be included in the block's
canonical hash input. A consistent field order and unambiguous
representation are important so the same transaction data produces the
same serialized bytes.

### Conceptual flow

1.  A transaction is created with transfer details.
2.  Basic field validation is performed.
3.  `Blockchain` checks available funds and pending-fund rules.
4.  If accepted, the transaction enters the pending transaction pool.
5.  The transaction is included in a block.
6.  The block's contents are serialized and hashed.
7.  On a successful block commit, `Blockchain` updates balances and
    removes committed transactions from the pending pool consistently.

------------------------------------------------------------------------

## 7. Design Rules and Important Notes

-   **Keep the class focused:** a transaction represents a transfer; it
    does not own the chain or account ledger.
-   **Validate basic fields:** reject empty account names and
    nonpositive amounts.
-   **Define self-transfer behavior:** reject self-transfers if that is
    the project's chosen rule.
-   **Use integer amounts:** the architecture recommends integer units
    such as `std::int64_t` for money-like values.
-   **Keep serialization deterministic:** the same transaction state
    should produce the same representation.
-   **Define ID generation:** establish an identifier strategy and
    consider collision handling.
-   **Do not confuse an ID with authorization:** an identifier does not
    prove that a transfer is genuine or approved.
-   **Prevent overspending elsewhere:** the blockchain must account for
    available funds and already accepted pending transactions.
-   **Avoid duplicate application:** `Blockchain` must ensure each
    committed transfer is applied only once.

------------------------------------------------------------------------

## 8. Testing Responsibilities

Test the class independently of the blockchain where possible.

  -----------------------------------------------------------------------
  Test                                Expected result
  ----------------------------------- -----------------------------------
  Valid transaction                   Basic validation succeeds.

  Empty sender                        Basic validation fails.

  Empty receiver                      Basic validation fails.

  Zero amount                         Basic validation fails.

  Negative amount                     Basic validation fails.

  Self-transfer                       Rejected if the project prohibits
                                      it.

  Accessors                           Return the initialized field
                                      values.

  Transaction ID                      Follows the implementation's
                                      identifier strategy.

  Stable serialization                Unchanged transaction state
                                      produces the same representation.

  Changed transaction data            The serialized representation
                                      reflects the changed data.
  -----------------------------------------------------------------------

Available-funds and pending-overspending tests belong primarily to
`Blockchain` tests because the transaction object does not own the
authoritative balance ledger.

------------------------------------------------------------------------

## 9. Summary

`Transaction` models one transfer between a sender and a receiver. It
stores the account identifiers, amount, timestamp, and transaction ID;
checks basic field validity; provides access to its data; and produces a
consistent representation. It does not manage account balances or the
blockchain. Those responsibilities belong to `Blockchain`, while `Block`
groups transactions and `HashUtil` supplies cryptographic hashing.
