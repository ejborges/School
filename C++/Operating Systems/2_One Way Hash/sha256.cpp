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

#include <iostream>
#include <fstream>
#include "sha256.h"

using namespace std;

sha256::sha256(){
    digest = "----------------------------------------------------------------";
}

// Calculates the SHA-256 hash for a file
// Returns 0 if completed successfully
// Returns 1 if an error occurred
int sha256::generate(char* filename){

    ifstream file;
    file.open(filename, ifstream::binary);

    if(file.fail()) return 1;

    // get file size
    unsigned __int32 file_byte_size;    // can handle files up to 4GB in size
    unsigned __int64 file_bit_size;
    unsigned __int32 remaining_bytes;
    file.seekg(0, file.end);
    file_byte_size = (unsigned __int32)file.tellg();
    remaining_bytes = file_byte_size;
    file_bit_size = file_byte_size << 3;
    file.seekg(0, file.beg);
#ifdef DEBUG
    cout << "Size of " << filename << ": " << file_byte_size << " bytes (" << file_bit_size << " bits)" << endl;
#endif

    // calculate number of 512-bit chunks we'll need ------------------------------------
    unsigned __int32 chunks;
    unsigned __int32 remaining_chunks;
    chunks = file_byte_size/64;
    // if some bytes at the end couldn't fill in a chunk
    if(file_byte_size % 64){
        chunks++; // we need another chunk for this data
        // if we don't have room in this chunk to append '1' and '0' bits and 64-bit message length
        if((file_byte_size % 64) >= 56) chunks++; // we'll need another chunk as well
    }
    // else all chunks are perfectly filled and we'll need another chunk for the
    // appended '1' and '0' bits and 64-bit message length
    else chunks++;
    remaining_chunks = chunks;
#ifdef DEBUG
    cout << "Number of 512-bit chunks: " << chunks << endl;
#endif
    // calculate number of 512-bit chunks we'll need ------------------------------------

    char buffer[64]; // 512-bit buffer
    bool included_one_bit = false;// have we appended the bit '1' to the chunk? (SHS 5.1.1)

    // Reset for next file (SHS 6.2.22)
    H[0] = h0; H[1] = h1; H[2] = h2; H[3] = h3; H[4] = h4; H[5] = h5; H[6] = h6; H[7] = h7;

    while(remaining_chunks){

        // Pre-processing of chunk (SHS 5.0.0) ----------------------------------------------------

        // fill the 512-bit buffer
        for(int i = 0; i < 64; i++){
            if(remaining_bytes) {
                file.get(buffer[i]);
                remaining_bytes--;
            }
            else if(i == 56 && included_one_bit){
                // add the 64-bit file size (in bits) length (SHS 5.1.1)
                for(int a=56; a >= 0; a -= 8, i++) buffer[i] = (char)((file_bit_size >> a) & 0xff);
            }
            else if(!included_one_bit) {
                buffer[i] = (char)0x80; // (SHS 5.1.1)
                included_one_bit = true;
            }
            else buffer[i] = 0x00; // (SHS 5.1.1)
        }

#ifdef HYPER_DEBUG
        cout << "Chunk[" << chunks - remaining_chunks << "] = 0x";
        for(int i = 0; i < 64; i++) {
            cout << hex_chars[(buffer[i] & 0xf0) >> 4];
            cout << hex_chars[(buffer[i] & 0x0f) >> 0];
        }
        cout << endl;
        //for(int i = 56; i < 64; i++) cout << "buffer[" << i << "] = " << (int)buffer[i] << endl;
#endif

        // Processing of chunk: (SHS 6.2.2) -------------------------------------------------------
        unsigned __int32 w[64];
        for(int i = 0; i < 64; i++) w[i] = 0x0;

        // Prepare the message schedule 'w' (SHS 6.2.2.1)
        for(int i = 0, j = 0; i < 16; i++, j+=4) {
            w[i] = (unsigned __int32)((0xff & buffer[j]) << 24) +
                    ((0xff & buffer[j+1]) << 16) +
                    ((0xff & buffer[j+2]) << 8) +
                    ((0xff & buffer[j+3]) << 0);
        }
        for(int i = 16; i < 64; i++) w[i] = w[i-16] + sigma0(w[i-15]) + w[i-7] + sigma1(w[i-2]);

#ifdef HYPER_DEBUG
        for(int i = 0; i < 64; i++) {
            cout << "w[" << i << "] = ";
            cout << hex_chars[(w[i] & 0xf0000000) >> 28];
            cout << hex_chars[(w[i] & 0x0f000000) >> 24];
            cout << hex_chars[(w[i] & 0x00f00000) >> 20];
            cout << hex_chars[(w[i] & 0x000f0000) >> 16];
            cout << hex_chars[(w[i] & 0x0000f000) >> 12];
            cout << hex_chars[(w[i] & 0x00000f00) >> 8];
            cout << hex_chars[(w[i] & 0x000000f0) >> 4];
            cout << hex_chars[(w[i] & 0x0000000f) >> 0];
            cout << endl;
        }
#endif

        // Initialize working variables to current hash value (SHS 6.2.2.2)
        unsigned __int32 a = H[0];
        unsigned __int32 b = H[1];
        unsigned __int32 c = H[2];
        unsigned __int32 d = H[3];
        unsigned __int32 e = H[4];
        unsigned __int32 f = H[5];
        unsigned __int32 g = H[6];
        unsigned __int32 h = H[7];

        // Compression function main loop (SHS 6.2.2.3)
        unsigned __int32 temp1;
        unsigned __int32 temp2;
        for(int i = 0; i < 64; i++){
            temp1 = h + SIGMA1(e) + ch(e,f,g) + k[i] + w[i];
            temp2 = SIGMA0(a) + maj(a,b,c);

            h = g;
            g = f;
            f = e;
            e = (d + temp1) & 0xffffffff;
            d = c;
            c = b;
            b = a;
            a = (temp1 + temp2) & 0xffffffff;
        }

        // Add the compressed chunk to the current hash value (note 'addition modulo 2^32') (SHS 6.2.2.4)
        H[0] = (H[0] + a) & 0xffffffff;
        H[1] = (H[1] + b) & 0xffffffff;
        H[2] = (H[2] + c) & 0xffffffff;
        H[3] = (H[3] + d) & 0xffffffff;
        H[4] = (H[4] + e) & 0xffffffff;
        H[5] = (H[5] + f) & 0xffffffff;
        H[6] = (H[6] + g) & 0xffffffff;
        H[7] = (H[7] + h) & 0xffffffff;

        remaining_chunks--;
    }

    digest = "";
    for(int i = 0; i < 8; i++){
        digest += hex_chars[(H[i] & 0xf0000000) >> 28];
        digest += hex_chars[(H[i] & 0x0f000000) >> 24];
        digest += hex_chars[(H[i] & 0x00f00000) >> 20];
        digest += hex_chars[(H[i] & 0x000f0000) >> 16];
        digest += hex_chars[(H[i] & 0x0000f000) >> 12];
        digest += hex_chars[(H[i] & 0x00000f00) >> 8];
        digest += hex_chars[(H[i] & 0x000000f0) >> 4];
        digest += hex_chars[(H[i] & 0x0000000f) >> 0];
    }

    file.close();

    return 0;
}

