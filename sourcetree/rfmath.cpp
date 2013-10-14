#include <iostream>
#include <random>
#include <stdexcept>
#include "gmpxx.h"
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

bool rf::is_prime_BPSW(const mpz_class& n)
{
	return true;
}

/**
 * aux function to find 'd' and 's' from
 * n - 1 = d*2^s
 * */
void __find_s_d(const mpz_class& n, mpz_class& d, long& s)
{
	auto dp = d.get_mpz_t();
	mpz_class n1 = n-1;
	auto n1p = n1.get_mpz_t();
	s = mpz_scan1(n1p, 0); // divide number by 2 until find odd
	mpz_tdiv_q_2exp(dp, n1p, s); // d == (int)n / (2^s)
}

bool rf::is_prime_miller_rabin(const mpz_class& n, const int k)
{
	if (n <= 0) return false;
	if (n <= 3) return true;
	if (k <= 0) throw std::invalid_argument("k must be >0");
	auto np = n.get_mpz_t();

	// if (n%2==0) composite
	if (mpz_divisible_ui_p(np, 2) != 0) return false;

	// n-1 = (2^s)*d
	mpz_class d;
	long s;
	__find_s_d(n, d, s);

	auto dp = d.get_mpz_t();
	std::cout << s << " " << d << std::endl;
	// if 2^d mod n == 1 -> spsp(2)
	// of if a^(d*(2^r)) mod n == -1 -> spsp(2)
	mpz_class i2{2};
	mpz_class r;
	auto rp = r.get_mpz_t();
	auto i2p = i2.get_mpz_t();

	mpz_powm(rp, i2p, dp, np);
	if (r == 1) return true;
	
	return true;
}
