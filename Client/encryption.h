#define USE_CRYPTO 0
#if USE_CRYPTO > 0

#ifndef CDN_ENCRYPT_H
#define CDN_ENCRYPT_H

#define KEYFILE ".keyfile"
#define IVFILE ".ivfile"

#include <string>

// Prototype encryption, not for any production use
std::string encryptFile(const std::string &fileContents);
std::string decryptFile(const std::string &fileContents);

#endif

#endif
