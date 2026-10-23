#include "../include/HashUtil.h"
#include <openssl/evp.h>
#include <iomanip>
#include <sstream>
HashUtil::HashUtil() {}

bool HashUtil::sha256(
    const std::string& input,
    std::string& output,
    std::string& errorMessage
) const {
    output.clear();
    errorMessage.clear();

    EVP_MD_CTX* context = EVP_MD_CTX_new();

    if (context == nullptr) {
        errorMessage = "Failed to create SHA-256 context.";
        return false;
    }

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digestLength = 0;

    bool success =
        EVP_DigestInit_ex(context, EVP_sha256(), nullptr) == 1;

    if (success) {
        success =
            EVP_DigestUpdate(context, input.data(), input.size()) == 1;
    }

    if (success) {
        success =
            EVP_DigestFinal_ex(context, digest, &digestLength) == 1;
    }

    EVP_MD_CTX_free(context);

    if (!success) {
        errorMessage = "OpenSSL failed to calculate SHA-256.";
        return false;
    }

    std::ostringstream hexStream;
    hexStream << std::hex << std::setfill('0');

    for (unsigned int i = 0; i < digestLength; i++) {
        hexStream << std::setw(2)
                  << static_cast<unsigned int>(digest[i]);
    }

    output = hexStream.str();
    return true;
}
