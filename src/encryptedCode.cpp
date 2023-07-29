#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include <stdio.h>

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}

int encrypt(unsigned char* plaintext, int plaintext_len, unsigned char* key,
    unsigned char* iv, unsigned char* ciphertext)
{
    EVP_CIPHER_CTX* ctx;
    int len;
    int ciphertext_len;

    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int main(void)
{
    /* A 256 bit key */
    unsigned char key[32] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                           0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
                           0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
                           0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31
    };

    /* A 128 bit IV */
    unsigned char iv[16] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                          0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35
    };

    char plaintext[128];
    unsigned char ciphertext[128];
    char decryptedtext[128];

    int ciphertext_len;
    int decryptedtext_len;

    // Encrypting and writing to a new file
    FILE* infile, * outfile;
    infile = fopen("students_with_class.txt", "r");
    outfile = fopen("students_encrypted.txt", "wb");

    if (!infile || !outfile)
    {
        fprintf(stderr, "Failed to open file(s).\n");
        return 1;
    }

    while (fgets(plaintext, sizeof plaintext, infile) != NULL)
    {
        ciphertext_len = encrypt((unsigned char*)plaintext, strlen(plaintext), key, iv, ciphertext);
        fwrite(&ciphertext_len, sizeof(int), 1, outfile);  // write length of ciphertext Must To remember on Decrypte
        fwrite(ciphertext, sizeof(unsigned char), ciphertext_len, outfile);
    }

    fclose(infile);
    fclose(outfile);

    return 0;
}
