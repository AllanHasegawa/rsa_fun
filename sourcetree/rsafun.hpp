#ifndef RF_RSAFUN_HPP
#define RF_RSAFUN_HPP

#include <utility>
#include <tuple>
#include <cmath>
#include "gmp.h"
#include "gmpxx.h"
#include "rfmath.hpp"

namespace rf {
	enum class RSAKeyType { PUBLIC, PRIVATE };

	template <RSAKeyType T>
	struct RSAKey {};

	template<>
	struct RSAKey<RSAKeyType::PRIVATE> {
		mpz_class d;
		mpz_class n;
	};

	template<>
	struct RSAKey<RSAKeyType::PUBLIC> {
		mpz_class e;
		mpz_class n;
	};

	using RSAPrivateKey = RSAKey<RSAKeyType::PRIVATE>;
	using RSAPublicKey = RSAKey<RSAKeyType::PUBLIC>;
	using RSAKeyPair = std::pair<RSAPrivateKey,RSAPublicKey>;

	
	RSAKeyPair gen_key_pairs(const int precision_bits,
				const int fermat_passes, const int threads);

	void crypt(const mpz_class& M, const RSAPublicKey& public_key,
				mpz_class& result);

	void decrypt(const mpz_class& C, const RSAPrivateKey& private_key,
				mpz_class& result);

	void decrypt_with_public_key(const mpz_class& C,
				const RSAPublicKey& public_key,
				const int threads,
				mpz_class& result);
} // end namespace rf

#endif //RF_RSAFUN_HPP
