## phase1 ##
Problem description:

Info: Created virtual disk 'test.fs' with '100' data blocks.  
Fail: got '' but expected 'FS Info:'  
Fail: got '' but expected 'total_blk_count=103'  
Fail: got '' but expected 'fat_blk_count=1'  
Fail: got '' but expected 'rdir_blk=2'  
Fail: got '' but expected 'data_blk=3'  
Fail: got '' but expected 'data_blk_count=100'  
Fail: got '' but expected 'fat_free_ratio=99/100'  
Fail: got '' but expected 'rdir_free_ratio=128/128'  

Possible problem:
fs_mount not able to mount size less than 4096 or situation like 
100/4096 != int

FIXED!

Bug in:
fs_info()
wrong printing of total_fat_ent

FIXED!

## Phase 2 ##
NO BUG COUNTER!

TEST LOG:.
$ ./test_fs.x add disk.fs file1  
Wrote file 'file1' (10/10 bytes)  
$ ./test_fs.x add disk.fs file2  
Wrote file 'file2' (5/5 bytes)  
$ ./test_fs.x add disk.fs file3  
Wrote file 'file3' (5/5 bytes)  
$ ./test_fs.x ls disk.fs  
FS Ls:  
file: file1, size: 10, data_blk: 0  
file: file2, size: 5, data_blk: 0  
file: file3, size: 5, data_blk: 0  
$ ./fs_ref.x ls disk.fs   
FS Ls:  
file: file1, size: 10, data_blk: 0  
file: file2, size: 5, data_blk: 0  
file: file3, size: 5, data_blk: 0  
$ ./test_fs.x rm disk.fs file1  
Removed file 'file1'  
$ ./test_fs.x rm disk.fs file2  
Removed file 'file2'  
$ ./test_fs.x rm disk.fs file3  
Removed file 'file3'  
$ ./test_fs.x ls disk.fs   
FS Ls:  
$ ./fs_ref.x ls disk.fs   
FS Ls:  

MANUALLY TESTED.  

Shell Script: 
FS_CREATE - wrong block number (find_root_entry function not returning correctly)




