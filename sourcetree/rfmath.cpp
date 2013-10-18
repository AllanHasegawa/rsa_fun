#include <mutex>
#include <future>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>
#include "gmpxx.h"
#include "gmp.h"
#include "rfmath.hpp"


void rf::binary_gcd(const mpz_class& a, const mpz_class& b, mpz_class& r)
{
	if (a == 0) r = b;
	if (b == 0) r = a;

	
	mpz_class x = a;
	mpz_class y = b;

	auto xp = x.get_mpz_t();
	auto yp = y.get_mpz_t();

	mpz_class t;
	auto tp = t.get_mpz_t();
	mpz_ior(tp, xp, yp);

	int ctz_xy = mpz_scan1(tp, 0);
	int ctz_x = mpz_scan1(xp, 0);
	int ctz_y;
	mpz_tdiv_q_2exp(xp, xp, ctz_x);
	while(true) {
		ctz_y = mpz_scan1(yp, 0);
		mpz_tdiv_q_2exp(yp, yp, ctz_y);

		if (x == y) break;
		if (x > y) std::swap(x, y);
		if (x == 1) break;

		y -= x;
	}

	auto rp = r.get_mpz_t();
	mpz_mul_2exp(rp, xp, ctz_xy);
}

void rf::random_coprime(const mpz_class& n, const int precision_bits,
			const int threads, mpz_class& coprime)
{
	if (precision_bits < 2)
		throw std::invalid_argument("random_coprime:"
				" Precision bits must be >=2");
	if (n < 2)
		throw std::invalid_argument("random_coprime: n must be >=2");
	if (threads < 1)
		throw std::invalid_argument("random_coprime: "
				"threads must be >=1");

	using namespace std;
	atomic_bool found_coprime;
	found_coprime = false;

	vector<future<void>> jobs;
	mutex copy;
	for (int i{}; i < threads; ++i) {
		jobs.push_back(async(launch::async, [&]() {
			gmp_randstate_t rs;
			gmp_randinit_mt(rs);
			gmp_randseed_ui(rs, std::random_device{}());

			mpz_class r;
			auto rp = r.get_mpz_t();
			int c{};	
			mpz_class t;

			while(true) {
				do {
					mpz_urandomb(rp, rs, precision_bits); 
				} while (r < 2);
				binary_gcd(r, n, t);
				// only coprimes should try to copy
				// to shared variable,
				// and when they do, one at a time ;)
				if (t == 1) {
					std::lock_guard<mutex> l(copy);
					found_coprime = true;
					coprime = r;
					break;
				}
				++c;
				if (c%10 == 0) {
					if (found_coprime) break;
				}
			}
			gmp_randclear(rs);
		}));
	}

}

void rf::random_prime(const int precision_bits, const int fermat_passes,
			const int threads, mpz_class& prime)
{
	if (precision_bits < 2)
		throw std::invalid_argument("Precision bits must be >=2");

	gmp_randstate_t rs;
	gmp_randinit_mt(rs);
	gmp_randseed_ui(rs, std::random_device{}());

	auto primep = prime.get_mpz_t();
	while(true) {
		mpz_urandomb(primep, rs, precision_bits); 
		if (prime < 2) continue;
		if (mpz_even_p(primep)) prime += 1;
		if (is_prime_fermat(prime,
			fermat_passes, threads)) break;
	}
	gmp_randclear(rs);
}

void  __is_prime_fermat_parallel(const mpz_class& n, const int passes,
		std::atomic_bool& r) {
	using namespace std;

	auto np = n.get_mpz_t();
	mpz_class n1{n-1};
	auto n1p = n1.get_mpz_t();
	
	gmp_randstate_t rs;
	gmp_randinit_mt(rs);
	gmp_randseed_ui(rs, std::random_device{}());

	mpz_class ri;
	auto rip = ri.get_mpz_t();

	for (int i{}; i < passes; ++i) {
		if (r) {
			do {
				mpz_urandomm(rip, rs, np);
			} while (ri < 1);

			mpz_powm(rip, rip, n1p, np);
			if (ri != 1) {
				r = false;
				break;
				// composite for sure
			}
		} else {
			break;
		}
	}
	gmp_randclear(rs);
	// probable prime
}

