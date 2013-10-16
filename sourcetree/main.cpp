#include <iostream>
#include <tuple>
#include "gmpxx.h"
#include "rfmath.hpp"
#include "rfutils.hpp"
#include "rsafun.hpp"

int main(int argc, char* argv[]) {
	using namespace std;

	using PB = rf::PrimeBuffer<mpz_class>;
	PB{10000};
	const auto& PBv = PB::value;

	return 0;
}
