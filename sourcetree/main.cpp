#include <iostream>
#include <fstream>
#include "gmp.h"
#include "gmpxx.h"
#include "cereal/archives/binary.hpp"
#include "rfutils.hpp"
#include "rsafun.hpp"

int main(int argc, char* argv[]) {
	using namespace std;

	using PB = rf::PrimeBuffer<mpz_class>;
	PB{100};

	int precision_bits = 38;
	int threads = 8;
	{
	ofstream ofs_pub("key.pub");

	cereal::BinaryOutputArchive boa_pub(ofs_pub);
	auto keys = rf::gen_key_pairs(32, 100, 8);
	boa_pub(keys.second);
	cout << keys.second.e << endl;
	}
	{
	ifstream ifs_pub("key.pub");

	cereal::BinaryInputArchive bia_pub(ifs_pub);
	std::string estr;
	std::string nstr;
	bia_pub(estr, nstr);
	rf::RSAPublicKey public_key;
	cout << estr << endl;
	public_key.e = mpz_class{estr.c_str(), 16};
	public_key.n = mpz_class{nstr.c_str(), 16};
	cout << public_key.e << endl;
	}

	auto f = rf::file_to_bytes("message1");

	rf::bytes_to_file("message2", f);

	return 0;
}
