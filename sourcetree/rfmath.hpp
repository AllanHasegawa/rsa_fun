#ifndef RF_RFMATH_HPP
#define RF_RFMATH_HPP

#include <vector>
#include <tuple>
#include "gmpxx.h"

namespace rf {
	
	template<typename T>
	std::tuple<T,T,T> extended_euclidean(const T& a, const T& b);

	void jacobi_number(const mpz_class& a,
			const mpz_class& b, mpz_class& n);
	
	template<typename T>
	struct PrimeBuffer;
	
	template<typename T>
	bool is_prime_naive(const T& n);

	bool is_prime_fermat(const mpz_class& n, const int p, const int t);

	bool is_prime_in_blocks(const mpz_class& n, const int BS);

	bool is_obvious_composite(const mpz_class& n);

	/**
	 * Implementation of the Miller-Rabin primality test algorithm
	 * from (Pomerance; Selfridge and Wagstaff. The Pseudoprimes
	 * to 25.10^9. 1980) article.
	 *
	 * \param Number 'N' to test primality against
	 * \param Miller-Rabin base 'B'
	 *
	 * \return true if 'N' is strongpseudoprime('B')
	 * */
	bool is_prime_miller_rabin(const mpz_class& n, const long B);

	/**
	 * Implementation of the Strong Lucas Selfridge primality test
	 * algorithm from (Pomerance; Selfridge and Wagstaff. The
	 * Pseudoprimes to 25.10^9. 1980) article.
	 *
	 * \param Number 'N' to test primality against
	 *
	 * \return true if 'N' is a Lucas-Selfridge strong pseudoprime
	 * */
	bool is_prime_strong_lucas_selfridge(const mpz_class& n);

	bool is_prime_BPSW(const mpz_class& n);

	void find_2_prime_factors_naive(const mpz_class& N,
				mpz_class& x, mpz_class& y);
} // end namespace rf

#include "rfmath_imp.hpp"

#endif //RF_RFMATH_HPP
