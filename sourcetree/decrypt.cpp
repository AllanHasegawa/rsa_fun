
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

	string usage = "./rsadecrypt <private key file> <crypted file>"
		" <decrypted file>";

	if (argc < 4) {
		cout << usage << endl;
		return 0;
	}

	string keyfile = argv[1];
	string cryptfile = argv[2];
	string decryptfile  = argv[3];

	rf::RSAPrivateKey private_key = rf::open_private_key(keyfile);

	vector<string> crypt_message;

	ifstream ifs(cryptfile);
	cereal::BinaryInputArchive bia(ifs);
	bia(crypt_message);

	mpz_class r;
	vector<uint8_t> message;
	for_each (crypt_message.begin(), crypt_message.end(),[&](string&  m) {
		mpz_class C{m, 16};
		rf::decrypt(C, private_key, r);
		message.push_back(r.get_ui());
	});

	rf::bytes_to_file(decryptfile, message);

	return 0;
}
