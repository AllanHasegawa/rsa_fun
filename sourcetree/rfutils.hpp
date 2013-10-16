#ifndef RF_RFUTILS_HPP
#define RF_RFUTILS_HPP
#include <fstream>
#include <vector>
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


namespace rf {
	std::vector<uint8_t> file_to_bytes(const std::string& file_name)
	{
		using namespace std;
		ifstream fs(file_name, istream::binary);
		if (!fs) throw std::invalid_argument("File (" + file_name
				+ ") not found");
		fs.seekg(0, fs.end);
		int size = fs.tellg();
		fs.seekg(0, fs.beg);

		std::vector<uint8_t> r;
		r.reserve(size);

		char ch;
		while (fs.get(ch)) {
			r.push_back(
				static_cast<uint8_t>(ch));
		}
		return r;
	}

	void bytes_to_file(const std::string& file_name,
			const std::vector<uint8_t>& b)
	{
		using namespace std;
		ofstream fs(file_name, ofstream::binary);

		if (!fs) throw std::invalid_argument("File (" + file_name
				+ ") failed to be opened");
		
		for (auto& B : b) {
			fs.put(B);
		}
	}
} // end namespace rf

#endif //RF_RFUTILS_HPP
