#!/bin/sh

run_test1()
{
  # make fresh virtual disk
  ./fs_make.x disk.fs 4096
  # get fs_info from reference lib
  ./fs_ref.x info disk.fs >ref.stdout
  # get fs_info from my lib
  ./test_fs.x info disk.fs >lib.stdout

  # put output files into variables
  REF_STDOUT=$(cat ref.stdout)
  LIB_STDOUT=$(cat lib.stdout)

  # compare disk info
  if [ "$REF_STDOUT" != "$LIB_STDOUT" ]; then
   echo "Stdout outputs don't match..."
   diff -u ref.stdout lib.stdout
  else
   echo "Case 1 passed!"
  fi

  # clean
  rm disk.fs
  rm ref.stdout lib.stdout
}

run_test2()
{
  # make fresh virtual disk
  ./fs_make.x disk.fs 100
  # get fs_info from reference lib
  ./fs_ref.x info disk.fs >ref.stdout
  # get fs_info from my lib
  ./test_fs.x info disk.fs >lib.stdout

  # put output files into variables
  REF_STDOUT=$(cat ref.stdout)
  LIB_STDOUT=$(cat lib.stdout)

  # compare disk info
  if [ "$REF_STDOUT" != "$LIB_STDOUT" ]; then
   echo "Case 2 outputs don't match..."
   echo "disk.fs size : 100"
   diff -u ref.stdout lib.stdout
  else
   echo "Case 2 passed!"
  fi

  # clean
  rm disk.fs
  rm ref.stdout lib.stdout
}

run_test34()
{
  # make fresh virtual disk
  ./fs_make.x disk.fs 100

  # make some empty files
  touch file1
  touch file2
  touch file3

  # add them into disk.fs
  ./test_fs.x add disk.fs file1
  ./test_fs.x add disk.fs file2
  ./test_fs.x add disk.fs file3

  # ls them out
  ./test_fs.x ls disk.fs > lib.stdout
  ./fs_ref.x ls disk.fs > ref.stdout

  # put output files into variables
  REF_STDOUT=$(cat ref.stdout)
  LIB_STDOUT=$(cat lib.stdout)

  # compare disk info
  if [ "$REF_STDOUT" != "$LIB_STDOUT" ]; then
   echo "Case 3 outputs don't match..."
   diff -u ref.stdout lib.stdout
  else
   echo "Case 3 passed!"
  fi

  # add them into disk.fs
  ./test_fs.x rm disk.fs file1
  ./test_fs.x rm disk.fs file2
  ./test_fs.x rm disk.fs file3

  # ls them out
  ./test_fs.x ls disk.fs > lib.stdout
  ./fs_ref.x ls disk.fs > ref.stdout

  # compare disk info
  if [ "$REF_STDOUT" != "$LIB_STDOUT" ]; then
   echo "Case 4 outputs don't match..."
   diff -u ref.stdout lib.stdout
  else
   echo "Case 4 passed!"
  fi

  # clean
  rm disk.fs
  rm ref.stdout lib.stdout
  rm file1 file2 file3
}

# test if it can show stat for empty file
run_test5()
{
  # make fresh virtual disk
  ./fs_make.x disk.fs 100

  # make some empty files
  touch file1

  # add them into disk.fs
  ./test_fs.x add disk.fs file1

  # have a status for a file
  ./test_fs.x stat disk.fs file1 > lib.stdout
  echo "Empty file" > ref.stdout

  # put output files into variables
  REF_STDOUT=$(cat ref.stdout)
  LIB_STDOUT=$(cat lib.stdout)

  # compare disk info
  if [ "$REF_STDOUT" != "$LIB_STDOUT" ]; then
   echo "Case 5 outputs don't match..."
   diff -u ref.stdout lib.stdout
  else
   echo "Case 5 passed!"
  fi

  # clean
  rm disk.fs
  rm ref.stdout lib.stdout
  rm file1
}

#
# Run tests
#
run_tests()
{
	# Phase 1
  echo "\n--- TESTING PHASE 1 ---"
  run_test1
  run_test2
	# Phase 2
  run_test34
  # Phase 3
  run_test5
}

run_tests