bool rf::is_prime_fermat(const mpz_class& n, const int passes,
						const int threads)
{
	if (threads < 1) throw std::invalid_argument("threads must be >=1");
	if (passes < 1) throw std::invalid_argument("passes must be >0");
	if (n <= 0) return false;
	if (n <= 3) return true;

	using namespace std;

	// sequential part
	if (threads == 1) {
		auto np = n.get_mpz_t();
		mpz_class n1{n-1};
		auto n1p = n1.get_mpz_t();
		
		gmp_randstate_t rs;
		gmp_randinit_mt(rs);
		gmp_randseed_ui(rs, std::random_device{}());
	
		mpz_class ri;
		auto rip = ri.get_mpz_t();
	
		for (int i{}; i < passes; ++i) {
			do {
				mpz_urandomm(rip, rs, np);
			} while (ri < 1);
	
			mpz_powm(rip, rip, n1p, np);
			if (ri != 1) {
				gmp_randclear(rs);
				return false; // composite for sure
			}
		}
		gmp_randclear(rs);
		return true; // probable prime
	// parallel part
	} else {
		atomic_bool r;
		r = true;
		vector<future<void>> jobs;	
		for (int i{}; i < threads; ++i) {
			jobs.push_back(
				async(launch::async, [&]() {
					return __is_prime_fermat_parallel(n,
						passes/threads, r);
			}));
		}

		for (auto i = jobs.begin(); i != jobs.end(); ++i) {
			i->wait();
		}

		return r;
	}
}

bool rf::is_prime_in_blocks(const mpz_class& n)
{
	if (is_obvious_composite(n)) return false;
	if (n <= 3) return true;
	if (n < PrimeBuffer<mpz_class>::value.back())
		return PrimeBuffer<mpz_class>::search_prime(n);
	if (n < 30*30) return is_prime_naive(n);

	// 30k + {1, 7, 11, 13, 17, 19, 23, 29}
	// 43 = 30*1 + 13
	// 757 = 30*25 + 7
	auto np = n.get_mpz_t();
	mpz_class t;
	auto tp = t.get_mpz_t();
	
	mpz_class i{30};
	auto ip = i.get_mpz_t();
	mpz_class ns;
	auto nsp = ns.get_mpz_t();
	mpz_sqrt(nsp, np);
	for (; i <= ns; i += 30) {
		mpz_add_ui(tp, ip, 1);
		if (mpz_divisible_p(np, tp)) return false;
		mpz_add_ui(tp, ip, 7);
		if (mpz_divisible_p(np, tp)) return false;
		mpz_add_ui(tp, ip, 11);
		if (mpz_divisible_p(np, tp)) return false;
		mpz_add_ui(tp, ip, 13);
		if (mpz_divisible_p(np, tp)) return false;
		mpz_add_ui(tp, ip, 17);
		if (mpz_divisible_p(np, tp)) return false;
		mpz_add_ui(tp, ip, 19);
		if (mpz_divisible_p(np, tp)) return false;
		mpz_add_ui(tp, ip, 23);
		if (mpz_divisible_p(np, tp)) return false;
		mpz_add_ui(tp, ip, 29);
		if (mpz_divisible_p(np, tp)) return false;
	}

	return true;
}

void rf::find_2_prime_factors_naive(const mpz_class& N,
	mpz_class& x, mpz_class& y)
{
	mpz_class t{2};
	auto Np = N.get_mpz_t();
	auto tp = t.get_mpz_t();
	if (mpz_divisible_p(Np,tp)) return; 
	t += 1;
	while (t < N) {
		// if (N%t) == 0
		if (mpz_divisible_p(Np, tp)) {
			x = t;
			y = N/t;
			return;
		}
		t += 2;
	}

	throw std::invalid_argument("Failed to find prime factor");
}

