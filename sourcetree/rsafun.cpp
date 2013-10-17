
#include "rsafun.hpp"

#include <utility>
#include <tuple>
#include <cmath>
#include "gmp.h"
#include "gmpxx.h"
#include "rfmath.hpp"

rf::RSAKeyPair rf::gen_key_pairs(const int precision_bits,
				const int fermat_passes, const int threads)
{
	RSAPrivateKey private_key;
	RSAPublicKey public_key;
	
	mpz_class p;
	mpz_class q;
	rf::random_prime(precision_bits, fermat_passes, threads, p);
	rf::random_prime(precision_bits, fermat_passes, threads, q);

	mpz_class pq{(p-1)*(q-1)};
	mpz_class n{p*q};


	// ed = 1 + pq(a)
	mpz_class e;	
	rf::random_coprime(pq, precision_bits, threads, e);



	// ed = 1 + pq(a)
	// e is know, also "pq".
	// Use extended euclidean to find d and a;
	const auto ee = rf::extended_euclidean(pq,public_key.e);
	mpz_class d{std::get<2>(ee)};
	// get the next positive value of "d"
	while (d <= 0) {
		d += pq;
	}

	public_key.n = n;
	public_key.e = std::move(e);

	private_key.d = std::move(d);
	private_key.n = std::move(n);
	
	return std::make_pair(private_key, public_key);
}

void rf::crypt(const mpz_class& M, const RSAPublicKey& public_key,
					mpz_class& result)
{
	if (public_key.type != RSAKeyType::PUBLIC)
		throw std::invalid_argument("RSAKey must be PUBLIC");
	// modular exponentiation doenst seems to be supported
	// by the C++ interface :(
	//
		// result = M^e mod n
	mpz_powm(result.get_mpz_t(), 
			M.get_mpz_t(),
			public_key.e.get_mpz_t(),
			public_key.n.get_mpz_t());	
}

void rf::decrypt(const mpz_class& C, const RSAKey& private_key,
					mpz_class& result)
{
	if (private_key.type != RSAKeyType::PRIVATE)
		throw std::invalid_argument("RSAKey must be PRIVATE");
	// result = C^d mod n
	mpz_powm(result.get_mpz_t(),
			C.get_mpz_t(),
			private_key.e.get_mpz_t(),
			private_key.n.get_mpz_t());
}

void rf::decrypt_with_public_key(const mpz_class& C, 
			const RSAKey& public_key, const int threads,
			mpz_class& result)
{
	if (public_key.type != RSAKeyType::PUBLIC)
		throw std::invalid_argument("RSAKey myst be PUBLIC");
	mpz_class x, y;
	rf::find_2_prime_factors_blocks(public_key.n, threads, x, y);

	// fist make (p-1)*(q-1)
	mpz_class pq{(x-1)*(y-1)};

	// ed = 1 + pq(a)
	// e is know, also "pq".
	// Use extended euclidean to find d and a;
	const auto ee = rf::extended_euclidean(pq,public_key.e);
	mpz_class d{std::get<2>(ee)};
	// get the next positive value of "d"
	while (d <= 0) {
		d += pq;
	}

	RSAKey private_key;
	key

	// now we have all the private key values,
	// just decrypt :)
	rf::decrypt(C, , result);
}

