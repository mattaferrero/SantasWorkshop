/* 
 * The Toymaker: a command-line utility designed for parsing and editing linux ELF binaries 
 *
 * Author: Matt A. Ferrero.
 * 
 * WARNING: This software is designed to be a TEST VIRUS alongisde it's normal function of
 * handling ELF files. IF MODIFIED OR USED INCORRECTLY THIS SOFTWARE _CAN_ AND _WILL_ DESTROY
 * COMPUTER SYSTEMS. I repeat: this software is SOLEY used for testing purposes. No malicious
 * intent is included with this software. Modify and use at your own risk. You have been warned.
 * 
 * Program Execution Flow:
 * 
 * 1) TO DO: COMMAND LINE ARGS FOR TARGET BINARIES
 * 
 * 2) main() will load the target binary to be modified into memory with mmap(), and call 
 * ehdr_sane_check() to validate the magic number and object class.
 * 
 * 3) ehdr_sane_check() will print out basic header information similar to objdump -h, then 
 * return to main().
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <elf.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_ARGS        2

int ehdr_sane_check(Elf64_Ehdr *elfhdr);

int main(int argc, char *argv[]) {  

    int                 fd = 0; /* open()'s return val */
    int                 status = 0; /* fstat()'s return val */
   
    struct  stat        finfo_buf = {0}; /* Used for mmap() values */

    Elf64_Ehdr          *hdr64 = NULL; /* efl.h provides us with aligned structs to use */
    Elf64_Phdr          *phdr64 = NULL;
    Elf64_Shdr          *shdr64 = NULL;
    Elf64_Sym           *sym64 = NULL;
    Elf64_Rel           *rel64 = NULL;
    Elf64_Rela          *rela64 = NULL; 

    void                *mptr = NULL;

    /* To do: implement args/flags later, do more specific error checking */
    if (argc != MAX_ARGS) {
        fprintf(stderr, "Error: exceeded maximum number of arguments. Please man toymaker.\n");
        return 1;
    }
    
    /* mmap() requires a file descriptor, so we are using open() instead for convenience instead of fopen(). */
    fd = open(argv[1], O_RDWR); /* O_RDWR read/write. other flags are read only/write only. */
    if (fd == -1) {
        fprintf(stderr, "Error: Could not open file.\n");
        return 1;
    }

    status = fstat(fd, &finfo_buf);
    if (status == -1) {
        fprintf(stderr, "Error: Could not grab file status.\n");
        
        if (close(fd) == -1) {
            fprintf(stderr, "Error: Could not close file descriptor after fstat() call.\n");
            return 1; /* Something has gone horribly, horribly wrong somehow (yes this is redundant, im aware). */
        }
        return 1;
    }

    /* 
     * We will load the file into memory with mmap(). Since mmap() does some behind the scenes magic and
     * doesn't actually load the ENTIRE file until we try accessing those parts of it, we don't care about
     * file size and can just do a sanity check for the ELF header contents before mucking around.
     */
    mptr = mmap(NULL, finfo_buf.st_size, PROT_EXEC, MAP_PRIVATE, fd, 0);
    if (mptr == MAP_FAILED) {
        fprintf(stderr, "Error: Mapping failed.\n");

        if (close(fd) == -1) {
            fprintf(stderr, "Error: Could not close file descriptor after mmap() call.\n");
            return 1;
        }
        return 1;
    }

    /* We can close the file descriptor without invaliding the mapping. */
    if (close(fd) == -1) {
        fprintf(stderr, "Error: Could not close file descriptor.\n");
        return 1;
    }

    /* 
     * We will dump each header into their respective structs, but first we will pass the
     * ELF header to ehdr_sane_check() since the other header locations are found here.
    */
    hdr64 = (Elf64_Ehdr *) mptr;

    if (ehdr_sane_check(hdr64) == 1) {
        fprintf(stderr, "Fatal Error: ELF Header Corrupted!\n");
        return 1;
    }
    
    if (munmap(mptr, finfo_buf.st_size) == -1) {
        fprintf(stderr, "Error: Could not unmap file.\n");
        return 1;
    }

return 0;
}

/* 
* Here we read the ELF header into a struct and perform sanity checks for each struct member.
* This is done because this program modifies binary files directly, and so the possibility of
* corrupting the target file is high. Relevant values to this program from the target binary 
* are then printed out similar to readelf -h. Returns 1 if any header data is corrupted, or 0.
*/