// Rotates right (circular right shift) value x by n positions (SHS 3.2.4)
unsigned __int32 sha256::rotr(unsigned __int32 x, unsigned int n) {
    return (x >> n) | (x << (32 - n));
}

// Logical functions (SHS 4.1.2) --------------------------------------------------------
unsigned __int32 sha256::SIGMA0(unsigned __int32 x) {
    return sha256::rotr(x, 2) ^ sha256::rotr(x, 13) ^ sha256::rotr(x, 22);
}

unsigned __int32 sha256::SIGMA1(unsigned __int32 x) {
    return sha256::rotr(x, 6) ^ sha256::rotr(x, 11) ^ sha256::rotr(x, 25);
}

unsigned __int32 sha256::sigma0(unsigned __int32 x) {
    return sha256::rotr(x, 7) ^ sha256::rotr(x, 18) ^ (x >> 3);
}

unsigned __int32 sha256::sigma1(unsigned __int32 x) {
    return sha256::rotr(x, 17) ^ sha256::rotr(x, 19) ^ (x >> 10);
}

// Choose: the x input chooses if the output is from y or z
unsigned __int32 sha256::ch(unsigned __int32 x, unsigned __int32 y, unsigned __int32 z) {
    return (x & y) ^ (~x & z);
}

// Majority: the result is set according to the majority of the 3 inputs
unsigned __int32 sha256::maj(unsigned __int32 x, unsigned __int32 y, unsigned __int32 z) {
    return (x & y) ^ (x & z) ^ (y & z);
}
// Logical functions (SHS 4.1.2) --------------------------------------------------------