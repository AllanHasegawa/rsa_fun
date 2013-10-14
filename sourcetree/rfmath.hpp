#ifndef RF_RFMATH_HPP
#define RF_RFMATH_HPP

#include <tuple>
#include "gmpxx.h"

namespace rf {
	template<typename T>
	std::tuple<T,T,T> extended_euclidean(const T& a, const T& b)
	{
		using namespace std;
		if (b == 0) return make_tuple(a, 1, 0);
		auto t = extended_euclidean(b,
				static_cast<T>(a%b));
		return make_tuple(get<0>(t),
					get<2>(t),
				get<1>(t) - (a/b)*get<2>(t));
	}
	
	template<typename T>
	bool is_prime_naive(const T& n) 
	{
		if (n <= 0) return false;
		T i{2};
		const auto m = sqrt(n);
		while(i <= m) {
			if (n%i == 0) return false;
			++i;
		}
		return true;
	}

	bool is_obvious_composite(const mpz_class& n);

	/**
	 * Implementation of the Miller-Rabin primality test algorithm
	 * from (Pomerance; Selfridge and Wagstaff. The Pseudoprimes
	 * to 25.10^9. 1980) article.
	 *
	 * \param Number 'N' to test primality against
	 *
	 * \param Miller-Rabin base 'B'
	 *
	 * \return True if 'N' is strongpseudoprime('B')
	 * */
	bool is_prime_miller_rabin(const mpz_class& n, const long B);

	bool is_prime_BPSW(const mpz_class& n);

	void find_2_prime_factors_naive(const mpz_class& N,
				mpz_class& x, mpz_class& y);
} // end namespace rf

#endif //RF_RFMATH_HPP
