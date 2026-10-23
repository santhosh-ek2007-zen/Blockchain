# Block Class Documentation

## 1. Overview

`Block` represents one block in the transaction-based blockchain. A
block groups an ordered collection of transactions and stores metadata
that connects it to the blockchain, including its index, timestamp,
previous block hash, and its own calculated hash.

The class is responsible for representing a block and calculating its
hash. It does not manage the entire blockchain or account balances.

**Header:** `include/Block.h`\
**Implementation:** `src/Block.cpp`\
**Related components:** `Transaction`, `HashUtil`

> This documentation follows the project's architecture specification.
> Match function names and exact signatures to the implementation in
> your repository if they differ.

------------------------------------------------------------------------

## 2. Design Summary

The `Block` class is a data-and-behavior unit for one block in the
chain.

Its main responsibilities are to:

-   Store block metadata.
-   Store transactions in a defined order.
-   Store the hash of the previous block.
-   Calculate a hash from a consistent representation of the block's
    contents.
-   Provide controlled access to block information.
-   Format block details for display when required.

The block uses `HashUtil` for SHA-256 hashing rather than implementing
cryptographic hashing itself.

### Place in the architecture

-   `Transaction` describes an individual transfer.
-   `Block` groups transactions and carries the information needed to
    link it to the previous block.
-   `Blockchain` manages the ordered collection of committed blocks,
    pending transactions, balances, and validation.
-   `HashUtil` calculates SHA-256 digests.

------------------------------------------------------------------------

## 3. Data Members

The project architecture specifies the following data members for
`Block`. The exact types should match the declarations in `Block.h`.

  ----------------------------------------------------------------------------
  Data member             Suggested type               Responsibility
  ----------------------- ---------------------------- -----------------------
  `index`                 `std::size_t`                Stores the block's
                                                       position in the chain.

  `timestamp`             Integer time value           Records when the block
                                                       was created.

  `transactions`          `std::vector<Transaction>`   Stores the ordered
                                                       transactions included
                                                       in this block.

  `previousHash`          `std::string`                Stores the hash
                                                       reference to the
                                                       preceding block.

  `hash`                  `std::string`                Stores the calculated
                                                       hash of this block.
  ----------------------------------------------------------------------------

### 3.1 `index`

Identifies the block's position in the chain. The genesis block normally
occupies the initial position, and later blocks follow it in order.

### 3.2 `timestamp`

Records the block's creation time. The chosen representation should be
consistent throughout the project.

### 3.3 `transactions`

Contains the transfers included in the block. Transaction order matters:
changing the order changes the serialized input and should therefore
change the resulting digest with overwhelming probability.

### 3.4 `previousHash`

Links the block to its predecessor by storing the predecessor's hash.
For a non-genesis block, this value should match the hash of the
preceding block.

### 3.5 `hash`

Stores the digest calculated from the block's canonical contents. The
hash input should include the index, timestamp, previous hash, and
ordered transaction data. It must **not** include the block's own stored
`hash`, because doing so would make the hash input depend on the value
being calculated.

### Encapsulation

Keep the block's state private and expose controlled public operations.
Normal APIs should not allow unrestricted mutation of committed blocks.
If tests need to simulate tampering, use a test-only approach rather
than exposing unrestricted mutation in the normal interface.

------------------------------------------------------------------------

## 4. Member Functions

The architecture specification identifies the following operations. Use
the signatures declared in your actual `Block.h`.

### 4.1 Constructor

**Typical responsibility:** Initialize the block's index, timestamp,
transaction collection, and previous hash.

The constructor establishes the block's initial state. Ensure that the
initial values are consistent with the project's rules for genesis and
ordinary blocks.

### 4.2 `addTransaction()`

**Responsibility:** Add a transaction while the block is being
assembled.

This operation modifies the block's transaction collection. The
project's design recommends defining an empty-block policy; for the MVP,
empty blocks should be rejected when committing them.

Once a block is committed, normal application code should treat its
contents as immutable.

### 4.3 `calculateHash()`

**Responsibility:** Calculate the block's SHA-256 hash from its
canonical representation.

The function should:

1.  Represent the relevant block fields in a fixed order.
2.  Serialize the transactions deterministically and unambiguously.
3.  Exclude the block's own stored `hash` from the hash input.
4.  Pass the resulting input to `HashUtil`.
5.  Return or store the calculated digest according to the actual
    function signature.

A block hash is only reliable for comparison when the same logical data
produces the same serialized input.

### 4.4 `getHash()`

**Responsibility:** Return the block's stored hash.

It allows `Blockchain` or tests to inspect the hash without directly
modifying the private member.

### 4.5 `getPreviousHash()`

**Responsibility:** Return the hash reference to the preceding block.

`Blockchain` can use this value to check whether the chain's links are
consistent.

### 4.6 `getTransactions()`

**Responsibility:** Provide access to the transactions contained in the
block.

Prefer read-only access for committed blocks so callers cannot silently
alter their transaction contents.

### 4.7 `getIndex()`

**Responsibility:** Return the block's index.

This lets callers inspect the block's position without accessing private
data directly.

