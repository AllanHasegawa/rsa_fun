
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
	using namespace std;
	RSAPrivateKey private_key;
	RSAPublicKey public_key;
	
	mpz_class p;
	mpz_class q;
	rf::random_prime(precision_bits, fermat_passes, threads, p);
	rf::random_prime(precision_bits, fermat_passes, threads, q);
	while (p == q) {
		rf::random_prime(precision_bits, fermat_passes, threads, q);
	}

	mpz_class pq{(p-1)*(q-1)};
	mpz_class n{p*q};
	auto np = n.get_mpz_t();

	// gcd(e,pq) == 1
	mpz_class e;
	auto ep = e.get_mpz_t();
	mpz_class t;
	auto tp = t.get_mpz_t();
	while (true) {
		rf::random_coprime(pq, precision_bits, threads, e);
		t = 42;
		mpz_powm(tp, tp, ep, np);
		if (e == p || e == q || e == n || t == 42) continue;
		else break;
	}


	// ed = 1 + pq(a)
	// e is know, also "pq".
	// Use extended euclidean to find d and a;
	auto ee = rf::extended_euclidean(pq,e);
	mpz_class d{std::get<2>(ee)};
	// get the next positive value of "d"
	// not equal to "e"
	while (d <= 0 || d == e) {
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
	// modular exponentiation doenst seems to be supported
	// by the C++ interface :(
	//
		// result = M^e mod n
	mpz_powm(result.get_mpz_t(), 
			M.get_mpz_t(),
			public_key.e.get_mpz_t(),
			public_key.n.get_mpz_t());	
}

void rf::decrypt(const mpz_class& C, const RSAPrivateKey& private_key,
					mpz_class& result)
{
	// result = C^d mod n
	mpz_powm(result.get_mpz_t(),
			C.get_mpz_t(),
			private_key.d.get_mpz_t(),
			private_key.n.get_mpz_t());
}

void rf::decrypt_with_public_key(const mpz_class& C, 
			const RSAPublicKey& public_key, const int threads,
			mpz_class& result)
{
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

	RSAPrivateKey private_key;
	
	private_key.n = public_key.n;
	private_key.d = d;

	// now we have all the private key values,
	// just decrypt :)
	rf::decrypt(C, private_key, result);
}

