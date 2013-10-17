#include <iostream>
#include <tuple>
#include <random>
#include "bandit/bandit.h"
#include "rsafun.hpp"
#include "rfmath.hpp"
using namespace bandit;
using namespace rf;
using namespace std;

go_bandit([]() {

	describe("Crypt/Decrypt test", []() {
		using PB = PrimeBuffer<mpz_class>;
		PB{500};
		
		mpz_class p{11};
		mpz_class q{13};
		mpz_class n{p*q};
		mpz_class e{17};
		mpz_class r;

		it("crypt 92", [&]() {
			// original msg
			// crypt using plubic key values
			rf::crypt(92, e, n, r);
			AssertThat(r, Equals(27));
		});

		it("decrypt_with_public_key 27", [&]() {
			// now decrypt!
			mpz_class c = r;
			rf::decrypt_with_public_key(c, e, n, 8, r);
			AssertThat(r, Equals(92));
		});
	});
});

