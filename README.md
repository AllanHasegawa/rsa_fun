# rsa_fun

rsa_fun is an educational project developed to investigate and study the RSA algorithm. It's an implementation
of the RSA algorithm described in ("Algorithms", Dasgupta; Papadimitriou;
Vazirani, 2006). The project creates the following programs:

  * Key pair generator (with any number os bits)
  * cipher (uses public key)
  * decipher (uses private key)
  * decipher (uses public key - factor integer number with brute force)


It is *NOT* an optimal solution. High performance functions already implemented
and available in open-source libraries were not used, apart from basic
arithmetics with GMP.

## Dependency

 * GNU MP 5.1.3
 * clang 3.3
 * cmake 2.8.10
