#include <tuple>
#include "gmpxx.h"

namespace rf {
	template<typename T>
	std::tuple<T,T,T> extended_euclidean(const T& a, const T& b)
	{
		using namespace std;
		if (b == 0) return make_tuple(a, 1, 0);
		auto t = extended_euclidean(b,a%b);
		return make_tuple(get<0>(t), get<2>(t),
				get<1>(t) - (a/b)*get<2>(t));
	}
	
	bool is_prime_naive(const mpz_class& n) {
		mpz_class i{"2"};
		const auto m = sqrt(n);
		while(i <= m) {
			if (n%i == 0) return false;
			++i;
		}
		return true;
	}
} // end namespace rf
