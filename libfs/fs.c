#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "disk.h"
#include "fs.h"

#define FS_MAGIC 	    "ECS150FS"
#define FAT_EOC		     0xffff

struct
{
	uint16_t IsValid;
	/* struct to store the information for mounted fs */
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
	/* struct to keep track of file directory in the fs */
	struct
	{
		char filename[FS_FILENAME_LEN];
		uint32_t size;
		uint16_t datablock;
		uint8_t reserved[10];
	} RootEntries[FS_FILE_MAX_COUNT];
	/* struct to keep track of opened file in fs */
	struct
	{
		uint16_t is_valid;
		uint16_t entry_no;
		uint16_t offset;
	} OpenFiles[FS_OPEN_MAX_COUNT];
	/* FAT array to store file data */
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

	/* if total block amount is not same as total block read from the block_read then return -1 */
	if (FileSystem.SuperBlock.TOTAL_BLOCKS != block_disk_count())
		return -1;

	/* malloc FAT blocks
	 * read correspond block in disk into FAT block */
	FileSystem.FAT = (uint16_t*)malloc(BLOCK_SIZE * FileSystem.SuperBlock.NUM_FAT_BLOCKS);
	for (uint16_t i = 0; i < FileSystem.SuperBlock.NUM_FAT_BLOCKS; i++)
	{
		if ((status = block_read(i + 1, (void*)(FileSystem.FAT + (BLOCK_SIZE / sizeof(uint16_t)) * i))) < 0)
			return -1;
	}

	/* set up root directory */
	if ((status = block_read(FileSystem.SuperBlock.ROOT_BLOCK, (void*)&FileSystem.RootEntries[0])) < 0)
		return -1;

	FileSystem.IsValid = 1;
	return status;
}

int fs_umount(void)
{
	int status;
	if (!FileSystem.IsValid)
		return -1;
	if ((status = block_write(0, (void*)&FileSystem.SuperBlock)) < 0)
		return -1;

	/* above is detection for fs,
	 * below is writing data into the virtual disk */

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
	/* file system is invalid */
	if (!FileSystem.IsValid)
		return -1;
        printf("FS Info:\n");
        printf("total_blk_count=%u\n", FileSystem.SuperBlock.TOTAL_BLOCKS);
        printf("fat_blk_count=%u\n", FileSystem.SuperBlock.NUM_FAT_BLOCKS);
        printf("rdir_blk=%u\n", FileSystem.SuperBlock.ROOT_BLOCK);
        printf("data_blk=%u\n", FileSystem.SuperBlock.DATA_BLOCK);
        printf("data_blk_count=%u\n", FileSystem.SuperBlock.NUM_DATA_BLOCKS);

				uint16_t free_fat_ent = 0, total_fat_ent = FileSystem.SuperBlock.NUM_DATA_BLOCKS;
                // calculate free blocks
                for (uint16_t i = 0; i < total_fat_ent; i++)
        {
                if (FileSystem.FAT[i] == 0)
                        free_fat_ent++;
        }

        uint16_t free_root_entries = 0, total_root_entries = FS_FILE_MAX_COUNT;
        for (uint16_t i = 0; i < FS_FILE_MAX_COUNT; i++)
        {
                if (*FileSystem.RootEntries[i].filename == 0)
                        free_root_entries++;
        }

        printf("fat_free_ratio=%u/%u\n", free_fat_ent, total_fat_ent);
        printf("rdir_free_ratio=%u/%u\n", free_root_entries, total_root_entries);
        return 0;
}

/* This functin will allocate a new block of data space for file to write
 */
uint16_t fs_findfirstblock()
{
	/* data block will start at block 1 to have same index with FAT */
	uint16_t block = FileSystem.SuperBlock.DATA_BLOCK + 1;
	while(block < FileSystem.SuperBlock.TOTAL_BLOCKS && FileSystem.FAT[block] != 0)
		block++;

	if (block == FileSystem.SuperBlock.TOTAL_BLOCKS)
		return FAT_EOC;

	/* update in fat array */
	FileSystem.FAT[block] = block;
	return block - FileSystem.SuperBlock.DATA_BLOCK;
}

/* function will find the filename in the rootentries
 * @filename: the filename you want to find in the root entries */
uint16_t fs_find_root_entry(const char *filename)
{
	for (uint16_t i = 0; i < FS_FILE_MAX_COUNT; i++)
	{
		if (strcmp(filename, FileSystem.RootEntries[i].filename) == 0){
			return i;
		}
	}

	return FS_FILE_MAX_COUNT;
}

