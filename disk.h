#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#define BLOCK_SIZE 128

FILE* diskFile;
int num_cylinders;
int num_sectors;
int file_size;
int num_blocks;
int num_writes = 0;
int num_reads = 0;

int create_disk(char* fileName) {
    //open the file for both reading and writing and the ability to store binary files
	diskFile = fopen(fileName,"r+b");
	if(!diskFile) diskFile = fopen(fileName,"w+b");
	if(!diskFile) return 0;

    //set the number of blocks
    num_blocks = num_cylinders * num_sectors;

    //set the file size
    file_size = num_sectors * num_cylinders * BLOCK_SIZE;

    //truncate the file the the specific length
    ftruncate(fileno(diskFile), file_size);

    //return 0 for all is well
    return 1;
}

int get_disk_file() {
    return fileno(diskFile);
}

int disk_size() {
    return num_blocks;
}

static void sanity_check( int blocknum)
{
	if(blocknum<0) {
		printf("ERROR: blocknum (%d) is negative!\n",blocknum);
		abort();
	}

	if(blocknum>=num_blocks) {
		printf("ERROR: blocknum (%d) is too big!\n",blocknum);
		abort();
	}
}

void disk_read( int cylinder, int sector, char buffer[])
{
	//calculate the block number using the cylinder and the sector
	/*
	Based on the cylinder ‘c’ and sector ‘s’, to calculate the block index is
	: block index = （c-1） * (# of sectors on the disk) + s
	*/
	int blocknum = (cylinder - 1) * (num_sectors) + sector;

	//perform a sanity check on the block number and data pointer
	sanity_check(blocknum);

	//seek the disk file for the correct data
	fseek(diskFile, blocknum * BLOCK_SIZE,SEEK_SET);

	printf("Starting Disk Read from block: %d\n", blocknum);

	if(fread(buffer, BLOCK_SIZE, 1,diskFile) == 1) {
		num_reads++;
	} else {
		printf("ERROR: couldn't access simulated disk: %s\n",strerror(-1));
		abort();
	}
}

void disk_write( int cylinder, int sector, char *data)
{
	int blocknum = (cylinder - 1) * (num_sectors) + sector;

	sanity_check(blocknum);

	fseek(diskFile,blocknum * BLOCK_SIZE,SEEK_SET);

	if(fwrite(data, BLOCK_SIZE, 1, diskFile) == 1) {
		num_writes++;
	} else {
		printf("ERROR: couldn't access simulated disk: %s\n",strerror(-1));
		abort();
	}
}

void disk_close()
{
	if(diskFile) {
		printf("%d disk block reads\n",num_reads);
		printf("%d disk block writes\n",num_writes);
		diskFile = 0;
	}
}

