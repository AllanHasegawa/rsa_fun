#include <iostream>
#include <vector>
#include <tuple>

namespace rf {
	template<typename T>
	std::tuple<T,T,T> extended_euclidean(const T& a, const T& b)
	{
		using namespace std;
		if (b == 0) return make_tuple(a, 1, 0);
		auto t = extended_euclidean(b,
				static_cast<T>(a%b));
		return make_tuple(get<0>(t),
					get<2>(t),
				get<1>(t) - (a/b)*get<2>(t));
	}

	template<typename T>
	bool is_prime_naive(const T& n) 
	{
		if (is_obvious_composite(n)) return false;
		T i{5};
		const auto m = sqrt(n);
		while(i <= m) {
			if (n%i == 0) return false;
			i+=2;
		}
		return true;
	}

	template<typename T>
	struct PrimeBuffer {
		static std::vector<T> value;

		PrimeBuffer(const int np)
		{
			if (value.empty()) {
				value.reserve(np);
				T n{2};
				for (int i{0}; i < np; ++n) {
					if (is_prime_naive<T>(n)) {
						value.push_back(n);
						++i;
					}
				}
			}
		}

		static bool search_prime(const T& n)
		{
			int a{};
			int b{static_cast<int>(value.size())};

			while (b >= a) {
				int c = a + ((b-a)/2);
				if (value[c] == n) return true;
				if (value[c] < n) a = c + 1;
				else b = c - 1;
			}
			return false;
		}
	};
	template<typename T>
	std::vector<T> PrimeBuffer<T>::value = {};

} // end namespace rf
