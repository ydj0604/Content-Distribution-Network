#include "encryption.h"
#if USE_CRYPTO > 0

#include <iostream>
#include <fstream>
#include <sstream>

#include "cryptopp/modes.h"
#include "cryptopp/aes.h"
#include "cryptopp/filters.h"
#include "cryptopp/filters.h"
#include "cryptopp/osrng.h"
#include "cryptopp/queue.h"


using namespace CryptoPP;
using namespace std;

// #include <boost/filesystem.hpp>

void printKey(const SecByteBlock& key) {
  for (int i = 0; i < key.size(); i++)
    printf("%02x ", key.data()[i]);
  cout << endl;
}

bool fileExists(const string& filePath) {
  ifstream f(filePath.c_str());
  if (f)
    return true;
  return false;
}

SecByteBlock loadKey(const string& keyFilePath) {
  ifstream keyF(keyFilePath.c_str());
  stringstream buf;
  buf << keyF.rdbuf();
  string contents = buf.str();

  SecByteBlock key((unsigned char*)contents.data(), contents.size()-1);

  return key;
}

void saveKey(const string& keyFilePath, const SecByteBlock& key) {
  cout << "Saving key to " << keyFilePath << endl;
  cout << "Key: " << endl;
  printKey(key);

  ofstream keyF;
  keyF.open(keyFilePath.c_str());
  keyF << key.data(); 
  keyF.close();
}

SecByteBlock getKey(const string &keyFilePath) {
  // Get key from file if exists
  // if not, generate key and write to file

  if (fileExists(keyFilePath)) {
    // cout << "Key File Exists, reading from " << keyFilePath << endl;
    return loadKey(keyFilePath);
  } else {
    cout << "Key does not exist, generating new one" << endl;
  }

  // Otherwirse, generate random key
  AutoSeededRandomPool rnd;

  SecByteBlock key(0x00, AES::DEFAULT_KEYLENGTH);
  rnd.GenerateBlock( key, key.size() );

  // Write to file
  saveKey(keyFilePath, key);

  return key;
}

string encryptFile(const string &fileContents) {
  SecByteBlock key = getKey(KEYFILE);
  SecByteBlock iv = getKey(IVFILE);

  size_t len = fileContents.size();
  char buf[len];
  strcpy(buf, fileContents.c_str());

  // Encrypt buf, store in buf
  CFB_Mode<AES>::Encryption cfbEncryption(key, key.size(), iv);
  cfbEncryption.ProcessData((byte*)buf, (byte*)buf, len);

  string encryptedS(buf);
  return encryptedS;
}

string decryptFile(const string &fileContents) {
  SecByteBlock key = getKey(KEYFILE);
  SecByteBlock iv = getKey(IVFILE);

  size_t len = fileContents.size();
  char buf[len];
  strcpy(buf,fileContents.c_str());

  // Decrypt to buf
  CFB_Mode<AES>::Decryption cfbDecryption(key, key.size(), iv);
  cfbDecryption.ProcessData((byte*)buf, (byte*)buf, len);

  string decryptedS(buf);
  return decryptedS;
}

/*
int main() {
  string test = "Hello, World!xxkdsajf;lksdjf";

  cout << test << endl;

  string enc = encryptFile(test);
  cout << enc << endl;
  cout << decryptFile(enc) << endl;
  return 0;
}
*/

#endif
