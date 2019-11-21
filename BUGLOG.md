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
