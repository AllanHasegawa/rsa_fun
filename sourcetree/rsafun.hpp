#ifndef RF_RSAFUN_HPP
#define RF_RSAFUN_HPP

#include <tuple>
#include <cmath>
#include "gmp.h"
#include "gmpxx.h"
#include "rfmath.hpp"

namespace rf {
	void crypt(const mpz_class& M, const mpz_class& e,
			const mpz_class& n, mpz_class& result)
	{
		// modular exponentiation doenst seems to be supported
		// by the C++ interface :(
		//
		// result = M^e mod n
		mpz_powm(result.get_mpz_t(), 
				M.get_mpz_t(),
				e.get_mpz_t(),
				n.get_mpz_t());	
	}

	void decrypt(const mpz_class& C, const mpz_class& d,
			const mpz_class& n, mpz_class& result)
	{
		// result = C^d mod n
		mpz_powm(result.get_mpz_t(),
				C.get_mpz_t(),
				d.get_mpz_t(),
				n.get_mpz_t());
	}

	void decrypt_with_public_key(const mpz_class& C, const mpz_class& e,
			const mpz_class& n, const int threads,
			mpz_class& result)
	{
		mpz_class x, y;
		rf::find_2_prime_factors_blocks(n, threads, x, y);
	
		// fist make (p-1)*(q-1)
		mpz_class pq{(x-1)*(y-1)};
	
		// ed = 1 + pq(a)
		// e is know, also "pq".
		// Use extended euclidean to find d and a;
		const auto ee = rf::extended_euclidean(pq,e);
		mpz_class d{std::get<2>(ee)};
		// get the next positive value of "d"
		while (d <= 0) {
			d += pq;
		}
	
		// now we have all the private key values,
		// just decrypt :)
		rf::decrypt(C, d, n, result);
	}
} // end namespace rf

#endif //RF_RSAFUN_HPP
