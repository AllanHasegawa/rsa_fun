#ifndef RF_RFUTILS_HPP
#define RF_RFUTILS_HPP
#include <fstream>
#include <vector>
#include <tuple>
#include <ostream>
#include "cereal/archives/binary.hpp"
#include "rsafun.hpp"

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

	void save_keys(const RSAKeyPair& keys, const std::string filename)
	{
		using namespace std;
		ofstream ofs_pub(filename + ".pub");
		ofstream ofs_priv(filename + ".priv");

		cereal::BinaryOutputArchive boa_pub(ofs_pub);
		cereal::BinaryOutputArchive boa_priv(ofs_priv);

		boa_priv(keys.first);
		boa_pub(keys.second);
	}

	RSAPrivateKey open_private_key(const std::string filename)
	{
		using namespace std;

		ifstream ifs_pub(filename);
		cereal::BinaryInputArchive bia_pub(ifs_pub);

		std::string dstr;
		std::string nstr;
		bia_pub(dstr, nstr);

		rf::RSAPrivateKey private_key;
		private_key.d = mpz_class{dstr.c_str(), 16};
		private_key.n = mpz_class{nstr.c_str(), 16};

		return private_key;
	}

	RSAPublicKey open_public_key(const std::string filename)
	{
		using namespace std;

		ifstream ifs_pub(filename);
		cereal::BinaryInputArchive bia_pub(ifs_pub);

		std::string estr;
		std::string nstr;
		bia_pub(estr, nstr);

		rf::RSAPublicKey public_key;
		public_key.e = mpz_class{estr.c_str(), 16};
		public_key.n = mpz_class{nstr.c_str(), 16};

		return public_key;
	}
} // end namespace rf

#endif //RF_RFUTILS_HPP
