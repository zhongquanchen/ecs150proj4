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
--- Running run_fs_simple_create ---
Info: Created virtual disk 'test.fs' with '10' data blocks

Info: 1+0 records in

1+0 records out

10 bytes copied, 0.00253676 s, 3.9 kB/s

Info: Wrote file 'test-file-1' (10/10 bytes)

Fail: got 'file: test-file-1, size: 10, data_blk: 0' but expected 'file: test-file-1, size: 10, data_blk: 1'

Possible problem:
fs_create() line :211
uint16_t entry = fs_find_root_entry("");
recieve entry wrong.

