
#include "../include/HashUtil.h"

#include <cstdint>
#include <vector>
#include <sstream>
#include <iomanip>
#include <string>

HashUtil::HashUtil()
{
    // No object-specific state is required.
}

bool HashUtil::sha256(
    const std::string& input,
    std::string& output,
    std::string& errorMessage
) const
{
    output.clear();
    errorMessage.clear();

    // SHA-256 initial hash values.
    uint32_t hash[8] = {
        0x6a09e667, 0xbb67ae85,
        0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c,
        0x1f83d9ab, 0x5be0cd19
    };

    // SHA-256 round constants.
    static const uint32_t K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70,
        0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585,
        0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c,
        0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f,
        0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814,
        0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7,
        0xc67178f2
    };

    // Prepare the input message for SHA-256.
    std::vector<unsigned char> message(
        input.begin(), input.end()
    );

    // Append one bit represented by the byte 0x80.
    message.push_back(0x80);

    // Pad until the message length is 56 modulo 64.
    while (message.size() % 64 != 56) {
        message.push_back(0x00);
    }

    // Append the original message length in bits,
    // represented as a 64-bit big-endian integer.
    uint64_t bitLength =
        static_cast<uint64_t>(input.size()) * 8ULL;

    for (int i = 7; i >= 0; --i) {
        message.push_back(
            static_cast<unsigned char>(
                (bitLength >> (i * 8)) & 0xff
            )
        );
    }

    // Process the message in 512-bit (64-byte) blocks.
    for (std::size_t offset = 0;
         offset < message.size();
         offset += 64)
    {
        uint32_t W[64];

        // Convert the first 16 words from big-endian bytes.
        for (int i = 0; i < 16; ++i) {
            std::size_t j = offset + i * 4;

            W[i] =
                (static_cast<uint32_t>(message[j]) << 24) |
                (static_cast<uint32_t>(message[j + 1]) << 16) |
                (static_cast<uint32_t>(message[j + 2]) << 8) |
                static_cast<uint32_t>(message[j + 3]);
        }

        // Expand the 16 words into 64 words.
        for (int i = 16; i < 64; ++i) {
            uint32_t s0 =
                ((W[i - 15] >> 7) |
                 (W[i - 15] << 25)) ^
                ((W[i - 15] >> 18) |
                 (W[i - 15] << 14)) ^
                (W[i - 15] >> 3);

            uint32_t s1 =
                ((W[i - 2] >> 17) |
                 (W[i - 2] << 15)) ^
                ((W[i - 2] >> 19) |
                 (W[i - 2] << 13)) ^
                (W[i - 2] >> 10);

            W[i] = W[i - 16] + s0 +
                   W[i - 7] + s1;
        }

        // Copy the current hash values into working variables.
        uint32_t a = hash[0];
        uint32_t b = hash[1];
        uint32_t c = hash[2];
        uint32_t d = hash[3];
        uint32_t e = hash[4];
        uint32_t f = hash[5];
        uint32_t g = hash[6];
        uint32_t h = hash[7];

        // Perform the 64 SHA-256 rounds.
        for (int i = 0; i < 64; ++i) {
            uint32_t S1 =
                ((e >> 6) | (e << 26)) ^
                ((e >> 11) | (e << 21)) ^
                ((e >> 25) | (e << 7));

            uint32_t choose =
                (e & f) ^ (~e & g);

            uint32_t temp1 =
                h + S1 + choose + K[i] + W[i];

            uint32_t S0 =
                ((a >> 2) | (a << 30)) ^
                ((a >> 13) | (a << 19)) ^
                ((a >> 22) | (a << 10));

            uint32_t majority =
                (a & b) ^ (a & c) ^ (b & c);

            uint32_t temp2 = S0 + majority;

            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        // Add the working variables to the current hash.
        hash[0] += a;
        hash[1] += b;
        hash[2] += c;
        hash[3] += d;
        hash[4] += e;
        hash[5] += f;
        hash[6] += g;
        hash[7] += h;
    }

    // Convert the eight 32-bit words into 64 hex characters.
    std::ostringstream hexStream;
    hexStream << std::hex << std::setfill('0');

    for (int i = 0; i < 8; ++i) {
        hexStream << std::setw(8) << hash[i];
    }

    output = hexStream.str();

    return true;
}
