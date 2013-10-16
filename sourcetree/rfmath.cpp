#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>
#include "gmpxx.h"
#include "gmp.h"
#include "rfmath.hpp"


bool rf::is_prime_fermat(const mpz_class& n, const int p, const int t)
{
	if (t < 1) throw std::invalid_argument("t must be >0");
	if (p < 1) throw std::invalid_argument("p must be >0");
	if (n <= 0) return false;
	if (n <= 3) return true;

	auto np = n.get_mpz_t();
	mpz_class n1{n-1};
	auto n1p = n1.get_mpz_t();
	
	// sequential algorithm
	if (t == 1) {
		gmp_randstate_t rs;
		gmp_randinit_mt(rs);
		gmp_randseed_ui(rs, std::random_device{}());

		mpz_class ri;
		auto rip = ri.get_mpz_t();

		for (int i{}; i < p; ++i) {
			do {
				mpz_urandomm(rip, rs, np);
			} while (ri < 1);

			mpz_powm(rip, rip, n1p, np);
			if (ri != 1) {
				gmp_randclear(rs);
				return false; // composite for sure
			}
		}
		gmp_randclear(rs);
	}
	return true; // probable prime
}

bool rf::is_prime_in_blocks(const mpz_class& n)
{
	if (is_obvious_composite(n)) return false;
	if (n <= 3) return true;
	if (n < PrimeBuffer<mpz_class>::value.back())
		return PrimeBuffer<mpz_class>::search_prime(n);
	if (n < 30*30) return is_prime_naive(n);

	// 30k + {1, 7, 11, 13, 17, 19, 23, 29}
	// 43 = 30*1 + 13
	// 757 = 30*25 + 7
	auto np = n.get_mpz_t();
	mpz_class t;
	auto tp = t.get_mpz_t();
	
	mpz_class i{30};
	auto ip = i.get_mpz_t();
	mpz_class ns;
	for (; i < ns; i += 30) {
		mpz_add_ui(tp, ip, 1);
		if (mpz_divisible_p(np, tp)) return false;
		mpz_add_ui(tp, ip, 7);
		if (mpz_divisible_p(np, tp)) return false;
		mpz_add_ui(tp, ip, 11);
		if (mpz_divisible_p(np, tp)) return false;
		mpz_add_ui(tp, ip, 13);
		if (mpz_divisible_p(np, tp)) return false;
		mpz_add_ui(tp, ip, 17);
		if (mpz_divisible_p(np, tp)) return false;
		mpz_add_ui(tp, ip, 19);
		if (mpz_divisible_p(np, tp)) return false;
		mpz_add_ui(tp, ip, 23);
		if (mpz_divisible_p(np, tp)) return false;
		mpz_add_ui(tp, ip, 29);
		if (mpz_divisible_p(np, tp)) return false;
	}

	return true;
}

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

bool rf::is_obvious_composite(const mpz_class& n)
{
	if (n <= 0) return true;
	if (n <= 3) return false;
	auto np = n.get_mpz_t();

	// if (n%2==0) composite
	if (mpz_even_p(np)) return true;
	return false;
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

bool rf::is_prime_miller_rabin(const mpz_class& n, const long B)
{
	if (is_obvious_composite(n)) return false;
	if (B < 2) throw std::invalid_argument("B must be >1");
	
	// n-1 = (2^s)*d
	mpz_class d;
	auto dp = d.get_mpz_t();
	long s;
	__find_s_d(n, d, s);
	
	// if B^d mod n == 1 -> spsp(B)
	mpz_class b{B};
	mpz_class x;
	auto bp = b.get_mpz_t();
	auto xp = x.get_mpz_t();
	auto np = n.get_mpz_t();
	mpz_powm(xp, bp, dp, np);
	if (x == 1) return true;

	// of if B^(d*(2^r)) mod n == -1 -> spsp(B)
	//
	// B^(d*2) = (B^d) * (B^d)
	// B^(d*2^2) = B^(d*4) = (B^d*2) * (B^d*2)
	// ...
	for (long r{}; r < s; ++r) {
		mpz_mul(xp, xp, xp); // B^(d*2^r) = (B^(d*2^(r-1)))^2
		mpz_mod(xp, xp, np); // B^(d*2^r) mod n
		if (x == -1) return true;
	}
	
	// composite
	return false;
}


bool rf::is_prime_strong_lucas_selfridge(const mpz_class& n)
{
	throw std::runtime_error("Not implemented");
	return true;
}

bool rf::is_prime_BPSW(const mpz_class& n)
{
	throw std::runtime_error("Not implemented");
	return true;
}

