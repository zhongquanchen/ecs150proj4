
# Project 4: File Systems
### by Devinesh Singh & Zhongquan Chen

## Phase I:
### data struct :

#### FileSystem struct 
> For data struct we have a FileSystem struct which contain superblock, root entries and openfiles in the struct.
#### SuperBlock struct
> It contains the basic info of a disk. 
#### RootEntries struct
> The struct stores files' name and directory.
#### OpenFiles struct
> DETAIL NEEDED

```c
fs_mount()
```
> The method makes the file system ready to be used.
1. First open the disk block by the @diskname given. Check if the disk contain the disk name, if not return -1.
2. Read the first block of disk and store it into ` Superblock ` in struct ` FileSystem `. Check if the filesytem name match ` ECS150fs `if not return -1.
3. Make sure the number fat blocks calculated by number of data block is the same as the number of fat block the read from ` block_read `.
4. Allocate the size of fat block, then use a for loop to assign each block's content by ` block_read() `. Return -1 if any allocation fails.
5. Set up ` RootEntries ` by ` block_read `
6. change status of ` FileSystem.IsValid ` to true 
	
```c
fs_umount()
```
> The method will close the disk and all the file data will written out to disk.  
1. First check if Filesytem is valid, if not return -1. In other words, FileSystem is not mounted.  
2. Write ` SuperBlock ` info into the disk.  
3. Use a for loop to write all the fat block info into the disk.  
4. Write ` ROOT_BLOCK ` info into the disk.  
5. Close the disk.   

```c
fs_info()
```
> The method will print out File system's info
1. (skip the printing implement detail)
2. Use for loop to determine if FAT at index i is 0, then it will be an empty FAT
3. Use for loop to determine if RootEntries at index i is '0\', then it will be an empty directory
4. Take those index and print out fat free ratio & rdir free ratio.

### Testing(DURING PHASE):
> Testing method we use is
1. first create a disk.fs using fs_make.x
2. add some files into disk.fs using fs_ref.x
3. print out the info of ` disk.fs ` by using fs_ref.x
4. print out the info of ` disk.fs ` by using test_fs.x
5. use diff to determine if result are the same.

## Phase II:
``` c
fs_create(const char *filename)
```
> The method will create a new and empty file name @filename. Has a helper function ` fs_find_root_entry `
1. Check if Filesystem is valid or if it is existed. Check if filename is empty
   or exceed the max name length
2. Use helper function ` fs_find_root_entry() ` to seek for an empty entry for 
   file to store.
3. Set size of the file = 0, and the datablock to ` FAT_EOC `.
4. Copy the filename to the root entry. 

``` c
fs_find_root_entry(const char *filename)
```
> The method will find an empty entry for the root directory and return the 
  index for the entry.  
1. Use for loop to find an empty entry, return index on found else return file
  maximum count  

``` c
fs_delete(const char *filename)
```
> The method will remove the file from root directory of the mounted file system
1. Check if Filesystem is valid or if it is existed. 
2. Use helper function ` fs_find_root_entry ` to seek for the corresponding  
   filename. return -1 if the filename does not exist in filesystem.  
3. Use helper function ` fs_free_block ` to free the space for the correspondin
   filename. 
4. Reset the root entry to 0. make it ready to store other file

``` c
fs_free_blcok(uint16_t firstblock)
```
> The function will free the space for the block in the mounted file system  
1. Check if the block that passed in @firstblock is End Of Chain, return. 
2. Use While loop to check if blcok has been freed and FAT at block index is
   set to 0

```c
fs_ls(void)
```
> The method will print out the list in mounted filesystem
1. Check if FileSystem is valid or if it is existed.
2. Use for loop to loop through the maximun number of file count
3. Check at index i has a filename that is not empty and print it. 

### Testing(DURING PHASE):
> Testing method we use is
1. first create a disk.fs using fs_make.x
2. add some empty files into disk.fs using fs_ref.x
3. print out the ls of ` disk.fs ` by using fs_ref.x
4. print out the ls of ` disk.fs ` by using test_fs.x
5. use diff to determine if result are the same.

## Phase III:
```c
fs_open(const char *filename)
```
>The method will open a file by fs in virtual disk  
1. Check if filename and the fs is valid.  
2. use ` fs_find_root_entry ` method to check if the filename is already in disk
3. use for loop to find the an empty place in ` openfile ` struct to mark the 
   open file.
4. Set the openfile's offset and entry. So that we can find it in root directory   struct and its validation. 

```c
fs_close(int fd)
```
>This function will close a file in fs 
1. Check if filename and the fs is valid.
2. Set ` Openfile ` in ` FileSystem ` to valid. So that when we have other file
   need to open, it can store in this index again.

```c
fs_stat(int fd)
```
>This function will check for the size, name, datablock start. 
1. Check if filename and the fs is valid. 
2. Find the entry in ` OpenFile ` struct to see if its has opened 
3. Return the size of the file 

```c
fs_lseek(int fd, size_t offset)
```
>This function set the offset of the file
1. Check if filename and the fs is valid.
2. Find the entry in ` OpenFile ` struct to see if its has opened
3. Store the new offset to the file 

### TESTING ###
Wrote a testing script to test.

## Phase IV:
```c
fs_write(int fd, void* buf, size_t count)
```
>This function will write @buf content into the @fd file descriptor
1. Check if file descriptor and fs valid
2. find the entry of the opened file by the @fd. 
3. Record down the starting block index. 
4. Check if the block is FAT_EOC, and see if it needs to extend data block
5. Check the position of the offset in block and set the bytesleft. 
6. convert offset position into corresponding data block.
7. copy the content of block out to a temp buf
8. copy the rest of the block content from @buf
9. overwritten the whole block with temp buf

```c
fs_get_block_from_offset(uint16_t firstblock, uint16_t offset)
```
>This is a helper function to search for the offset in the corresponding data  
 block.  
1. Set block position to the starting datablock of the file  
2. See if the offset data block is different than the starting datablock  
3. if it is different, than mark the @block position plus a datablock  
4. until the offset data block is the same data block with @block  
5. return the @block, which is the offset block position.  

```c
fs_findfirstblock()
```
>This is a helper function to search for the an availble data block, will used 
 when the file needs to extend block.
1. Set the @block to the starting index of data_block
2. use while loop to find the a empty data block for use
3. if the @block is availble in FAT array, which FAT[block] = 0, then the block
   can be use.
4. return avaible @block or FAT_EOC to tell no empty space anymore

```c
fs_read(int fd, void* buf, size_t count)
```
>This function will read the file and copy to @buf
1. Check if @fd and fs is valid
2. Find the entry for the fd in ` Openfile `
3. Find the starting data block in ` RootEntries `
4. Find the offset to corresponding datablock using ` fs_get_block_from_offset `
5. Copy the @buf to the data block
6. return the btyes that have read.  

### TESTING ###
Wrote a testing script to test.

