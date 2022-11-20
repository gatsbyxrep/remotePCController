#ifndef SHA1_H
#define SHA1_H

#include <inttypes.h>


typedef struct
 {
   uint32_t state[5];
   uint32_t count[2];
   unsigned char buffer[64];
 } SHA1_CTX;
static unsigned char charset[64] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};

void SHA1Transform(uint32_t state[5], const unsigned char buffer[64]);
void SHA1Init(SHA1_CTX* context);
void SHA1Update(SHA1_CTX* context, const unsigned char* data, uint32_t len);
void SHA1Final(unsigned char digest[20], SHA1_CTX* context);
void SHA1(unsigned char* hash_out, const char* str, unsigned int len);
int base64_encode(unsigned char sha_key_in[], unsigned char base64_key_out[], int len);

#endif // SHA1_H
