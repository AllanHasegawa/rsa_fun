#include <string>
#include <exception>
#include "rsafun.hpp"
#include "rfutils.hpp"

int main(int argc, char* argv[]) {
	using namespace std;

	string usage = "./rsagen <key name> <precision_bits> "
		"<fermat passes> <n threads>";

	if (argc < 5) {
		cout << usage << endl;
		return 0;
	}

	string filename = argv[1];
	string precision_str = argv[2]; 
	string passes_str = argv[3];
	string threads_str = argv[4];

	
	int precision_bits = stoi(precision_str);
	int threads = stoi(threads_str);
	int passes;
	try {
		precision_bits = stoi(precision_str);
		threads = stoi(threads_str);
		passes = stoi(passes_str);
	} catch (exception& e) {
		cout << "Error: Wrong number entered..." << endl;
		return 0;
	}
	auto keys = rf::gen_key_pairs(precision_bits, passes, threads);
	rf::save_keys(keys, filename);

	return 0;
}
