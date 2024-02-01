#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>
#include "numtheory.h"
#include <stdlib.h>
#include "randstate.h"

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    //Variable Initialization
    mpz_t lambda_n;
    mpz_inits(lambda_n, NULL);

    //Arithmetic Variable initialization
    mpz_t gcd_pq, abs_mul_pq, gcd_e_lambda;
    mpz_inits(gcd_pq, abs_mul_pq, gcd_e_lambda, NULL);

    //organizing bits for make_prime()
    uint64_t lower_bound = nbits / 4;
    uint64_t upper_bound = (3 * nbits) / 4;
    uint64_t pbits = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;
    uint64_t qbits = nbits - pbits;

    //making primes
    do {
        make_prime(p, pbits, iters);
        make_prime(q, qbits, iters);
        mpz_mul(n, p, q);
    } while (mpz_sizeinbase(n, 2) < nbits);

    //computing lcm(p -1, q - 1)
    gcd(gcd_pq, p, q);
    mpz_mul(abs_mul_pq, p, q);
    mpz_abs(abs_mul_pq, abs_mul_pq);
    mpz_fdiv_q(lambda_n, abs_mul_pq, gcd_pq);

    //finding public exponent
    do {
        mpz_urandomb(e, state, nbits);
        gcd(gcd_e_lambda, e, lambda_n);
    } while (mpz_cmp_ui(gcd_e_lambda, 1) != 0);

    //Cleaning Variables
    mpz_clears(lambda_n, NULL);
    mpz_inits(gcd_pq, abs_mul_pq, gcd_e_lambda, NULL);
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n", n);
    gmp_fprintf(pbfile, "%Zx\n", e);
    gmp_fprintf(pbfile, "%Zx\n", s);
    fprintf(pbfile, "%s\n", username);
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n", n);
    gmp_fscanf(pbfile, "%Zx\n", e);
    gmp_fscanf(pbfile, "%Zx\n", s);
    fprintf(pbfile, "%s\n", username);
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    //Initalizing arithmetic Variables
    mpz_t gcd_pq, abs_mul_pq, lambda_n;
    mpz_inits(gcd_pq, abs_mul_pq, lambda_n, NULL);

    //getting lambda(n)
    gcd(gcd_pq, p, q);
    mpz_mul(abs_mul_pq, p, q);
    mpz_abs(abs_mul_pq, abs_mul_pq);
    mpz_fdiv_q(lambda_n, abs_mul_pq, gcd_pq);

    //computing d
    mod_inverse(d, e, lambda_n);
    mpz_clears(gcd_pq, abs_mul_pq, lambda_n, NULL);
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n", n);
    gmp_fprintf(pvfile, "%Zx\n", d);
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n", n);
    gmp_fscanf(pvfile, "%Zx\n", d);
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    //Variable initialization
    size_t block_size = 0;
    mpz_t m, c;
    mpz_inits(m, c, NULL);

    //Calculating Block Size (block_size = floor((log2(n) - 1 ) / 8))
    block_size = (mpz_sizeinbase(n, 2) - 1) / 8;

    //Creating an array for bytes
    uint8_t *block = (uint8_t *) calloc(block_size, sizeof(uint8_t *));

    //setting zeroth block to 0xFF
    block[0] = 0xFF;

    //Reading the file and encrypting
    size_t j;
    while (!feof(infile)) {
        j = fread(&block[1], sizeof(uint8_t), block_size - 1, infile);
		if (feof(infile))
		{
			size_t paddingSize = block_size - 1 - j;
			for (size_t i = 0; i < paddingSize; ++i)
			{
				block[j + 1 + i] = paddingSize;
			}
			j += paddingSize;
		}
        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, block);
        rsa_encrypt(c, m, e, n);
        gmp_fprintf(outfile, "%Zx\n", m);
    }
}

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    //Variable initialization
    size_t block_size = 0;
    mpz_t m, c;
    mpz_inits(m, c, NULL);

    //Calculating Block Size (block_size = floor((log2(n) - 1 ) / 8))
    block_size = (mpz_sizeinbase(n, 2) - 1) / 8;

    //Creating an array for bytes
    uint8_t *block = (uint8_t *) calloc(block_size, sizeof(uint8_t *));

    //Reading the file and encrypting
    size_t j;
	bool isLastBlock = false;
    while (!feof(infile)) {
        gmp_fscanf(infile, "%Zx\n", c);
        rsa_decrypt(m, c, d, n);
        mpz_export(block, &j, 1, sizeof(uint8_t), 0, 0, c);

		if (feof(infile))
		{
			isLastBlock = true;
		}

		if (isLastBlock)
		{
			size_t paddingSize = block[j - 1];
			j -= paddingSize;
		}
        fwrite(&block[1], sizeof(uint8_t), block_size - 1, outfile);
    }
}
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t, sign;
    mpz_inits(t, sign, NULL);
    mpz_set(sign, s);
    pow_mod(t, sign, e, n);
    if (mpz_cmp(t, m) == 0) {
        return true;
    }
    return false;
}
