#include <iostream>
#include "gmp.h"

int main(void) {
	using namespace std;
	mpz_t sum;
	mpz_init(sum);
	mpz_set_str(sum, "12345", 10);
	cout << sum << endl;
	mpz_clear(sum);
	int i{};
	cout << i << endl;
	return 0;
}
