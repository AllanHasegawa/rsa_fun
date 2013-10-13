#include <iostream>
#include <tuple>
#include "rfmath.hpp"
#include "rfutils.hpp"

int main(void) {
	using namespace std;

	int i{};
	auto t = rf::extended_euclides(120,23);

	cout << t << endl;

	return 0;
}
