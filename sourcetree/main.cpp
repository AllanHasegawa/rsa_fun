#include <iostream>
#include "gmp.h"
#include "gmpxx.h"
#include "rfutils.hpp"
#include "rsafun.hpp"

int main(int argc, char* argv[]) {
	using namespace std;

	using PB = rf::PrimeBuffer<mpz_class>;
	PB{10000};

	auto f = rf::file_to_bytes("message1");

	mpz_class t{1};
	for (int i = 0; i < 2; ++i) {
		rf::random_prime(1024, 1000, 8, t);
		cout << t << endl;
	}

	rf::bytes_to_file("message2", f);

	return 0;
}