### 4.8 `getTimestamp()`

**Responsibility:** Return the block's creation timestamp.

It provides access to metadata without exposing unrestricted mutation.

### 4.9 `toString()`

**Responsibility:** Produce a readable representation of the block and
its transaction details.

This is useful for console output and debugging. Formatting for display
should remain separate from the canonical serialization used for
hashing; human-readable formatting should not accidentally become an
unstable hash format.

### Constructors, destructors, and other helpers

Consult `Block.h` for the exact constructors, destructors, overloads,
and helper functions in your current implementation. The architecture
specification does not establish additional signatures beyond the
operations described above.

------------------------------------------------------------------------

## 5. Responsibility of Data Members and Member Functions

  -----------------------------------------------------------------------
  Element                             Responsibility
  ----------------------------------- -----------------------------------
  `index`                             Identify the block's position.

  `timestamp`                         Record block creation time.

  `transactions`                      Hold the ordered transfers in the
                                      block.

  `previousHash`                      Reference the preceding block's
                                      hash.

  `hash`                              Store the digest of the block's
                                      canonical contents.

  Constructor                         Initialize the block's metadata and
                                      transaction collection.

  `addTransaction()`                  Add a transfer during block
                                      assembly.

  `calculateHash()`                   Produce the SHA-256 digest for the
                                      block's canonical contents.

  `getHash()`                         Expose the stored hash for
                                      inspection.

  `getPreviousHash()`                 Expose the predecessor-hash
                                      reference.

  `getTransactions()`                 Provide access to the included
                                      transactions.

  `getIndex()`                        Expose the block index.

  `getTimestamp()`                    Expose the block timestamp.

  `toString()`                        Format block details for display.
  -----------------------------------------------------------------------

------------------------------------------------------------------------

## 6. How `Block` Works With Other Classes

### With `Transaction`

A block stores multiple `Transaction` objects. Each transaction
describes a transfer; the block groups those transfers together in
order.

### With `HashUtil`

The block prepares a deterministic representation of its fields and
transactions, then uses `HashUtil` to calculate a SHA-256 digest.

### With `Blockchain`

`Blockchain` creates and commits blocks, sets the previous-hash
relationship, and validates the ordered chain. `Block` should not manage
account balances or the entire chain itself.

### Conceptual flow

1.  The blockchain selects transactions for a new block.
2.  A block is initialized with its metadata and previous hash.
3.  Transactions are added in a defined order.
4.  The block's canonical contents are serialized.
5.  `HashUtil` calculates the SHA-256 digest.
6.  The blockchain checks the commit conditions and adds the block to
    the chain.

------------------------------------------------------------------------

## 7. Design Rules and Important Notes

-   **Deterministic serialization:** Identical block state must produce
    identical hash input.
-   **Fixed field order:** Include the index, timestamp, previous hash,
    and ordered transaction data.
-   **Exclude the stored hash:** Do not hash the block's own `hash`
    field.
-   **Preserve transaction order:** Reordering transactions changes the
    serialized input.
-   **Keep responsibilities separate:** Do not place balance management
    or full-chain management inside `Block`.
-   **Protect committed state:** Avoid unrestricted changes to a block
    after it has been committed.
-   **Define the empty-block policy:** The project MVP recommends
    rejecting empty blocks.
-   **Do not overstate security:** Hash links help detect changes, but
    an attacker who can rewrite the whole chain and recalculate its
    hashes may create a replacement chain. This educational MVP does not
    include distributed consensus or digital signatures.

------------------------------------------------------------------------

## 8. Testing Responsibilities

Test the class through its public interface where possible.

  -----------------------------------------------------------------------
  Test                                Expected result
  ----------------------------------- -----------------------------------
  Construction                        Metadata and initial transaction
                                      state are correct.

  Add transaction                     The transaction collection contains
                                      the added transaction.

  Transaction access                  The returned transaction data
                                      matches the block's contents.

  Hash repeatability                  Unchanged block contents produce
                                      the same hash.

  Content modification                Altered hashed contents produce a
                                      different digest with overwhelming
                                      probability.

  Previous-hash value                 The stored predecessor reference
                                      can be retrieved correctly.

  Metadata accessors                  Index and timestamp match the
                                      initialized values.

  String formatting                   `toString()` includes the expected
                                      block and transaction information.

  Empty-block policy                  The block/commit workflow follows
                                      the project's defined rule for
                                      empty blocks.

  Tampering detection                 Chain validation detects altered
                                      block contents or broken hash
                                      links.
  -----------------------------------------------------------------------

Keep tests for the `Block` class distinct from tests for the
`Blockchain` ledger rules. A structurally correct block hash does not
prove that its transactions obey account-balance rules.

------------------------------------------------------------------------

## 9. Summary

`Block` represents one unit of the blockchain. It stores block metadata,
an ordered collection of transactions, a reference to the previous
block's hash, and its own hash. Its main behavior is to assemble
transactions, calculate a deterministic SHA-256 hash through `HashUtil`,
expose block information, and format details for display. The
`Blockchain` class remains responsible for managing the full chain,
commits, and balances.
