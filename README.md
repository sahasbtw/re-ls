ls program written in C
=========================

Just as a recreational programming session.

**$ ./re-ls**
Makefile  main.c  re-ls

**$ ./re-ls -a**
.  ..  .git  .hello  Makefile  main.c  re-ls

**$ ./re-ls -l**
Makefile
main.c
re-ls

**$ ./re-ls -la**
drwxr-xr-x  3 sahas sahas  4096 Nov  6 13:09 .
drwxr-xr-x  4 sahas sahas  4096 Nov  6 12:51 ..
drwxr-xr-x  7 sahas sahas  4096 Nov  6 13:10 .git
-rw-r--r--  1 sahas sahas     0 Nov  4 12:57 .hello
-rw-r--r--  1 sahas sahas    99 Nov  6 13:05 Makefile
-rw-r--r--  1 sahas sahas  4365 Nov  6 13:04 main.c
-rwxr-xr-x  1 sahas sahas 17240 Nov  6 13:05 re-ls