int ehdr_sane_check(Elf64_Ehdr *elfhdr) {

    int ctr = 0;

    /* 
     * The first thing we check is e_ident[EI_NIDENT], a 16 byte array with basic file info. 
     * Some values are irrelevant to this program's execution (such as EI_VERSION) but we are
     * doing sanity checks anyway for the sake of completion. 
     */
    if (elfhdr->e_ident[EI_MAG0] != ELFMAG0 || elfhdr->e_ident[EI_MAG1] != ELFMAG1 || elfhdr->e_ident[EI_MAG2] != ELFMAG2 || elfhdr->e_ident[EI_MAG3] != ELFMAG3) {
        fprintf(stderr, "Error: ELF magic number is corrupted. Closing program.\n");
        return 1;
    }

    fprintf(stdout, "ELF Header:\n\tMagic Number: ");
     
    /* Printing out contents of e_ident[NE_NIDENT] which includes the magic number.*/
    while (ctr < EI_NIDENT) {
        fprintf(stdout, "%x ", elfhdr->e_ident[ctr]);
        ctr++;
    }

    switch (elfhdr->e_ident[EI_CLASS]) {
        case ELFCLASSNONE:      fprintf(stdout, "\n\tClass:\t\t\t\tInvalid Class\n"); break;
        case ELFCLASS32:        fprintf(stdout, "\n\tClass:\t\t\t\tELF 32\n"); break;
        case ELFCLASS64:        fprintf(stdout, "\n\tClass:\t\t\t\tELF 64\n"); break;
        default:                fprintf(stderr, "\n\tCorrupted EI_CLASS\n");
    }

    if (elfhdr->e_ident[EI_CLASS] != ELFCLASS64) {
        fprintf(stderr, "Error: ELF class is not 64-bit. Closing program.\n");
        return 1;
    }

    switch (elfhdr->e_ident[EI_DATA]) {
        case ELFDATANONE:       fprintf(stdout, "\tData Format:\t\t\tUnknown Format\n"); break;
        case ELFDATA2LSB:       fprintf(stdout, "\tData Format:\t\t\tTwo's complement, little endian\n"); break;
        case ELFDATA2MSB:       fprintf(stdout, "\tData Format:\t\t\tTwo's complement, big endian\n"); break;
        default:                fprintf(stderr, "\tCorrupted EI_DATA\n");
    }

    if (elfhdr->e_ident[EI_DATA] == ELFDATANONE) {
        fprintf(stderr, "Error: Unknown data format. Closing program.\n");
        return 1;
    }

    switch (elfhdr->e_ident[EI_VERSION]) {
        case EV_NONE:           fprintf(stdout, "\tELF Version:\t\t\tInvalid Version\n"); break;
        case EV_CURRENT:        fprintf(stdout, "\tELF Version:\t\t\t%x (current)\n", elfhdr->e_ident[EI_VERSION]); break;
        default:                fprintf(stderr, "\tCorrupted EI_VERSION\n");
    }

    if (elfhdr->e_ident[EI_VERSION] == EV_NONE) {
        fprintf(stderr, "Error: Invalid ELF version. Closing program.\n");
        return 1;
    }

    switch (elfhdr->e_type) {
        case ET_NONE:           fprintf(stdout, "\tObject type:\t\t\tUnknown\n"); break;
        case ET_REL:            fprintf(stdout, "\tObject type:\t\t\tRelocatable\n"); break;
        case ET_EXEC:           fprintf(stdout, "\tObject type:\t\t\tExecutable\n"); break;
        case ET_DYN:            fprintf(stdout, "\tObject type:\t\t\tShared Object\n"); break;
        case ET_CORE:           fprintf(stdout, "\tObject type:\t\t\tCore File\n"); break;
        default:                fprintf(stderr, "\tCorrupted e_type\n");
    }

    /* e_type defines the object file type. We want an executable. */
    if (elfhdr->e_type != ET_EXEC && elfhdr->e_type != ET_DYN) {
        fprintf(stderr, "Error: non-executable object type. Closing program.\n");
        return 1;
    }

    switch (elfhdr->e_version) {
        case EV_NONE:           fprintf(stdout, "\tFile Version:\t\t\tInvalid Version\n"); break;
        case EV_CURRENT:        fprintf(stdout, "\tFile Version:\t\t\t%x (current file version)\n", elfhdr->e_version); break;
        default:                fprintf(stderr, "\tCorrupted e_version\n");
    }

    /* e_version identifies file version. We are skipping e_machine, it's value does not matter. */
    if (elfhdr->e_version == EV_NONE) {
        fprintf(stderr, "Error: invalid file version. Closing program.\n");
        return 1;
    }

    fprintf(stdout, "\tEntry Point Address:\t\t%lx\n", elfhdr->e_entry);

    /* Sanity checking the important parts defining the other headers. */
    if (elfhdr->e_entry == 0) {
        fprintf(stderr, "Error: e_entry has no associated entry point. Closing program.\n");
        return 1;
    }

    fprintf(stdout, "\tStart of Program Headers:\t%ld (bytes from start of file)\n", elfhdr->e_phoff);

    if (elfhdr->e_phoff == 0) {
        fprintf(stderr, "Error: Program header table does not exist. Closing program.\n");
        return 1;
    }

    fprintf(stdout, "\tStart of Section Headers:\t%ld (bytes from start of file\n", elfhdr->e_shoff);

    if (elfhdr->e_shoff == 0) {
        fprintf(stderr, "Error: Section header table does not exist. Closing program.\n");
        return 1;
    }

    /* Todo: table size error checking */

    if (elfhdr->e_shstrndx == SHN_UNDEF) {
        fprintf(stderr, "Error: Section name string table does not exist. Closing program.\n");
        return 1;
    }

    return 0;
}

