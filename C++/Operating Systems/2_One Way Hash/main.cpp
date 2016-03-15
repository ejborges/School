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
/* In MinGW pthread.h v2.9.1-1 had to amend
 *     #if !defined(__struct_timespec_defined) to line 318 and
 *     #endif to line 326
 * in order to navigate around build error: redefinition of 'struct timespec'
 * defined in pthread.h:321 and parts\time.h:105
 * In parts\time.h:91 amended `&& ! _TIMESPEC_DEFINED` to end of line
*/
#include <pthread.h>
#include "sha256.h"

#define USE_THREADS


using namespace std;


bool can_print = true;  // makes sure no two threads are printing to console at same time
void* hash_file(void* filename_ptr);


int main(int argc, char* argv[]) {

    #ifdef HYPER_DEBUG
    cout << "\n\tThere are " << argc << " arguments." << endl;
    for(int i = 0; i < argc; i++) cout << "\t\targv[" << i << "] = " << argv[i] << endl;
    #endif

    if(argc < 2) {
        cout << "\n\tNo arguments to execute." << endl;
        return 0;
    }

    #ifdef USE_THREADS

    pthread_t thread[argc - 1];
    int ret_val;

    for(int arg_id = 1, hash_id = 0; arg_id < argc; arg_id++, hash_id++){

        ret_val = pthread_create(&thread[hash_id], NULL, hash_file, (void*)argv[arg_id]);
        if(ret_val) {
            // The double while is in case there are more than one thread waiting to print
            // to the console. It is likely that one thread will get stuck in one of the
            // whiles when the other thread moves to set can_print to false.
            while(!can_print){}
            while(!can_print){}
            can_print = false;
            cout << "\n\tError creating thread:" << hash_id << endl;
            can_print = true;
        }
    }
    pthread_exit(NULL);

    #else

    sha256 hash;
    for(int i = 1; i < argc; i++){
        if(!hash.generate(argv[i])) cout << "\n\tFile: " << argv[i] << endl << "\tSHA2: " << hash.digest << endl;
        else cout << "\n\tError opening file " << argv[i] << endl;
    }
    cout << endl;
    return 0;

    #endif
}

void* hash_file(void* filename_ptr){
    char* filename;
    filename = (char*)filename_ptr;

    sha256 hash;
    int ret_val;
    ret_val = hash.generate(filename);
    if(!ret_val) {
        // The double while is in case there are more than one thread waiting to print
        // to the console. It is likely that one thread will get stuck in one of the
        // whiles when the other thread moves to set can_print to false.
        while(!can_print){}
        while(!can_print){}
        can_print = false;
        cout << "\n\tFile: " << filename << endl << "\tSHA2: " << hash.digest << endl;
        can_print = true;
    }
    else {
        while(!can_print){}
        while(!can_print){}
        can_print = false;
        cout << "\n\tError opening file " << filename << endl;
        can_print = true;
    }
    pthread_exit(NULL);
}