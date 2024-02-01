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
        "   This program will decrypt a file\n"
        "\n"
        "USAGE\n"
        "   %s [-i:o:n:vh] "
        "-[i: specifies the input], [o: specifies the output], [n: specifies file containing "
        "private key], [v: verbose output], [h: help page]"
        "\n"
        "OPTIONS\n"
        "   -i         specifies the input file to decrypt (default: stdin)\n"
        "   -o         specifies the output file to decrypt (default: stdout)\n"
        "   -n         specifies the file containing the priv key (default: rsa.priv)\n"
        "   -v         enables verbose output\n"
        "   -h         displays program synopsis and usage\n",
        exec);
}

int main(int argc, char **argv) {
    //Variable declaration & initialization
    int opt = 0;
    bool verbose_output = false;
    char *private_key_filename = "rsa.priv";
    char *input_filename = "stdin";
    char *output_filename = "stdout";
    FILE *private_key_file;
    FILE *input_file;
    FILE *output_file;

    //Organizing user input
    while ((opt = getopt(argc, argv, "b:i:o:n:d:s:vh")) != -1) {
        switch (opt) {
        case 'i': input_filename = optarg; break;
        case 'o': output_filename = optarg; break;
        case 'n': private_key_filename = optarg; break;
        case 'v': verbose_output = true; break;
        case 'h': usage(argv[0]); return EXIT_FAILURE;
        default: fprintf(stderr, "incorrect usage please see -H for help\n"); return 1;
        }
    }

    //opening files
    input_file = fopen(input_filename, "rb");
    output_file = fopen(output_filename, "wb");
    private_key_file = fopen(private_key_filename, "r");

    if (private_key_file == NULL || input_file == NULL) {
        fprintf(stderr, "Files failed to open, please try again (see -h for help)");
        fclose(input_file);
        fclose(output_file);
        fclose(private_key_file);
        return 1;
    }

    mpz_t n, d;
    mpz_inits(n, d, NULL);

    //reading private key
    rsa_read_priv(n, d, private_key_file);

    //decrypting
    rsa_decrypt_file(input_file, output_file, n, d);

    //if verbose
    if (verbose_output) {
        printf("User = %s\n", getenv("USER"));
        gmp_printf("n (%lu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("d (%lu bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    //closing and clearing
    fclose(private_key_file);
    fclose(input_file);
    fclose(output_file);
    mpz_clears(n, d, NULL);
    return 0;
}
