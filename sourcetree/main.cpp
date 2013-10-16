#include <iostream>
#include "rfutils.hpp"
#include "rsafun.hpp"

int main(int argc, char* argv[]) {
	using namespace std;

	using PB = rf::PrimeBuffer<mpz_class>;
	PB{10000};

	auto f = rf::file_to_bytes("message1");



	rf::bytes_to_file("message2", f);

	return 0;
}
