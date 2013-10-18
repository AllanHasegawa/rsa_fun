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

	mpz_class r;
	for_each (message.begin(), message.end(),[&](uint8_t m) {
		mpz_class M{m};
		rf::crypt(M, public_key, r);
		string t(r.get_str(16));
		crypt_message.push_back(t);
	});


	ofstream ofs(cryptfile);
	cereal::BinaryOutputArchive boa(ofs);
	boa(crypt_message);


	return 0;
}
