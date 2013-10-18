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

	rf::bytes_to_file("message2", f);

	return 0;
}
