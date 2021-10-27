/*
    Provides function implementations for the nine required functions in Project 4
*/
#include "cpmfsys.h"
#include "diskSimulator.h" 
#include <string.h>

//function to allocate memory for a DirStructType (see above), and populate it, given a
//pointer to a buffer of memory holding the contents of disk block 0 (e), and an integer index
// which tells which extent from block zero (extent numbers start with 0) to use to make the
// DirStructType value to return. Index can be between 0 and 31 since the block can hold 32 files total
//    Index different files via: 'd->BC = (e + index * EXTENT_SIZE)[13];'
//    Reverse that statement in order to write dir struct
//    dir_addr = e + index * EXTENT_SIZE
//    dir->status = dir_addr[0]
DirStructType *mkDirStruct(int index, uint8_t *e){
    // create mem space for dir (a pointer)
    struct dirStruct *dir = malloc(EXTENT_SIZE); // or maybe sizeof(e)

    // get extent address in block 0 
    uint8_t *dir_addr = e + index * EXTENT_SIZE;

    // copy status from block 0 
    dir->status = dir_addr[0];

    // dir addr [1-8]: for loop
    // if name length < 8, filled by ' '
    // obtain dir->name from in-memory block0
    for (int i = 1; i < 9; i++) {
        dir->name[i-1] = dir_addr[i];
    }
    
    dir->name[8] = 0;   //add null char to end

    // dir_addr[9-11]: for loop
    // obtain dir extension from in-memory block0
    // add '\0' at the end of dir->extension
    for (int i = 0; i <= 2; i++) {
        dir->extension[i] = dir_addr[i+9];
    }
    dir->extension[3] = 0;

    // obtain dir->XL, BC, XH, RC from block0 referenced by dir_addr
    dir->XL = dir_addr[12];
    dir->BC = dir_addr[13];
    dir->XH = dir_addr[14];
    dir->RC = dir_addr[15];

    // obtain dir->block array from in-memory block0
    for (int i = 0; i <= 15; i++) {
        dir->blocks[i] = dir_addr[i+16];
    }

    return dir;
} 

// function to write contents of a DirStructType struct back to the specified index of the extent
// in block of memory (disk block 0) pointed to by e
// be sure to pad file name to 8 chars with '\0' end token
//    Index different files via: 'd->BC = (e + index * EXTENT_SIZE)[13];'

void writeDirStruct(DirStructType *d, uint8_t index, uint8_t *e) {
    // get extent address in block 0 
    uint8_t *dir_addr = e + index * EXTENT_SIZE;

    // copy status from block 0 
    dir_addr[0] = d->status;

    // dir addr [1-8]: for loop
    // if name length < 8, filled by ' '
    // obtain dir->name from in-memory block0
    for (int i = 1; i < 9; i++) {
        dir_addr[i] = d->name[i-1];
    }
    dir_addr[9] = 0;

    // dir_addr[9-11]: for loop
    // obtain dir extension from in-memory block0
    // add '\0' at the end of dir->extension
    for (int i = 0; i <= 2; i++) {
        dir_addr[i+9] = d->extension[i];
    }
    dir_addr[13] = 0;

    // obtain dir->XL, BC, XH, RC from block0 referenced by dir_addr
    dir_addr[12] = d->XL;
    dir_addr[13] = d->BC;
    dir_addr[14] = d->XH;
    dir_addr[15] = d->RC;

    // obtain dir->block array from in-memory block0
    for (int i = 0; i <= 15; i++) {
        dir_addr[i+16] = d->blocks[i];
    }
}

// populate the FreeList global data structure. freeList[i] == true means 
// that block i of the disk is free. block zero is never free, since it holds
// the directory. freeList[i] == false means the block is in use. 
/*
    for (all i extent in block0) {
        dir = mkDirStruct(i, block0);
        if (dir is used) {
            //mark all blocks of dir as 'used'
            for (j=0; j<16; j++) {
                if (dir->blocks[j] != 0) {
                    freelist[(int)dir->block[j]] = false; //not available
                }
            }
        }
    }
*/
void makeFreeList() {
    //initialize everything to true, then set false the ones that are full
    freeList[0] = false;    //block0 should always be used
    for (int i = 1; i < NUM_BLOCKS; i++) {
        freeList[i] = true;
    }
    for (int i = 0; i < 32; i++) {  // each block is 1024b and 1024/32 = 32, so 32 files
        struct dirStruct *dir = mkDirStruct(i, directory);
        if (dir->status != 0xe5) { //0xe5 indicates a free block, so if not 0xe5, we are used
            // now mark all blocks of dir as 'used'
            for (int j = 0; j < 16; j++) {
                if (dir->blocks[j] != 0) {
                    freeList[(int) dir->blocks[j]] = false; // mark as not available
                }
            }
        }
    }
}