void rf::find_2_prime_factors_blocks(const mpz_class& n, const int threads,
				mpz_class& x, mpz_class& y)
{
	if (n <= 6) throw std::invalid_argument("n must be >= 6");
	const auto& PB = PrimeBuffer<mpz_class>::value;
	PrimeBuffer<mpz_class>{100};
	using namespace std;

	if (PB.back() < 210) 
		throw std::runtime_error("PrimeBuffer not enough!");

	auto np = n.get_mpz_t();
	for (auto i = PB.begin(); *i < 210; ++i) {
		auto ip = i->get_mpz_t();
		if (mpz_divisible_p(np, ip)) {
			x = *i;
			y = n/x;
			return;
		}
	}

	using namespace std;

	mutex copy_mutex;
	atomic_bool found_factor;
	found_factor = false;
	vector<future<void>> jobs;
	for (int i{}; i < threads; ++i) {
		jobs.push_back(async(launch::async,[&](int ID){

		mpz_class t;
		auto tp = t.get_mpz_t();
		// thread 1 starts at 210
		// "      2 "         210*2
		// ...
		mpz_class i{210*(ID+1)};
		auto ip = i.get_mpz_t();
		

		const mpz_class n1{1};
		const auto n1p = n1.get_mpz_t();
		const mpz_class n11{11};
		const auto n11p = n11.get_mpz_t();
		const mpz_class n13{13};
		const auto n13p = n13.get_mpz_t();
		const mpz_class n17{17};
		const auto n17p = n17.get_mpz_t();
		const mpz_class n19{19};
		const auto n19p = n19.get_mpz_t();
		const mpz_class n23{23};
		const auto n23p = n23.get_mpz_t();
		const mpz_class n29{29};
		const auto n29p = n29.get_mpz_t();
		const mpz_class n31{31};
		const auto n31p = n31.get_mpz_t();
		const mpz_class n37{37};
		const auto n37p = n37.get_mpz_t();
		const mpz_class n41{41};
		const auto n41p = n41.get_mpz_t();
		const mpz_class n43{43};
		const auto n43p = n43.get_mpz_t();
		const mpz_class n47{47};
		const auto n47p = n47.get_mpz_t();
		const mpz_class n53{53};
		const auto n53p = n53.get_mpz_t();
		const mpz_class n59{59};
		const auto n59p = n59.get_mpz_t();
		const mpz_class n61{61};
		const auto n61p = n61.get_mpz_t();
		const mpz_class n67{67};
		const auto n67p = n67.get_mpz_t();
		const mpz_class n71{71};
		const auto n71p = n71.get_mpz_t();
		const mpz_class n73{73};
		const auto n73p = n73.get_mpz_t();
		const mpz_class n79{79};
		const auto n79p = n79.get_mpz_t();
		const mpz_class n83{83};
		const auto n83p = n83.get_mpz_t();
		const mpz_class n89{89};
		const auto n89p = n89.get_mpz_t();
		const mpz_class n97{97};
		const auto n97p = n97.get_mpz_t();
		const mpz_class n101{101};
		const auto n101p = n101.get_mpz_t();
		const mpz_class n103{103};
		const auto n103p = n103.get_mpz_t();
		const mpz_class n107{107};
		const auto n107p = n107.get_mpz_t();
		const mpz_class n109{109};
		const auto n109p = n109.get_mpz_t();
		const mpz_class n113{113};
		const auto n113p = n113.get_mpz_t();
		const mpz_class n127{127};
		const auto n127p = n127.get_mpz_t();
		const mpz_class n131{131};
		const auto n131p = n131.get_mpz_t();
		const mpz_class n137{137};
		const auto n137p = n137.get_mpz_t();
		const mpz_class n139{139};
		const auto n139p = n139.get_mpz_t();
		const mpz_class n149{149};
		const auto n149p = n149.get_mpz_t();
		const mpz_class n151{151};
		const auto n151p = n151.get_mpz_t();
		const mpz_class n157{157};
		const auto n157p = n157.get_mpz_t();
		const mpz_class n163{163};
		const auto n163p = n163.get_mpz_t();
		const mpz_class n167{167};
		const auto n167p = n167.get_mpz_t();
		const mpz_class n173{173};
		const auto n173p = n173.get_mpz_t();
		const mpz_class n179{179};
		const auto n179p = n179.get_mpz_t();
		const mpz_class n181{181};
		const auto n181p = n181.get_mpz_t();
		const mpz_class n191{191};
		const auto n191p = n191.get_mpz_t();
		const mpz_class n193{193};
		const auto n193p = n193.get_mpz_t();
		const mpz_class n197{197};
		const auto n197p = n197.get_mpz_t();
		const mpz_class n199{199};
		const auto n199p = n199.get_mpz_t();

		const mpz_class ns = n/2;
		//auto nsp = ns.get_mpz_t();
		//mpz_sqrt(nsp, np);
		unsigned c{};
		// every iteration jumps the others threads works
		for (; i <= ns; i += 210*threads) {
			++c;
			// lets not interrupt others threads
			// all the time. Be Polite!
			if (c%100 == 0) {
				if (found_factor) {
					return;
				}
			}
			mpz_add(tp, ip, n1p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n11p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n13p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n17p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n19p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n23p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n29p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n31p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n37p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n41p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n43p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n47p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n53p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n59p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n61p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n67p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n71p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n73p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n79p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n83p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n89p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n97p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n101p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n103p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n107p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n109p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n113p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n127p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n131p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n137p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n139p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n149p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n151p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n157p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n163p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n167p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n173p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n179p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n181p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n191p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n193p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n197p);
			if (mpz_divisible_p(np, tp)) break;
			mpz_add(tp, ip, n199p);
			if (mpz_divisible_p(np, tp)) break;
		}
		if (mpz_divisible_p(np, tp)) {
			lock_guard<mutex> l(copy_mutex);
			found_factor = true;
			x = t;
			y = n/x;
		}
	}, i));
	}

	for (auto i = jobs.begin(); i != jobs.end(); ++i)
		i->wait();

}