void fs_free_blocks(uint16_t firstblock)
{
	if (firstblock == FAT_EOC) return;

	uint16_t block = firstblock + FileSystem.SuperBlock.DATA_BLOCK;
	/* set all data blocks containing the file contents set to 0 */
	while(block != 0 && block != FAT_EOC)
	{
		uint16_t lastblock = block;
		block = FileSystem.FAT[block];
		FileSystem.FAT[lastblock] = 0;
	}
}

/* This function will find the corresponding block for the offset in the file
 * @firstblock: the starting block for the file
 * @offset: the start reading offset for file descriptor
 * return the current block that offset at, return FAT_EOC if the block ends
 */
uint16_t fs_get_block_from_offset(uint16_t firstblock, uint16_t offset)
{
	uint16_t block = firstblock + FileSystem.SuperBlock.DATA_BLOCK;
	while(offset >= BLOCK_SIZE)
	{
		uint16_t newblock = FileSystem.FAT[block];
		if (newblock == 0 || newblock == FAT_EOC)
		{
			newblock = fs_findfirstblock();
			if (newblock == FAT_EOC)
				return FAT_EOC;
			newblock += FileSystem.SuperBlock.DATA_BLOCK;
			FileSystem.FAT[block] = newblock;
		}
		block = newblock;
		offset -= BLOCK_SIZE;
	}
	return block;
}


int fs_create(const char *filename)
{
	/* check if fs is valid, if filename is empty or exceed the max len */
	if (!FileSystem.IsValid || filename == NULL || strlen(filename) > FS_FILENAME_LEN)
		return -1;

	if (fs_find_root_entry(filename) != FS_FILE_MAX_COUNT)
		return -1;

	uint16_t entry = fs_find_root_entry("");

	if (entry == FS_FILE_MAX_COUNT)
		return -1;

	/* size set to 0, and datablk set to FAT_EOC, set filename to @filename */
	FileSystem.RootEntries[entry].size = 0;
	FileSystem.RootEntries[entry].datablock = FAT_EOC;

	strncpy(FileSystem.RootEntries[entry].filename, filename, FS_FILENAME_LEN);
	FileSystem.RootEntries[entry].filename[FS_FILENAME_LEN - 1] = 0;

	return 0;
}

int fs_delete(const char *filename)
{
	/* check if fs is valid, if filename is empty or exceed the max len */
	if (!FileSystem.IsValid || filename == NULL || strlen(filename) > FS_FILENAME_LEN)
		return -1;

	uint16_t entry = fs_find_root_entry(filename);
	if (entry == FS_FILE_MAX_COUNT)
		return -1;

	/* delete fat block(set fat block to 0) */
	fs_free_blocks(FileSystem.RootEntries[entry].datablock);
	memset(FileSystem.RootEntries[entry].filename, 0, FS_FILENAME_LEN);
	FileSystem.RootEntries[entry].size = 0;
	FileSystem.RootEntries[entry].datablock = 0;
	return 0;
}

int fs_ls(void)
{
	int status;
	if (!FileSystem.IsValid)
		return -1;

	printf("FS Ls:\n");
	for (uint16_t i = 0; i < FS_FILE_MAX_COUNT; i++)
	{
		if (*FileSystem.RootEntries[i].filename != 0)
		{
			uint16_t sz = FileSystem.RootEntries[i].size;
			printf("file: %s, size: %u, data_blk: %u\n",
					FileSystem.RootEntries[i].filename,
					sz, FileSystem.RootEntries[i].datablock);
		}
	}
}

int fs_open(const char *filename)
{
	/* check if the filesystem is valid, if filename valid */
	if (!FileSystem.IsValid || filename == NULL || strlen(filename) > FS_FILENAME_LEN)
		return -1;

	/* find the file entry for this filename in mounted fs */
	uint16_t entry = fs_find_root_entry(filename);

	if (entry == FS_FILE_MAX_COUNT)
		return -1;

	/* find a valid index in open file struct */
	uint16_t fd;
	for (fd = 0; fd < FS_OPEN_MAX_COUNT; fd++)
	{
		if (!FileSystem.OpenFiles[fd].is_valid)
			break;
	}

	if (fd == FS_OPEN_MAX_COUNT)
		return -1;
	/* set offset to 0, set valid to not valid, store FAT entry to entry */
	FileSystem.OpenFiles[fd].offset = 0;
	FileSystem.OpenFiles[fd].is_valid = 1;
	FileSystem.OpenFiles[fd].entry_no = entry;
	return fd;
}

int fs_close(int fd)
{
	/* check if have condition to close file */
	if (!FileSystem.IsValid || fd < 0 || fd >= FS_OPEN_MAX_COUNT || FileSystem.OpenFiles[fd].is_valid == 0)
		return -1;

	FileSystem.OpenFiles[fd].is_valid = 0;
	return 0;
}

