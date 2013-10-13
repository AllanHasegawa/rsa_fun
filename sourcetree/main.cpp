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

	mpz_class r;
	rf::crypt(92, 17, 143, r);
	cout << r << endl;

	rf::decrypt(r.get_si(), 113, 143, r);

	cout << r << endl;

	return 0;
}
