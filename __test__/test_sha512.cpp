#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

#include <openssl/sha.h>

int main(int argc, char** argv)
{
    SHA512_CTX ctx;
    u_int8_t results[SHA512_DIGEST_LENGTH];
    char *buf = "abc";
    int n = strlen(buf);
    SHA512_Init(&ctx);
    SHA512_Update(&ctx, (u_int8_t *)buf, n);
    SHA512_Final(results, &ctx);

    /* Print the digest as one long hex value */
    printf("0x");
    for (n = 0; n < SHA512_DIGEST_LENGTH; n++)
        printf("%02x", results[n]);
    putchar('\n');
}