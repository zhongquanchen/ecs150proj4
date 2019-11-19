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
	int status;

	FileSystem.IsValid = 0;

	status = block_disk_open(diskname);
	if (status < 0)
		return -1;

	status = block_read(0, &FileSystem.SuperBlock);
	if (status < 0)
		return -1;

	if (strncmp((char*)&FileSystem.SuperBlock.SIGANTURE, FS_MAGIC, strlen(FS_MAGIC)) != 0)
		return -1;
	
	// calculate the number of blocks
	uint16_t calculated_fat_blocks = (FileSystem.SuperBlock.NUM_DATA_BLOCKS * 2) / BLOCK_SIZE;
	if (calculated_fat_blocks <= 0 || FileSystem.SuperBlock.NUM_FAT_BLOCKS != calculated_fat_blocks)
		return -1;

	FileSystem.FAT = (uint16_t*)malloc(BLOCK_SIZE * FileSystem.SuperBlock.NUM_FAT_BLOCKS);
	for (uint16_t i = 0; i < FileSystem.SuperBlock.NUM_FAT_BLOCKS; i++)
	{
		if ((status = block_read(i + 1, (void*)(FileSystem.FAT + (BLOCK_SIZE / sizeof(uint16_t)) * i))) < 0)
			return -1;
	}

	if ((status = block_read(FileSystem.SuperBlock.ROOT_BLOCK, (void*)&FileSystem.RootEntries[0])) < 0)
		return -1;

	FileSystem.IsValid = 1;
	return status;
}

int fs_umount(void)
{
	/* TODO: Phase 1 */
	int status;
	if (!FileSystem.IsValid)
		return -1;

	if ((status = block_write(0, (void*)&FileSystem.SuperBlock)) < 0)
		return -1;

	for (int i = 0; i < FileSystem.SuperBlock.NUM_FAT_BLOCKS; i++)
	{
		if ((status = block_write(i + 1, (void*)(FileSystem.FAT + (BLOCK_SIZE / sizeof(uint16_t)) * i))) < 0)
			return -1;
	}

	if ((status = block_write(FileSystem.SuperBlock.ROOT_BLOCK, (void*)&FileSystem.RootEntries[0])) < 0)
		return -1;

	FileSystem.IsValid = 0;
	status = block_disk_close();
	return status;
}

int fs_info(void)
{
	/* TODO: Phase 1 */
	int fs_info(void)
{
	if (!FileSystem.IsValid)
		return -1;

	printf("FS Info:\n");
	printf("total_blk_count=%u\n", FileSystem.SuperBlock.TOTAL_BLOCKS);
	printf("fat_blk_count=%u\n", FileSystem.SuperBlock.NUM_FAT_BLOCKS);
	printf("rdir_blk=%u\n", FileSystem.SuperBlock.ROOT_BLOCK);
	printf("data_blk=%u\n", FileSystem.SuperBlock.DATA_BLOCK);
	printf("data_blk_count=%u\n", FileSystem.SuperBlock.NUM_DATA_BLOCKS);

	uint16_t free_fat_ent = 0, total_fat_ent = ((FileSystem.SuperBlock.NUM_FAT_BLOCKS * BLOCK_SIZE) / sizeof(uint16_t));
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

