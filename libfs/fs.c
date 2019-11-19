#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

#define FS_MAGIC 	    "ECS150FS"
#define FAT_EOC		     0xffff

struct
{
	uint16_t IsValid;

	struct
	{
		uint64_t SIGANTURE;
		uint16_t TOTAL_BLOCKS;
		uint16_t ROOT_BLOCK;
		uint16_t DATA_BLOCK;
		uint16_t NUM_DATA_BLOCKS;
		uint8_t  NUM_FAT_BLOCKS;
		uint8_t  RESERVED[BLOCK_SIZE - 17]; // 17 blocks: Section 5.1 Table
	} SuperBlock;
	
	struct
	{
		char filename[FS_FILENAME_LEN];
		uint32_t size;
		uint16_t datablock;
		uint8_t reserved[10];
	} RootEntries[FS_FILE_MAX_COUNT];

	struct
	{
		uint16_t is_valid;
		uint16_t entry_no;
		uint16_t offset;
	} OpenFiles[FS_OPEN_MAX_COUNT];

	uint16_t *FAT;
} FileSystem;

int fs_mount(const char *diskname)
{
    char* sign = "ECS150FS";
    /* check if block in the disk can't open */
    if (block_disk_open(diskname) == -1)
	return -1;
    
    block_read(0, (void*)&sb);

    if(memcmp((char*)&sb.signature, signature, 8 != 0))
    {
	printf("read the wrong disk\n");
	return -1;	
    }

}

int fs_umount(void)
{
	/* TODO: Phase 1 */
}

int fs_info(void)
{
	/* TODO: Phase 1 */
}

int fs_create(const char *filename)
{
	/* TODO: Phase 2 */
}

int fs_delete(const char *filename)
{
	/* TODO: Phase 2 */
}

int fs_ls(void)
{
	/* TODO: Phase 2 */
}

int fs_open(const char *filename)
{
	/* TODO: Phase 3 */
}

int fs_close(int fd)
{
	/* TODO: Phase 3 */
}

int fs_stat(int fd)
{
	/* TODO: Phase 3 */
}

int fs_lseek(int fd, size_t offset)
{
	/* TODO: Phase 3 */
}

int fs_write(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}

int fs_read(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}

