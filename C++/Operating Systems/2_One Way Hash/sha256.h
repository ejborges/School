/* This is an implementation of the SHA-256 hashing algorithm
 *
 * ----------------------------------------------------------
 *
 *      Author: Emilio Borges
 *      Developed: March 2016
 *
 *      The University of Toledo
 *      EECS 3540: Operating Systems and Systems Programming
 *
 *      Project documentation can be found at:
 *      https://drive.google.com/open?id=0B4CF__kbczDjdzJqOG5MdEtpVEU
 *
 *      Secure Hash Standard (SHS): http://csrc.nist.gov/publications/fips/fips180-4/fips-180-4.pdf
 *      SHA-256 wiki: https://en.wikipedia.org/wiki/SHA-2
 *
 *
 *      HOW TO RUN ------------------------------------------
 *
 *          In the command line, after typing this executable's
 *          name, type the name of the files you wish to hash.
 *          Multiple files can be hashed at the same time if
 *          multiple arguments are passed in separated by a
 *          space.
 *
 *          Example of arguments:
 *
 *              One file will be hashed with this argument
 *                  test.txt
 *
 *              Multiple files will be hashed with these arguments
 *                  test1.txt test2.txt test3.txt
 *
 *      HOW IT WORKS ----------------------------------------
 *
 *          If threads are used, a thread will be created for
 *          each filename passed in as an argument. Each thread
 *          created will then execute the hash_file() method below.
 *          There, an instance of the sha256 class will be created
 *          and used to generate the SHA-256 hash for the file name
 *          passed in with the thread.
 *
 *          If no threads are used, each filename passed in as an
 *          argument will be executed in the order they were passed
 *          in one after the other. Only one instance of the sha256
 *          class will be created and used to generate the SHA-256
 *          hash for each file.
 *
 *          The sha256.generate() method first calculates the size
 *          of the file it will be hashing. Using this size value,
 *          it then calculates how many 512-bit segments (aka chunks)
 *          will be needed in order to fully hash the file. For each
 *          segment, read in a byte at a time from the file and put
 *          it in the segment. Once the segment fills up, run it
 *          through the SHA-256 algorithm defined by the standard
 *          above. Right after all the file's bytes have been read
 *          in, the next byte in the segment will be 0x80 which
 *          contains the '1' bit as defined in the standard. Next,
 *          check if there is enough room in the segment to include
 *          the 64-bit length value and include it if there is room
 *          or wait until the next segment to include it if there is
 *          no room. But, before including the 64-bit length value
 *          and after including the '1' bit, add as many zeros as
 *          possible even if they overrun to the next segment. Once
 *          every segment has been put through the SHA-256 algorithm,
 *          digest the resulting hashes and print the resulting 256
 *          bit value in hex to the console.
 *
 */

#ifndef SHA256_H
#define SHA256_H

//uncomment for unix/linux
//comment out for dos/windows with MinGW
#ifndef __int32
#define __int32 int
#define __int64 long
#endif

//#define DEBUG         // uncomment for essential debug info
//#define HYPER_DEBUG   // uncomment for all debug info

#include <string>

class sha256 {

public:
    sha256();
    int generate(char* filename);
    std::string digest;

private:
    //Initialize hash values (SHS 5.3.1)
    //(first 32 bits of the fractional parts of the square roots of the first 8 primes 2..19)
    const unsigned __int32 h0 = 0x6a09e667;
    const unsigned __int32 h1 = 0xbb67ae85;
    const unsigned __int32 h2 = 0x3c6ef372;
    const unsigned __int32 h3 = 0xa54ff53a;
    const unsigned __int32 h4 = 0x510e527f;
    const unsigned __int32 h5 = 0x9b05688c;
    const unsigned __int32 h6 = 0x1f83d9ab;
    const unsigned __int32 h7 = 0x5be0cd19;
    unsigned __int32 H[8] = {h0, h1, h2, h3, h4, h5, h6, h7};

    //Initialize array of round constants (SHS 4.2.2)
    //(first 32 bits of the fractional parts of the cube roots of the first 64 primes 2..311)
    const unsigned __int32 k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    const char hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    // Rotates right (SHS 3.2.4)
    unsigned __int32 rotr(unsigned __int32 x, unsigned int n);

    // Logical functions (SHS 4.1.2)
    unsigned __int32 SIGMA0(unsigned __int32 x);
    unsigned __int32 SIGMA1(unsigned __int32 x);
    unsigned __int32 sigma0(unsigned __int32 x);
    unsigned __int32 sigma1(unsigned __int32 x);
    unsigned __int32 ch(unsigned __int32 x, unsigned __int32 y, unsigned __int32 z);
    unsigned __int32 maj(unsigned __int32 x, unsigned __int32 y, unsigned __int32 z);

};

#endif //SHA256_H
