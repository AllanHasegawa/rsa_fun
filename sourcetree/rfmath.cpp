#include <stdexcept>
#include "gmp.h"
#include "rfmath.hpp"


void rf::find_2_prime_factors_naive(const mpz_class& N,
	mpz_class& x, mpz_class& y)
{
	mpz_class t{2};
	auto Nptr = N.get_mpz_t();
	auto tptr = t.get_mpz_t();
	while (t < N) {
		// if (N%t) == 0
			if (mpz_divisible_p(Nptr, tptr) != 0) {
			x = t;
			y = N/t;
			return;
		}
		++t;
	}

	throw std::invalid_argument("Failed to find prime factor");
}
