#include <tuple>
#include <ostream>


namespace rf {
	/**
	 * print_tuple is an aux class to provide partial especialization.
	 *
	 * It's a functional compile-time way of printing tuples.
	 * Usage is simple: std::cout << std::tuple;
	 * */
	template<int I,int N>
	struct print_tuple {
		template<typename S,typename T>
		static void print(S& o,const T& t)
		{
			o << std::get<N-I>(t) << ",";
			print_tuple<I-1,N>::print(o,t);
		}
	};

	template<int N>
	struct print_tuple<0,N> {
		template<typename S,typename T>
		static void print(S& o, const T& t)
		{
			o << std::get<N>(t);
		}
	};
} // end namespace rf

template<typename C, typename T, typename...Args>
std::basic_ostream<C,T>& operator<<(std::basic_ostream<C,T>& o,
		const std::tuple<Args...>& t)
{
	o << "(";
	rf::print_tuple<
		sizeof...(Args)-1,
		sizeof...(Args)-1>::print(o,t);
	return o << ")";
}

