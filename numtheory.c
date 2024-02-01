#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>
#include "randstate.h"
void gcd(mpz_t d, mpz_t a, mpz_t b) {
    mpz_t tempVar, a_var, b_var;
    mpz_init_set(b_var, b);
    mpz_init_set(a_var, a);
    while (mpz_cmp_ui(b_var, 0) != 0) {
        mpz_init_set(tempVar, b_var); // tempVar = b
        mpz_mod(b_var, a_var, b_var); //b = a % b;
        mpz_set(a_var, tempVar); //a = tempVar
    }
    mpz_set(d, a_var); //'returning' the value
    mpz_clears(tempVar, a_var, b_var, NULL);
}

void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {
    mpz_t tempVar, r, r1, t, t1, a_var, n_var, q, rMult, tMult; //initalizing variables

    //setting variables for pointer args
    mpz_init_set(a_var, a);
    mpz_init_set(n_var, n);

    //(r, r1) = (n, a);
    mpz_init_set(r, n_var); // r = n
    mpz_init_set(r1, a_var); // r1 = a

    //(t, t1) = (0, 1)
    mpz_init_set_ui(t, 0); //t = 0
    mpz_init_set_ui(t1, 1); //t1 = 1

    while (mpz_cmp_ui(r1, 0) != 0) //while (b != 0)
    {

        //Creating the variable q
        mpz_init(q); //initalizing the variable q
        mpz_fdiv_q(q, r, r1); //q = r / r1 (floor)

        //R swap
        mpz_init_set(tempVar, r1); //initalizing temVar and setting to r1
        mpz_init(rMult);
        mpz_mul(rMult, q, r1); //(q * r1)
        mpz_sub(r1, r, rMult); //r1 = (q * r1);
        mpz_set(r, tempVar); //r = tempVar

        //T swap
        mpz_set(tempVar, t1); //temVar setting to r1
        mpz_init(tMult);
        mpz_mul(tMult, q, t1); //(q * t1)
        mpz_sub(t1, t, tMult); //t1 = (q * t1);
        mpz_set(t, tempVar); //t = tempVar
    }

    if (mpz_cmp_ui(r, 1) > 0) {
        return;
    }

    if (mpz_cmp_ui(t, 0) < 0) {
        mpz_add(t, t, n_var);
    }
    mpz_set(i, t);
    mpz_clears(tempVar, r, r1, t, t1, a_var, n_var, q, rMult, tMult, NULL);
}

void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
    //setting and initialzing variables
    mpz_t out_var, base_var, exponent_var, modulus_var;
    mpz_t exponent_mod_two, out_times_base, base_var_squared;

    mpz_init_set_ui(out_var, 1);
    mpz_init_set(base_var, base);
    mpz_init_set(exponent_var, exponent);
    mpz_init_set(modulus_var, modulus);
    mpz_inits(base_var_squared, out_times_base, exponent_mod_two, NULL);

    while (mpz_cmp_ui(exponent_var, 0) > 0) {
        mpz_mod_ui(exponent_mod_two, exponent_var, 2); //exponent % 2
        if (mpz_cmp_ui(exponent_mod_two, 0) != 0) //if (exponent % 2 != 0)
        {
            //out_var = (out_var * base_var) % modulus_var
            mpz_mul(out_times_base, out_var, base_var);
            mpz_mod(out_var, out_times_base, modulus_var);
        }
        mpz_mul(base_var_squared, base_var, base_var);
        mpz_mod(base_var, base_var_squared,
            modulus_var); //base_var = (base_var * base_var) % modulus_var
        mpz_fdiv_q_ui(exponent_var, exponent_var, 2); //exponent_var = exponent_var / 2
    }
    mpz_set(out, out_var);
    mpz_clears(out_var, base_var, exponent_var, modulus_var, NULL);
    mpz_clears(exponent_mod_two, out_times_base, base_var_squared, NULL);
}

bool is_prime(mpz_t n, uint64_t iters) {
    //Variable initalization and declaration
    mpz_t n_var, r, s, a, y, j;
    mpz_inits(r, s, a, y, j, NULL);
    mpz_init_set(n_var, n);

    //arithmetic variable initialization and declaration
    mpz_t mod_2, int_var, n_minus_1, upper_random;
    mpz_inits(mod_2, int_var, n_minus_1, upper_random, NULL);

    //if (n == 2)
    if (mpz_cmp_ui(n_var, 2) == 0) {
        return true;
    }

    //checking if n is even
    mpz_mod_ui(mod_2, n, 2);
    if (mpz_cmp_ui(mod_2, 0) == 0) {
        return false;
    }

    //Writing (n - 1) == (2^s * r) such that r is odd
    mpz_sub_ui(r, n_var, 1); //r = (n - 1)
    mpz_set_ui(s, 0); //s = 0

    //while (r % 2) == 0 'while r is even'
    mpz_mod_ui(mod_2, r, 2);
    while (mpz_cmp_ui(mod_2, 0) == 0) {
        mpz_fdiv_q_ui(r, r, 2);
        mpz_add_ui(s, s, 1);
        mpz_mod_ui(mod_2, r, 2);
    }

    //loop for guessing if its prime
    for (uint64_t i = 0; i < iters; i++) {
        //a = some random number between 2 and n - 2
        mpz_sub_ui(upper_random, n_var, 2);
        mpz_sub_ui(upper_random, upper_random, 2);
        mpz_urandomm(a, state, upper_random);
        mpz_add_ui(a, a, 2);

        pow_mod(y, a, r, n_var); //y = pow_mod(a, r, n_var)

        //if (y != 1 and y != (n - 1)
        mpz_sub_ui(n_minus_1, n_var, 1);
        if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, n_minus_1) != 0) {
            mpz_set_ui(j, 1);
            while (mpz_cmp(j, s) <= 0 && mpz_cmp(y, n_minus_1) != 0) {
                mpz_set_ui(int_var, 2);
                pow_mod(y, y, int_var, n_var);
                if (mpz_cmp_ui(y, 1) == 0) {
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }

            //if (y != (n - 1))
            if (mpz_cmp(y, n_minus_1) != 0) {
                return false;
            }
        }
    }

    //clearing variables
    mpz_clears(n_var, r, s, a, y, j, NULL);
    mpz_clears(mod_2, int_var, n_minus_1, upper_random, NULL);
    return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {

    //variable declaration & iterations
    mpz_t p_var;
    mpz_inits(p_var, NULL);

    //while loop runs as long as theres no prime
    while (!is_prime(p_var, iters)) {
        mpz_urandomb(p_var, state, bits); //getting random number at least bits long
    }
    mpz_set(p, p_var);
}
