#include <tuple>
#include <ostream>


namespace rf {
	/**
	 * print_tuple is an aux class to provide partial especialization.
	 *
	 * It's a functional compile-time way of printing tuples.
	 * Probably not work in EVERY case, works for this project.
	 * Usage is simple: std::cout << std::tuple;
	 * */
	template<int N>
	struct __print_tuple {
		template<typename S,typename T>
		static void print(S& o,const T& t)
		{
			__print_tuple<N-1>::print(o,t);
			o << "," << std::get<N-1>(t);
		}
	};

	template<>
	struct __print_tuple<1> {
		template<typename S,typename T>
		static void print(S& o, const T& t)
		{
			o << std::get<0>(t);
		}
	};
} // end namespace rf

template<typename C, typename T, typename...Args>
std::basic_ostream<C,T>& operator<<(std::basic_ostream<C,T>& o,
		const std::tuple<Args...>& t)
{
	o << "(";
	rf::__print_tuple<
		sizeof...(Args)
		>::print(o,t);
	return o << ")";
}

