/* The Toymaker: a command-line utility designed for parsing and editing linux ELF binaries 
 *
 * Author (Very Important, Must Include in Source): Matt A. Ferrero.
 * 
 * WARNING: This software is designed to be a TEST VIRUS alongisde it's normal function of
 * handling ELF files. IF MODIFIED OR USED INCORRECTLY THIS SOFTWARE _CAN_ AND _WILL_ DESTROY
 * COMPUTER SYSTEMS. I repeat: this software is SOLEY used for testing purposes. No malicious
 * intent is included with this software. Modify and use at your own risk. You have been warned.
 * 
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char *argv[]) {

    char ch;
    
    while ( (ch = getopt(argc, argv, "l:i:")) != EOF) {
        switch(ch) {
            case 'l': return 0; break;
            case 'i': return 0; break;
            default: fprintf("Incorrect argument.\n", stderr);
        }
    }
return 0;
}