bool rf::is_obvious_composite(const mpz_class& n)
{
	if (n <= 0) return true;
	if (n <= 3) return false;
	auto np = n.get_mpz_t();

	// if (n%2==0) composite
	if (mpz_even_p(np)) return true;
	return false;
}

/**
 * aux function to find 'd' and 's' from
 * n - 1 = d*2^s
 * */
void __find_s_d(const mpz_class& n, mpz_class& d, long& s)
{
	auto dp = d.get_mpz_t();
	mpz_class n1 = n-1;
	auto n1p = n1.get_mpz_t();
	s = mpz_scan1(n1p, 0); // divide number by 2 until find odd
	mpz_tdiv_q_2exp(dp, n1p, s); // d == (int)n / (2^s)
}

bool rf::is_prime_miller_rabin(const mpz_class& n, const long B)
{
	if (is_obvious_composite(n)) return false;
	if (B < 2) throw std::invalid_argument("B must be >1");
	
	// n-1 = (2^s)*d
	mpz_class d;
	auto dp = d.get_mpz_t();
	long s;
	__find_s_d(n, d, s);
	
	// if B^d mod n == 1 -> spsp(B)
	mpz_class b{B};
	mpz_class x;
	auto bp = b.get_mpz_t();
	auto xp = x.get_mpz_t();
	auto np = n.get_mpz_t();
	mpz_powm(xp, bp, dp, np);
	if (x == 1) return true;

	// of if B^(d*(2^r)) mod n == -1 -> spsp(B)
	//
	// B^(d*2) = (B^d) * (B^d)
	// B^(d*2^2) = B^(d*4) = (B^d*2) * (B^d*2)
	// ...
	for (long r{}; r < s; ++r) {
		mpz_mul(xp, xp, xp); // B^(d*2^r) = (B^(d*2^(r-1)))^2
		mpz_mod(xp, xp, np); // B^(d*2^r) mod n
		if (x == -1) return true;
	}
	
	// composite
	return false;
}


bool rf::is_prime_strong_lucas_selfridge(const mpz_class& n)
{
	throw std::runtime_error("Not implemented");
	return true;
}

bool rf::is_prime_BPSW(const mpz_class& n)
{
	throw std::runtime_error("Not implemented");
	return true;
}

