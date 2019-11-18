#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"


/* have a packing attribute for all the data structure
 * within the #pragma 
 */
#pragma pack(push, 1)

typedef struct superblock {
    int64_t signature;
    int16_t amount;
    int16_t root;
    int16_t data_index;
    int16_t data_amou;
    int8_t fat_blocks;
    int32_t padding[4079];
} superblock;

#pragma pack(pop)

superblock sb;

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