// debugging function, print out the contents of the free list in 16 rows of 16, with each 
// row prefixed by the 2-digit hex address of the first block in that row. Denote a used
// block with a *, a free block with a . 
void printFreeList() {
    printf("---------------------------------------\n");
    printf("Contents of free list: (* means in-use)");
    for (int i = 0; i < NUM_BLOCKS; i++) {
        if (i % 16 == 0) {
            printf("\n%.2x", i);
        }
        if (freeList[i] == true) { // if block is free
            printf(" .");
        }
        else {
            printf(" *");
        }
    }
    printf("\n---------------------------------------\n");
}

// internal function, returns -1 for illegal name or name not found
// otherwise returns extent nunber 0-31
/*
    split up name into file_name, ext_name
    check legal file_name, no blanks/punctuation/control chars;
    pad '\0\ to file_name
    check legal ext_name
    pad '\0' to ext_name

    for (all dir entries in block 0) {
        obtain dir i from block 0; //makedirstruct()
        if (dir->name == file-name and dir->extension == \
            ext_name and dir->status == valid) {
            return i; //only return index
        }
    }
    return -1;
*/
int findExtentWithName(char *name, uint8_t *block0) {    //TODO may need to implement cpmrename's stuff here
    int split_index = -1;
    for (int i = 0; i < strlen(name); i++) {
        if ((int) name[i] == 46) {
            split_index = i;
        }
    }
    if (split_index == -1) {
        split_index = strlen(name);
    }
    char fname[split_index + 1];
    fname[split_index] = 0;
    char ext[strlen(name)-split_index];
    char fname_fin[9];
    char ext_fin[4];

    //handle no extension
    if (strlen(name) - split_index < 1) {    // only edge case where ext length is 0
        char ext[1];
        ext[1] = 0;
        if (checkLegalName(ext) == false) {
            return -2;
        }
        strcpy(ext_fin, ext);
        //pad ext
        if ((int) ext[0] != 32) {
            ext_fin[0] = 32;
        }
        if (strlen(ext) < 3) {
            for (int i = strlen(ext); i < 3; i++) {
                ext_fin[i] = 32;
            }
            ext_fin[3] = 0;
        }
    }
    else {
        char ext[strlen(name)-split_index];
        strncpy(ext, name + split_index + 1, strlen(name));
        if (strlen(ext) >= 4 && (int) ext[3] != 0) {  //if ext is too long
            return -2;
        }
        if (checkLegalName(ext) == false) {
            return -2;
        }
        strcpy(ext_fin, ext);
        //pad ext
        if (strlen(ext) < 3) {
            for (int i = strlen(ext); i < 3; i++) {
                ext_fin[i] = 32;
            }
            ext_fin[3] = 0;
        }
    }
    strncpy(fname, name, split_index);   // problem here with how we treat split_index
    if (checkLegalName(fname) == false) {   // newName is illegal
        return -2;
    }
    //pad name
    strcpy(fname_fin, fname);
    if (strlen(fname) < 8) {
        for (int i = strlen(fname); i < 8; i++) {
            fname_fin[i] = 32;
        }
        fname_fin[8] = 0;
    }
    
    // now actually find index
    for (int i = 0; i < EXTENT_SIZE; i++) {
        struct dirStruct *dir = mkDirStruct(i, block0);
        if ((strcmp(dir->name, fname_fin) == 0) && (strcmp(dir->extension, ext_fin) == 0) \
            && dir->status == 1) {
            return i; // return index of found block
        }
    }
    // else return -1 for not found
    return -1;
}

// internal function, returns true for legal name (8.3 format), false for illegal
// (name or extension too long, name blank, or  illegal characters in name or extension)
// between ascii 65-90, 97-122, 48-57
// basically check that char when cast as an int is in valid range
bool checkLegalName(char *name) {
    for (int i = 0; i < 9; i++) {
        if (((int) name[i] >  0 && (int) name[i] < 48) || \
            ((int) name[i] > 57 && (int) name[i] < 65) || \
            ((int) name[i] > 90 && (int) name[i] < 97) || \
            ((int) name[i] > 122) || ((int) name[i] == 32)) {
            return false;
        }
        if ((int) name[i] == 0) {   // in case name is valid but not long enough
            break;
        }
        if (i == 8 && (int) name[i] != 0) { // in case name too long
            return false;
        }
    }
    return true;
}

