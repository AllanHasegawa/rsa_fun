#ifndef RF_RSAFUN_HPP
#define RF_RSAFUN_HPP

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
} // end namespace rf

#endif //RF_RSAFUN_HPP
