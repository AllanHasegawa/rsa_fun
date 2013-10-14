#include <iostream>
#include <tuple>
#include "gmpxx.h"
#include "rfmath.hpp"
#include "rfutils.hpp"
#include "rsafun.hpp"

int main(void) {
	using namespace std;

	cout << rf::extended_euclidean(120,67) << endl;

	for (int i = 0; i < 100; ++i) {
		if (rf::is_prime_naive(i)) {
			cout << i << endl;
		}
	}

	cout << endl << string(25,'-') << endl;

	mpz_class p{11};
	mpz_class q{13};
	mpz_class n{p*q};
	mpz_class e{17};
	mpz_class r;

	// original msg
	// crypt using plubic key values
	rf::crypt(92, e, n, r);
	cout << r << endl;


	// now decrypt!
	// factor the N from public key
	mpz_class x, y;
	rf::find_2_prime_factors_naive(n, x, y);
	cout << x << "," << y << endl;

	// fist make (p-1)*(q-1)
	mpz_class pq{(x-1)*(y-1)};
	cout << pq << endl;

	// ed = 1 + pq(a)
	// e is know, also "pq". Use extended euclidean to find d and a;
	const auto ee = rf::extended_euclidean(pq,e);
	cout << ee << endl;
	mpz_class d{get<2>(ee)};
	// get the next positive value of "d"
	while (d <= 0) {
		d += pq;
	}

	// now we have all the private key values,
	// just decrypt :)
	rf::decrypt(r, d, n, r);
	cout << r << endl;

	cout << rf::is_prime_miller_rabin(213, 2) << endl;
	return 0;
}
