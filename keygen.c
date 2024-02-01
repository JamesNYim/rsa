#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <gmp.h>
#include <unistd.h>
#include <sys/stat.h>
#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"

//Help Page
void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "   This program will create a public key and a private key needed for cryptography\n"
        "\n"
        "USAGE\n"
        "   %s [-b:i:n:d:s:vh] "
        "-[b: specifies the minimum number of bits], [i: specifies number of iterations], [n: "
        "public key file], [d: private key file], [s: seed], [v: verbose output], [h: help page]"
        "\n"
        "OPTIONS\n"
        "   -b         specifies if the universe is toroidal\n"
        "   -i         silences ncurses\n"
        "   -n         number of generations. Default: 100\n"
        "   -d         specifies the input file to read to populate the universe. Default: stdin\n"
        "   -s         specifies output file to print the final universe state. Default: stdout\n"
        "   -v	       enables verbose output\n"
        "   -h         displays program synopsis and usage\n",
        exec);
}

int main(int argc, char **argv) {
    //Variable declaration & initialization
    int opt = 0;
    uint64_t seed = time(NULL);
    bool verbose_output = false;
    uint64_t num_of_bits = 64;
    uint64_t iterations = 50;
    char *private_key_filename = "rsa.priv";
    char *public_key_filename = "rsa.pub";
    FILE *private_key_file;
    FILE *public_key_file;

    //Organizing user input
    while ((opt = getopt(argc, argv, "b:i:n:d:s:vh")) != -1) {
        switch (opt) {
        case 'b': num_of_bits = strtod(optarg, NULL); break;
        case 'i': iterations = strtod(optarg, NULL); break;
        case 'd': private_key_filename = optarg; break;
        case 'n': public_key_filename = optarg; break;
        case 's': seed = strtod(optarg, NULL); break;
        case 'v': verbose_output = true; break;
        case 'h': usage(argv[0]); return EXIT_FAILURE;
        default: fprintf(stderr, "incorrect usage please see -H for help\n"); return 1;
        }
    }

    //Checking if fopen was successful
    private_key_file = fopen(private_key_filename, "w");
    public_key_file = fopen(public_key_filename, "w");

    //setting file perms for private key
    fchmod(fileno(private_key_file), 0600);

    //setting random state
    randstate_init(seed);
    srandom(seed);

    //Making public and private keys
    mpz_t p, q, private_key, public_modulo, public_exponent, signature, username;
    mpz_inits(p, q, private_key, public_modulo, public_exponent, signature, username, NULL);

    make_prime(p, num_of_bits, iterations);
    make_prime(q, num_of_bits, iterations);

    rsa_make_pub(p, q, public_modulo, public_exponent, num_of_bits, iterations);
    rsa_make_priv(private_key, public_exponent, p, q);

    //getting username and signature
    mpz_set_str(username, getenv("USER"), 62);
    rsa_sign(signature, username, private_key, public_modulo);

    //writing keys
    rsa_write_priv(public_modulo, private_key, private_key_file);
    rsa_write_pub(public_modulo, public_exponent, signature, getenv("USER"), public_key_file);

    if (verbose_output) {
        printf("User = %s\n", getenv("USER"));
        gmp_printf("s (%lu bits) = %Zd\n", mpz_sizeinbase(signature, 2), signature);
        gmp_printf("p (%lu bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_printf("q (%lu bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_printf("n (%lu bits) = %Zd\n", mpz_sizeinbase(public_modulo, 2), public_modulo);
        gmp_printf("e (%lu bits) = %Zd\n", mpz_sizeinbase(public_exponent, 2), public_exponent);
        gmp_printf("d (%lu bits) = %Zd\n", mpz_sizeinbase(private_key, 2), private_key);
    }

    //closing files and clearing variables
    randstate_clear();
    fclose(public_key_file);
    fclose(private_key_file);
    mpz_clears(p, q, private_key, public_modulo, public_exponent, signature, NULL);
}
