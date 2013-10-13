#include <tuple>

namespace rf {
	template<typename T>
	std::tuple<T,T,T> extended_euclides(const T& a, const T& b)
	{
		using namespace std;
		if (b == 0) return make_tuple(a, 1, 0);
		auto t = extended_euclides(b,a%b);
		return make_tuple(get<0>(t), get<2>(t),
				get<1>(t) - (a/b)*get<2>(t));
	}

} // end namespace rf
