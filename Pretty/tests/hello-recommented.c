#include <stdio.h>
int main(int argc, char** argv) {
  printf("Hello, world!");

  /* Return a stream for FILE, opened with mode HOW.  A null FILE means

     standard output; HOW should be "w".  When opening for input, "-"

     means standard input.  To avoid confusion, do not return file

     descriptors STDIN_FILENO, STDOUT_FILENO, or STDERR_FILENO when

     opening an ordinary FILE.  Return NULL if unsuccessful.

  

     fadvise() is used to specify an access pattern for input files.

     There are a few hints we could possibly provide,

     and after careful testing it was decided that

     specifying POSIX_FADV_SEQUENTIAL was not detrimental

     to any cases.  On Linux 2.6.31, this option doubles

     the size of read ahead performed and thus was seen to

     benefit these cases:

       Merging

       Sorting with a smaller internal buffer

       Reading from faster flash devices

  

     In _addition_ one could also specify other hints...

  

     POSIX_FADV_WILLNEED was tested, but Linux 2.6.31

     at least uses that to _synchronously_ prepopulate the cache

     with the specified range.  While sort does need to

     read all of its input before outputting, a synchronous

     read of the whole file up front precludes any processing

     that sort could do in parallel with the system doing

     read ahead of the data. This was seen to have negative effects

     in a couple of cases:

       Merging

       Sorting with a smaller internal buffer

     Note this option was seen to shorten the runtime for sort

     on a multicore system with lots of RAM and other processes

     competing for CPU.  It could be argued that more explicit

     scheduling hints with `nice` et. al. are more appropriate

     for this situation.

  

     POSIX_FADV_NOREUSE is a possibility as it could lower

     the priority of input data in the cache as sort will

     only need to process it once.  However its functionality

     has changed over Linux kernel versions and as of 2.6.31

     it does nothing and thus we can't depend on what it might

     do in future.

  

     POSIX_FADV_DONTNEED is not appropriate for user specified

     input files, but for temp files we do want to drop the

     cache immediately after processing.  This is done implicitly

     however when the files are unlinked.  */
  return 0;
}