int fs_stat(int fd)
{
	if (!FileSystem.IsValid || fd < 0 || fd >= FS_OPEN_MAX_COUNT || FileSystem.OpenFiles[fd].is_valid == 0)
	return -1;

	uint16_t entry = FileSystem.OpenFiles[fd].entry_no;
	if (*FileSystem.RootEntries[entry].filename == 0)
		return -1;

	return FileSystem.RootEntries[entry].size;
}

int fs_lseek(int fd, size_t offset)
{
	if (!FileSystem.IsValid || fd < 0 || fd >= FS_OPEN_MAX_COUNT || FileSystem.OpenFiles[fd].is_valid == 0)
	return -1;

	uint16_t entry = FileSystem.OpenFiles[fd].entry_no;
	if (*FileSystem.RootEntries[entry].filename == 0)
		return -1;

	uint16_t newoffset = offset + FileSystem.OpenFiles[fd].offset;
	if (newoffset > FileSystem.RootEntries[entry].size)
		return -1;

	FileSystem.OpenFiles[fd].offset = newoffset;
	return 0;
}

int fs_write(int fd, void *buf, size_t count)
{
	/* check if fs is valid, fd is opened*/
	if (!FileSystem.IsValid || fd < 0 || fd >= FS_OPEN_MAX_COUNT || FileSystem.OpenFiles[fd].is_valid == 0)
	return -1;

	/* get the starting index of block in root entries */
	uint16_t entry = FileSystem.OpenFiles[fd].entry_no;
	uint16_t firstblock = FileSystem.RootEntries[entry].datablock;

	if (*FileSystem.RootEntries[entry].filename == 0)
		return -1;

	/* when file is an empty file and needs to extend the size */
	if (firstblock == FAT_EOC && count > 0)
	{
		firstblock = FileSystem.RootEntries[entry].datablock = fs_findfirstblock();
		if (firstblock == FAT_EOC)
			return 0;
	}

	int byteswritten = 0;
	int offset = FileSystem.OpenFiles[fd].offset;

	/* 1. check the position of offset in block. and set the bytesleft
	 * 2. convert offset position into the corresponding data block
	 * 3. copy the content of block out to tempbuf
	 * 4. copy the rest of the block content from @buf
	 * 5. overwritten the whole block with tempbuf */
	uint8_t tmpbuffer[BLOCK_SIZE];
	while(count > 0)
	{
		int bytesleft = BLOCK_SIZE - (offset % BLOCK_SIZE);
		if (bytesleft > count)
			bytesleft = count;

		uint16_t block = fs_get_block_from_offset(firstblock, offset);
		if (block == FAT_EOC || block_read(block, tmpbuffer) < 0)
			break;

		memcpy(tmpbuffer + (offset % BLOCK_SIZE), buf + byteswritten, bytesleft);
		if (block_write(block, tmpbuffer) < 0)
			break;

		count -= bytesleft;
		byteswritten += bytesleft;
		offset += bytesleft;
	}

	if (FileSystem.RootEntries[entry].size < offset)
		FileSystem.RootEntries[entry].size = offset;

	FileSystem.OpenFiles[fd].offset = offset;
	return byteswritten;

}

int fs_read(int fd, void *buf, size_t count)
{
	/* check if fs is valid, fd is opened*/
	if (!FileSystem.IsValid || fd < 0 || fd >= FS_OPEN_MAX_COUNT || FileSystem.OpenFiles[fd].is_valid == 0)
		return -1;

	uint16_t entry = FileSystem.OpenFiles[fd].entry_no;
	uint16_t firstblock = FileSystem.RootEntries[entry].datablock;
	if (*FileSystem.RootEntries[entry].filename == 0)
		return -1;

	int bytesread = 0;
	int size = FileSystem.RootEntries[entry].size;
	int offset = FileSystem.OpenFiles[fd].offset;

	if (offset + count > size)
		count = size - offset;

	/* read fd block by block */
	uint8_t tmpbuffer[BLOCK_SIZE];
	while(count > 0)
	{
		int bytesleft = BLOCK_SIZE - (offset % BLOCK_SIZE);
		if (bytesleft > count)
			bytesleft = count;
		/* get the offset current block */
		uint16_t block = fs_get_block_from_offset(firstblock, offset);
		if (block == FAT_EOC || block_read(block, tmpbuffer) < 0)
			break;
		memcpy(buf + bytesread, tmpbuffer + (offset % BLOCK_SIZE), bytesleft);

		/* copy offset current block into the buffer,
		 * and set offset points to next block */
		count -= bytesleft;
		bytesread += bytesleft;
		offset += bytesleft;
	}

	/* implicitly incremented offset */
	FileSystem.OpenFiles[fd].offset = offset;
	return bytesread;
}
