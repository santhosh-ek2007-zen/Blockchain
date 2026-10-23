# HashUtil Class Documentation

## 1. Overview

`HashUtil` is a utility class in the C++ transaction-based blockchain
project. Its responsibility is to calculate a **SHA-256 cryptographic
hash** from an input string and return the digest as lowercase
hexadecimal text.

The class keeps hashing logic separate from blockchain logic. `Block`
can use `HashUtil` to calculate a block hash without needing to
implement the SHA-256 algorithm itself.

**Header:** `include/HashUtil.h`\
**Implementation:** `src/HashUtil.cpp`\
**Dependency:** OpenSSL Crypto library

------------------------------------------------------------------------

## 2. Design Summary

`HashUtil` is intentionally small and stateless.

-   It has **no data members**.
-   It exposes one public member function: `sha256()`.
-   It uses OpenSSL's EVP digest interface to calculate SHA-256.
-   It converts the raw digest into a readable hexadecimal string.
-   It does not store blocks, transactions, balances, or blockchain
    state.

A `HashUtil` object does not need to remember anything between calls.
The output depends on the input provided to `sha256()`.

### Why is this class separate?

Hashing is a different responsibility from constructing blocks or
managing the blockchain. Keeping it in a dedicated class avoids
duplicating cryptographic code in `Block` and `Blockchain`, and makes
the hashing behavior easier to test independently.

------------------------------------------------------------------------

## 3. Data Members

### Data-member summary

  -----------------------------------------------------------------------
  Data member       Type              Access            Responsibility
  ----------------- ----------------- ----------------- -----------------
  None              ---               ---               `HashUtil` does
                                                        not maintain
                                                        object state. It
                                                        calculates a
                                                        digest from each
                                                        function call's
                                                        input.

  -----------------------------------------------------------------------

There are no private or public data members declared in `HashUtil.h`.

### Why are there no data members?

The class does not need to retain the input, digest, or OpenSSL working
context after a hash has been calculated. The input is supplied to the
function, and the digest is returned to the caller.

This stateless design keeps the class simple and avoids maintaining
unnecessary mutable state.

------------------------------------------------------------------------

## 4. Member Functions

### `sha256()`

**Declaration**

``` cpp
std::string sha256(const std::string& input) const;
```

  -----------------------------------------------------------------------
  Property                            Description
  ----------------------------------- -----------------------------------
  Access                              Public

  Return type                         `std::string`

  Parameter                           `const std::string& input`

  `const` member function             Does not modify the `HashUtil`
                                      object's state

  Main responsibility                 Calculate the SHA-256 digest of the
                                      supplied input and return it as
                                      hexadecimal text
  -----------------------------------------------------------------------

#### Responsibilities

1.  Receive the input string whose contents need to be hashed.
2.  Use OpenSSL's EVP interface to perform SHA-256 hashing.
3.  Check whether the required OpenSSL operations succeed.
4.  Convert the raw digest bytes into lowercase hexadecimal characters.
5.  Return the resulting digest as a `std::string`.
6.  Report a hashing/context failure using `std::runtime_error`, as
    implemented by this class.

SHA-256 produces a 256-bit digest. When represented as hexadecimal, that
digest is **64 hexadecimal characters** long.

#### Why is the parameter a `const` reference?

`const std::string&` avoids making an unnecessary copy of the input
string and prevents the function from modifying the caller's input
through that parameter.

#### Why is the member function `const`?

The function does not need to change any member data. Declaring it
`const` communicates that calling it does not change the state of the
`HashUtil` object.

### Constructors and destructors

`HashUtil.h` does not declare an explicit constructor or destructor. C++
supplies the default constructor and destructor, which are sufficient
because the class does not own persistent resources or maintain data
members.

### Helper functions

No separate public helper member function is declared in the class
interface. Digest conversion and OpenSSL-specific operations are
implementation details handled in `HashUtil.cpp`.

------------------------------------------------------------------------

## 5. Responsibility Boundaries

  -----------------------------------------------------------------------
  Component                           Responsibility
  ----------------------------------- -----------------------------------
  `HashUtil`                          Calculate SHA-256 and return the
                                      digest as hexadecimal text.

  `Block`                             Gather the block's relevant fields
                                      into a consistent representation
                                      and request a hash from `HashUtil`.

  `Blockchain`                        Manage blocks, pending
                                      transactions, balances, commits,
                                      and chain validation.

  `Transaction`                       Represent an individual transfer
                                      and provide its transaction data.
  -----------------------------------------------------------------------

`HashUtil` must not decide whether a transaction is valid, update
account balances, create blocks, or manage the chain. Those
responsibilities belong to other classes.

------------------------------------------------------------------------

## 6. How the Class Is Used

A caller creates a `HashUtil` object and invokes `sha256()` with the
string to hash.

``` cpp
HashUtil hasher;
std::string digest = hasher.sha256("abc");
```

The variable `digest` receives the SHA-256 digest as hexadecimal text.
The example demonstrates the interface; the caller is responsible for
deciding what data should be hashed.

In the blockchain project, `Block` can serialize its relevant fields in
a deterministic order and pass that representation to `sha256()`.

### Expected hashing flow

1.  The caller prepares the input string.
2.  `sha256()` passes the input to OpenSSL's SHA-256 implementation.
3.  OpenSSL calculates the raw digest.
4.  The implementation converts the digest bytes to lowercase
    hexadecimal.
5.  The hexadecimal digest is returned to the caller.

------------------------------------------------------------------------

## 7. Design Rules and Important Notes

-   **Deterministic input matters:** the same input bytes should produce
    the same digest.
-   **Serialization belongs to the caller:** for a block hash, the
    block's fields must be represented consistently before being passed
    to `HashUtil`.
-   **Do not implement SHA-256 manually for this project:** use the
    OpenSSL implementation.
-   **Handle failures:** the implementation reports hashing/context
    failures using `std::runtime_error`.
-   **Hashing is not encryption:** SHA-256 does not provide a way to
    recover the original input from the digest.
-   **A hash alone does not prove authenticity:** hashing does not
    establish who created the data or whether a transaction was
    authorized.
-   **A hash-linked chain is not automatically immutable:** someone able
    to rewrite the whole chain and recalculate subsequent hashes may be
    able to produce a structurally consistent replacement. This
    educational project does not include distributed consensus or
    digital signatures.

------------------------------------------------------------------------

## 8. Testing Responsibilities

The hashing class should be tested independently of the blockchain.

Recommended checks:

  -----------------------------------------------------------------------
  Test                                Expected result
  ----------------------------------- -----------------------------------
  Known test vector: input `"abc"`    Returns the standard SHA-256 digest
                                      for `"abc"`.

  Empty input                         Hashes the empty input correctly.

  Repeated input                      The same input produces the same
                                      digest.

  Different inputs                    Different test inputs should
                                      produce their corresponding SHA-256
                                      digests; collisions are
                                      theoretically possible, so this is
                                      not a mathematical guarantee that
                                      every different input has a
                                      different digest.

  Output format                       The hexadecimal digest contains 64
                                      characters for SHA-256.

  Failure path                        OpenSSL/context failures are
                                      reported as defined by the
                                      implementation.
  -----------------------------------------------------------------------

Use a trusted known SHA-256 test vector rather than checking the
function only against itself.

------------------------------------------------------------------------

## 9. Summary

`HashUtil` is a stateless hashing class with one public member function,
`sha256()`, and no data members. It delegates SHA-256 calculation to
OpenSSL, returns the digest in lowercase hexadecimal form, and keeps
cryptographic hashing separate from the blockchain's transaction, block,
and ledger responsibilities.
