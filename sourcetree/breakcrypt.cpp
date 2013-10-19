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
	

	string usage = "./rsabreakcrypt <public key file> <threads> <crypted file>"
		" <decrypted file>";

	if (argc < 4) {
		cout << usage << endl;
		return 0;
	}

	string keyfile = argv[1];
	string threads_str = argv[2];
	string cryptfile = argv[3];
	string decryptfile  = argv[4];

	int threads{};
	try {
		threads = stoi(threads_str);
	} catch (exception& e) {
		cout << "Error: Wrong number entered..." << endl;
		return 0;
	}

	rf::RSAPublicKey public_key = rf::open_public_key(keyfile);
	rf::RSAPrivateKey private_key = rf::break_crypt(public_key, threads);

	auto np = public_key.n.get_mpz_t();
	// size in bits of the public key
	// (determines max size of the block!)
	auto n_size = mpz_sizeinbase(np,2);
	// 2 bits bc the function may return +/- 2 bits
	int max_block_size = (n_size-2)/8;

	vector<string> crypt_message;

	ifstream ifs(cryptfile);
	cereal::BinaryInputArchive bia(ifs);
	bia(crypt_message);

	mpz_class b;
	auto bp = b.get_mpz_t();
	mpz_class r;
	auto rp = r.get_mpz_t();
	vector<uint8_t> message;
	for_each (crypt_message.begin(), crypt_message.end(),[&](string&  m) {
		mpz_class C{m, 16};
		rf::decrypt(C, private_key, r);
		for (int i{1}; i <= max_block_size; ++i) {
			mpz_tdiv_r_2exp(bp, rp, 8);
			message.push_back(b.get_ui());

			mpz_tdiv_q_2exp(rp, rp, 8);
		}
	});

	rf::bytes_to_file(decryptfile, message);

	return 0;
}
