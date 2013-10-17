#include <iostream>
#include "gmp.h"
#include "gmpxx.h"
#include "rfutils.hpp"
#include "rsafun.hpp"

int main(int argc, char* argv[]) {
	using namespace std;

	using PB = rf::PrimeBuffer<mpz_class>;
	PB{100};

	int precision_bits = 38;
	int threads = 8;
	auto f = rf::file_to_bytes("message1");

	mpz_class x;
	mpz_class y;
	rf::find_2_prime_factors_blocks(n, threads, x, y);
	cout << "n: " << n << endl;
	cout << "xy:" << x * y << endl;
	cout << "x: " << x << endl;
	cout << "y: " << y << endl;

	if (x*y == 0)
		rf::find_2_prime_factors_naive(n, x, y);
	cout << string(25, '-') << endl;
	cout << "n: " << n << endl;
	cout << "xy:" << x * y << endl;
	cout << "x: " << x << endl;
	cout << "y: " << y << endl;

	rf::bytes_to_file("message2", f);

	return 0;
}
