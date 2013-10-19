#include <string>
#include <iostream>
#include <fstream>
#include "gmp.h"
#include "gmpxx.h"
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"
#include "rfutils.hpp"
#include "rsafun.hpp"

int main(int argc, char* argv[]) {
	using namespace std;

	string usage = "./rsacrypt <public key file> <original file>"
		" <crypt file>";

	if (argc < 4) {
		cout << usage << endl;
		return 0;
	}

	string keyfile = argv[1];
	string messagefile = argv[2];
	string cryptfile = argv[3];

	rf::RSAPublicKey public_key = rf::open_public_key(keyfile);

	auto message = rf::file_to_bytes(messagefile);
	vector<string> crypt_message;

	auto np = public_key.n.get_mpz_t();
	// size in bits of the public key
	// (determines max size of the block!)
	auto n_size = mpz_sizeinbase(np,2);
	// 2 bits bc the function may return +/- 2 bits
	int max_block_size = (n_size-2)/8;

	mpz_class r;
	mpz_class block = 0;
	auto blockp = block.get_mpz_t();
	mpz_class m;
	auto mp = m.get_mpz_t();
	int c{};
	for_each (message.begin(), message.end(),[&](uint8_t b) {
		m = b;
		mpz_mul_2exp(mp, mp, c*8);	
		block = block + m;
		++c;
		if (c == max_block_size) {
			rf::crypt(block, public_key, r);
			string t(r.get_str(16));
			crypt_message.push_back(t);
			c = 0;
			block = 0;
		}
	});

	if (c > 0) {
		mpz_mul_2exp(blockp, blockp, (max_block_size-c)*8);
		rf::crypt(block, public_key, r);
		string t(r.get_str(16));
		crypt_message.push_back(t);
	}


	ofstream ofs(cryptfile);
	cereal::BinaryOutputArchive boa(ofs);
	boa(crypt_message);


	return 0;
}
