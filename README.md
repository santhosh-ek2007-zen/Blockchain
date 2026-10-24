# Design and Implementation of Blockchain

*A modular, transaction-based blockchain simulator built with C++ and Object-Oriented Programming.*

## 1. Overview

This project implements the fundamental concepts of blockchain technology using C++. It demonstrates how transactions are created, grouped into blocks, linked using SHA-256 cryptographic hashes, and validated to detect modifications to the blockchain.

The project follows an object-oriented architecture that separates transaction management, block creation, blockchain operations, and cryptographic hashing into individual components.

It is designed as an educational, single-process blockchain simulator.

## 2. Features

- **Transaction Management:** Represents transfers between accounts.
- **Block Creation:** Groups multiple transactions into a block.
- **SHA-256 Hashing:** Generates cryptographic hashes for blocks.
- **Block Linking:** Connects consecutive blocks using previous-block hashes.
- **Blockchain Validation:** Detects altered block contents and broken hash links.
- **Account Balance Management:** Tracks account balances and validates transfers.
- **Pending Transactions:** Maintains transactions awaiting inclusion in a block.
- **Object-Oriented Design:** Organizes functionality into modular C++ classes.

## 3. Technology Stack

- **Programming Language:** C++
- **Programming Paradigm:** Object-Oriented Programming (OOP)
- **Cryptographic Algorithm:** SHA-256
- **Cryptographic Library:** OpenSSL
- **Compiler:** GCC / G++
- **Version Control:** Git
- **Repository Hosting:** GitHub

## 4. Architecture / Project Structure

The project separates header files, source files, and the main application entry point.

```text
Blockchain/
├── include/
│   ├── Transaction.h
│   ├── Block.h
│   ├── Blockchain.h
│   └── HashUtil.h
├── src/
│   ├── Transaction.cpp
│   ├── Block.cpp
│   ├── Blockchain.cpp
│   └── HashUtil.cpp
├── main.cpp
└── README.md
```

### Core Components

- **Transaction:** Represents a transfer between two accounts and handles basic transaction validation.
- **Block:** Stores transactions, block metadata, the previous block's hash, and its own hash.
- **Blockchain:** Manages committed blocks, pending transactions, account balances, block creation, and chain validation.
- **HashUtil:** Provides SHA-256 hashing functionality using OpenSSL.
- **main.cpp:** Provides the console interface through which users interact with the blockchain.

Header files (`.h`) contain class declarations, while source files (`.cpp`) contain their implementations.

## 5. Prerequisites

Before building the project, ensure the following tools are installed:

- A C++ compiler supporting C++17 or the standard configured for the project.
- Git for cloning the repository.
- OpenSSL development libraries for SHA-256 hashing.

Verify that OpenSSL is installed and that its headers and libraries are accessible to the compiler.
## 6.Installing OpenSSL for a C++ Blockchain Project on Windows

## Step 1: Install MSYS2

For a MinGW-based C++ project, MSYS2 is a convenient way to install OpenSSL and a compatible compiler toolchain.

Official website: https://www.msys2.org/

1. Download and install MSYS2 using the official installer.
2. Keep the default installation directory, typically `C:\msys64`.
3. Open the **MSYS2 MINGW64** terminal from the Start menu. Use the MINGW64 terminal rather than the plain MSYS terminal.

## Step 2: Update MSYS2

In the MSYS2 MINGW64 terminal, run:

```bash
pacman -Syu
```

If the terminal asks you to close it, close it, reopen the MINGW64 terminal, and run the update command again if required.

## Step 3: Install the compiler toolchain and OpenSSL

Run:

```bash
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-openssl
```

Confirm the installation when prompted.

This installs the MinGW-w64 compiler toolchain and the OpenSSL development package.

## Step 4: Verify the installation

Check the compiler:

```bash
g++ --version
```

Check the OpenSSL command-line tool:

```bash
openssl version
```

Check that the SHA header exists:

```bash
ls /mingw64/include/openssl/sha.h
```

## 7. Installation and Build Instructions

### Step 1: Clone the repository

```bash
git clone https://github.com/santhosh-ek2007-zen/Blockchain
cd Blockchain
```

### Step 2: Compile the project

Using a GCC-compatible compiler with OpenSSL installed:

```bash
g++ -std=c++17 -Iinclude main.cpp src/Transaction.cpp src/Block.cpp src/Blockchain.cpp src/HashUtil.cpp -o blockchain -lcrypto
```

This command compiles the main application and its implementation files, includes the project headers, and links the OpenSSL cryptographic library.

### Step 3: Run the application

On Linux or macOS:

```bash
./blockchain
```

On Windows PowerShell:

```powershell
.\\blockchain.exe
```

**Important:** These commands assume your files follow the directory structure shown above and that OpenSSL is correctly installed. Adjust the filenames, paths, and linking options if your actual setup differs.

## 8. Limitations and Roadmap

### Current Limitations

- The blockchain operates as a single-process educational simulator.
- Peer-to-peer networking between blockchain nodes is not implemented.
- Distributed consensus mechanisms are not implemented.
- Digital signatures and cryptographic transaction authorization are not implemented.
- Persistent storage and recovery across application restarts may not be supported.

### Future Roadmap

- Add persistent blockchain storage.
- Implement digital signatures and transaction verification.
- Explore consensus mechanisms.
- Introduce communication between multiple blockchain nodes.
- Improve transaction validation and ledger consistency.
- Develop a graphical user interface.

## 9. Project Status

This project is under development as an educational implementation of blockchain fundamentals using C++ and Object-Oriented Programming.