// print the file directory to stdout. Each filename should be printed on its own line, 
// with the file size, in base 10, following the name and extension, with one space between
// the extension and the size. If a file does not have an extension it is acceptable to print
// the dot anyway, e.g. "myfile. 234" would indicate a file whose name was myfile, with no 
// extension and a size of 234 bytes. This function returns no error codes, since it should
// never fail unless something is seriously wrong with the disk 
/*
    cpm_block0 = blockRead(block0); //read block0 into cpm
    for all the extent reffered by index in cpm_block0 {
        cpm_dir->status is used) { // 0xe5
            //compute file length
            block_number = 0
            for (b_index = 0; b_index < 16; b_index++) {
                if cpm_dir->blocks[b_index] != 0
                    block_number++
            }
            //RC = cpm_dir->RC, BC = cpm_dir->BC
            file_length = (block_number-1*1024+RC*128+BC
            // a bug if both RC and BC are 0s
            print file name and lenght from cpm
        }   //end if cmp_dir->status is used
    }
*/
void cpmDir() {
    printf("-----------------------\n");
    printf("---Directory Listing---\n");
    for (int i = 0; i < 32; i++) {  // each block is 1024b and 1024/32 = 32, so 32 files
        struct dirStruct *dir = mkDirStruct(i, directory);
        if (dir->status != 0xe5) { //0xe5 indicates a free block, so if not 0xe5, we are used
            int block_num = 0;
            for (int i = 0; i < 16; i++) {
                if (dir->blocks[i] != 0) {
                    block_num++;
                }
            }
            int rc = dir->RC;
            int bc = dir->BC;
            int file_size = 0 + ((block_num - 1) * 1024) + (rc * 128) + bc;
            int space = 8;
            //for name
            for (int i = 7; i > 0; i--) {
                if ((int) dir->name[i] == 32) {
                    space = i;
                }
            }
            char name[space+1];
            int size = strlen(name);
            name[space] = '\0';
            strncpy(name, dir->name, space);
            //for ext
            space = 2;
            for (int i = 2; i >= 0; i--) {
                if ((int) dir->extension[i] == 32) {
                    space = i-1;
                }
            }
            char ext[space+2];
            ext[space+1] = '\0';
            strncpy(ext, dir->extension, space+1);

            printf("%s.%s %d\n", name, ext, file_size);
        }
    }
    printf("-----------------------\n\n");
}

// error codes for next five functions (not all errors apply to all 5 functions)  
/* 
    0 -- normal completion
   -1 -- source file not found
   -2 -- invalid  filename
   -3 -- dest filename already exists 
   -4 -- insufficient disk space 
*/ 

// read directory block, 
// modify the extent for file named oldName with newName, and write to the disk
int cpmRename(char *oldName, char * newName) {
    int index = findExtentWithName(oldName, directory);
    int check = findExtentWithName(newName, directory);
    if (index == -1) {  // oldName file doesn't exist
        return index;
    }
    if (check != -1) {  // newName already taken
        return -3;
    }
    struct dirStruct *dir = mkDirStruct(index, directory);
    
    //format new name
    int split_index = -1;
    for (int i = 0; i < strlen(newName); i++) {
        if ((int) newName[i] == 46) {
            split_index = i;
        }
    }
    if (split_index == -1) {
        split_index = strlen(newName);
    }
    char fname[split_index + 1];
    fname[split_index] = 0;
    char fname_fin[9];
    char ext_fin[4];
    //handle no extension
    if (strlen(newName) - split_index < 1) {    // only edge case where ext length is 0
        char ext[1];
        ext[1] = 0;
        if (checkLegalName(ext) == false) {
            return -2;
        }
        strcpy(ext_fin, ext);
        //pad ext
        if ((int) ext[0] != 32) {
            ext_fin[0] = 32;
        }
        if (strlen(ext) < 3) {
            for (int i = strlen(ext); i < 3; i++) {
                ext_fin[i] = 32;
            }
            ext_fin[3] = 0;
        }
    }
    else {
        char ext[strlen(newName)-split_index];
        strncpy(ext, newName + split_index + 1, strlen(newName));
        if (strlen(ext) >= 4 && (int) ext[3] != 0) {  //if ext is too long
            return -2;
        }
        if (checkLegalName(ext) == false) {
            return -2;
        }
        strcpy(ext_fin, ext);
        //pad ext
        if (strlen(ext) < 3) {
            for (int i = strlen(ext); i < 3; i++) {
                ext_fin[i] = 32;
            }
            ext_fin[3] = 0;
        }
    }
    strncpy(fname, newName, split_index);   // problem here with how we treat split_index
    if (checkLegalName(fname) == false) {   // newName is illegal
        return -2;
    }
    //pad name
    strcpy(fname_fin, fname);
    if (strlen(fname) < 8) {
        for (int i = strlen(fname); i < 8; i++) {
            fname_fin[i] = 32;
        }
        fname_fin[8] = 0;
    }

    //write new name and extension
    for (int i = 0; i <= 8; i++) {
        dir->name[i] = fname_fin[i];
    }

    for (int i = 0; i <= 2; i++) {
        dir->extension[i] = ext_fin[i];
    }

    writeDirStruct(dir, index, directory);
    makeFreeList(); 
    return 0;
}

// delete the file named name, and free its disk blocks in the free list 
int  cpmDelete(char * name) {
    int index = findExtentWithName(name, directory);
    if (index == -1) {  // oldName file doesn't exist
        return index;
    }
    struct dirStruct *dir = mkDirStruct(index, directory);
    
    // get extent address in block 0 
    uint8_t *dir_addr = directory + index * EXTENT_SIZE;

    dir_addr[0] = 0xe5;
    for (int i = 1; i < 32; i++) {
        dir_addr[i] = 0;
    }
    makeFreeList();
    return 0;
}

