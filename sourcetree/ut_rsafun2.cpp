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

	describe("Crypt/Decrypt test2", []() {
		for (int i{}; i < 20; ++i) {
			auto pair = rf::gen_key_pairs(24, 1000, 8);
			RSAPrivateKey private_key = pair.first;
			RSAPublicKey public_key = pair.second;
	
			mpz_class r1,r2;
			it("crypt 92", [&]() {
				// original msg
				// crypt using plubic key values
				rf::crypt(92, public_key, r1);
			});
	
			it("decrypt_with_public_key 27", [&]() {
				// now decrypt!
				mpz_class c = r1;
				rf::decrypt_with_public_key(c, public_key, 8, r2);
				AssertThat(r2, Equals(92));
			});
	
			it("break_crypt 27", [&]() {
				auto pk = rf::break_crypt(public_key, 8);
				mpz_class c = r1;
				rf::decrypt(c, pk, r2);
				AssertThat(r2, Equals(92));
			});
		}
	});
});

