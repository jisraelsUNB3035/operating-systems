#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include "shell.h"
#include "fat32.h"

#define BUF_SIZE 256
#define CMD_INFO "INFO"
#define CMD_DIR "DIR"
#define CMD_CD "CD"
#define CMD_GET "GET"
#define CMD_PUT "PUT"

void shellLoop(int fd) {
	int running = true;
	uint32_t curDirClus;
	char buffer[BUF_SIZE];
	char bufferRaw[BUF_SIZE];

	fat32Head *h = createHead(fd);

	if(h == NULL) {
		running = false;
	}
	else {// valid, grab the root cluster	
		curDirClus = h->bs->BPB_RootClus;
	}	

	while(running) 
	{
		printf(">");
		if(fgets(bufferRaw, BUF_SIZE, stdin) == NULL) 
		{
			running = false;
			break;
		}
		bufferRaw[strlen(bufferRaw)-1] = '\0'; /* cut new line */
		for(int i=0; i < strlen(bufferRaw)+1; i++)
			buffer[i] = toupper(bufferRaw[i]);
	
		if(strncmp(buffer, CMD_INFO, strlen(CMD_INFO)) == 0) {
			printInfo(h);
		}
		else if(strncmp(buffer, CMD_DIR, strlen(CMD_DIR)) == 0) {
			doDir(h, curDirClus);	
		}
		else if(strncmp(buffer, CMD_CD, strlen(CMD_CD)) == 0) {
			//curDirClus = doCD(h, curDirClus, buffer);
		}
		else if(strncmp(buffer, CMD_GET, strlen(CMD_GET)) == 0) {
			//doDownload(h, curDirClus, buffer);
		}
		else if(strncmp(buffer, CMD_PUT, strlen(CMD_PUT)) == 0) {
			//doUpload(h, curDirClus, buffer, bufferRaw);
			printf("Bonus marks!\n");
		}
		else { 
			printf("\nCommand not found\n");
		}
	}
	printf("\nExited...\n");
	
	cleanupHead(h);
}

static void println(char* string) {
	printf("%s\n", string);
}

void printInfo(fat32Head* h) {
	fat32BS* bs = h->bs;
	uint64_t sizeB = (uint64_t)bs->BPB_BytesPerSec * bs->BPB_TotSec32;
	uint16_t sizeMB = (sizeB / 1000000);
	float sizeGB = (float)sizeMB / 1000;

	println("---- Device Info ----");
	printf("OEM Name: %.*s\n", BS_OEMName_LENGTH, bs->BS_OEMName);
	printf("Label: %.*s\n", BS_VolLab_LENGTH, bs->BS_VolLab);
	printf("File System Type: %.*s\n", BS_FilSysType_LENGTH, bs->BS_FilSysType);
	printf("Media Type: 0x%X (fixed)\n", (unsigned char)bs->BPB_Media);
	printf("Size: %llu bytes (%u MB, %1.3f GB)\n", (unsigned long long)sizeB, (unsigned int)sizeMB, sizeGB);
	printf("Drive #: %u (hard disk)\n", (unsigned int)bs->BS_DrvNum);
	println("");

	println("---- Geometry ----");
	printf("Bytes per Sector: %u\n", (unsigned int)bs->BPB_BytesPerSec);
	printf("Sectors per Cluster: %u\n", (unsigned int)bs->BPB_SecPerClus);
	printf("Total Sectors: %lu\n", (unsigned long)bs->BPB_TotSec32);
	printf("Geom: Sectors per Track: %u\n", (unsigned int)bs->BPB_SecPerTrk);
	printf("Geom: Heads: %u\n", (unsigned int)bs->BPB_NumHeads);
	printf("Hidden Sectors: %lu\n", (unsigned long)bs->BPB_HiddSec);
	println("");

	println("---- FS Info ----");
	printf("Volume ID: TODO\n"); //comes from cluster
	printf("Version: %u.%u\n", (unsigned int)bs->BPB_FSVerHigh, (unsigned int)bs->BPB_FSVerLow);
	printf("Reserved Sectors: %u\n", (unsigned int)bs->BPB_RsvdSecCnt);
	printf("# of FATs: %u\n", (unsigned int)bs->BPB_NumFATs);
	printf("Fat Size: %lu\n", (unsigned long)bs->BPB_FATSz32);
	printf("Mirrored FAT: %u (%s)\n", (unsigned int)bs->BPB_ExtFlags, (bs->BPB_ExtFlags ? "no" : "yes"));
	printf("Boot Sector Backup Sector #: %u\n", (unsigned int)bs->BPB_BkBootSec);
}

//TODO: figure out why this doesn't work
void doDir(fat32Head* h, uint32_t curDirClus) {
	uint8_t* cluster = loadCluster(h, curDirClus);
	fat32Dir* dir = (fat32Dir*)(&cluster[0]);	
	for(int i=0; i<16; i++) {
		if(dir->DIR_Name[0] == 0xE5) break;
		if(dir->DIR_Name[0] == 0x00) { i=17; break; }
		printf("%s\n", dir->DIR_Name);
		dir++;
	}
	
	printf("gay");
}
