#ifndef HASHUTIL_H
#define HASHUTIL_H

#include <string>

class HashUtil
{
public:
    // Constructor
    HashUtil();
    // Calculates the SHA-256 hash of input.
    bool sha256(
        const std::string& input,
        std::string& output,
        std::string& errorMessage
    ) const;
};

#endif
