#include <iostream>
#include <random>
#include "bandit/bandit.h"
#include "rfmath.hpp"
using namespace bandit;
using namespace rf;
using namespace std;

go_bandit([]() {

	describe("Primality test", []() {
		using PB = PrimeBuffer<mpz_class>;
		PB{500};
		const auto P = PB::value;

		it("naive <=3", [](){
			AssertThat(is_prime_naive(-1), Equals(false));
			AssertThat(is_prime_naive(0), Equals(false));
			AssertThat(is_prime_naive(1), Equals(true));
			AssertThat(is_prime_naive(2), Equals(true));
			AssertThat(is_prime_naive(3), Equals(true));
		});
		it("naive 439", [](){
			AssertThat(is_prime_naive(439), Equals(true));
		});
		it("naive 441", [](){
			AssertThat(is_prime_naive(441), Equals(false));
		});

		mt19937 e(random_device{}());
		uniform_int_distribution<int> d(0, 500-1);

		for (int i = 0; i < 20; ++i) {
			it("random is_prime_in_blocks", [&](){
				AssertThat(
					is_prime_in_blocks(
						P[d(e)]),
					Equals(true));
			});
		}

		it("PrimeBuffer search_prime", [&]() {
			int c{};
			for (long i{2}; i <= PB::value.back(); ++i) {
				if (PB::search_prime(i)) ++c;
			}
			AssertThat(c, Equals(500));
		});
	});
});

int main(int argc, char* argv[])
{
	return bandit::run(argc, argv);
}
