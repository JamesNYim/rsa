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
        "   This program will encrypt a file\n"
        "\n"
        "USAGE\n"
        "   %s [-i:o:n:vh] "
        "-[i: specifies the input], [o: specifies the output], [n: specifies file containing "
        "public key], [v: verbose output], [h: help page]"
        "\n"
        "OPTIONS\n"
        "   -i         specifies the input file to encrypt (default: stdin)\n"
        "   -o         specifies the output file to encrypt (default: stdout)\n"
        "   -n         specifies the file containing the public key (default: rsa.pub)\n"
        "   -v         enables verbose output\n"
        "   -h         displays program synopsis and usage\n",
        exec);
}

int main(int argc, char **argv) {
    //Variable declaration & initialization
    int opt = 0;
    bool verbose_output = false;
    char *public_key_filename = "rsa.pub";
    char *input_filename = "stdin";
    char *output_filename = "stdout";
    FILE *public_key_file;
    FILE *input_file;
    FILE *output_file;

    //Organizing user input
    while ((opt = getopt(argc, argv, "i:o:n:vh")) != -1) {
        switch (opt) {
        case 'i': input_filename = optarg; break;
        case 'o': output_filename = optarg; break;
        case 'n': public_key_filename = optarg; break;
        case 'v': verbose_output = true; break;
        case 'h': usage(argv[0]); return EXIT_FAILURE;
        default: fprintf(stderr, "incorrect usage please see -H for help\n"); return 1;
        }
    }

    //opening files
    input_file = fopen(input_filename, "rb");
    output_file = fopen(output_filename, "wb");
    public_key_file = fopen(public_key_filename, "r");
    if (public_key_file == NULL || input_file == NULL) {
        fprintf(stderr, "Files failed to open, please try again (see -h for help)");
        fclose(input_file);
        fclose(output_file);
        fclose(public_key_file);
        return 1;
    }

    mpz_t n, e, s, m;
    mpz_inits(n, e, s, m, NULL);
    char *username = getenv("USER");

    rsa_read_pub(n, e, s, username, public_key_file);
    //verifying username
    /*
	mpz_set_str(m, username, 62);
    if (!rsa_verify(m, s, e, n)) {
        fclose(input_file);
        fclose(output_file);
        fclose(public_key_file);
        fprintf(stderr, "signature could not be verified please try again (see -h for help)");
        exit(EXIT_FAILURE);
    }
	*/

    //encrypting
    rsa_encrypt_file(input_file, output_file, n, e);

    //if verbose
    if (verbose_output) {
        printf("User = %s\n", getenv("USER"));
        gmp_printf("s (%lu bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("n (%lu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("d (%lu bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    }

    //closing and clearing
    fclose(public_key_file);
    fclose(input_file);
    fclose(output_file);
    mpz_clears(n, e, s, m, NULL);
    return 0;
}
