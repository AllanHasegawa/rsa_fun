#include <iostream>
#include <tuple>
#include "rfmath.hpp"
#include "rfutils.hpp"

int main(void) {
	using namespace std;

	cout << rf::extended_euclidean(120,67) << endl;

	for (int i = 0; i < 100; ++i) {
		if (rf::is_prime_naive(i)) {
			cout << i << endl;
		}
	}

	return 0;
}
