/* Note currently for glibc (2.3.5) the following call does not change the
   the buffer size, and more problematically does not give any indication
   that the new size request was ignored:

       setvbuf (stdout, (char*)NULL, _IOFBF, 8192);

   The ISO C99 standard section 7.19.5.6 on the setvbuf function says:

   ... If buf is not a null pointer, the array it points to _may_ be used
   instead of a buffer allocated by the setvbuf function and the argument
   size specifies the size of the array; otherwise, size _may_ determine
   the size of a buffer allocated by the setvbuf function. ...

   Obviously some interpret the above to mean setvbuf(....,size)
   is only a hint from the application which I don't agree with.

   FreeBSD's libc seems more sensible in this regard. From the man page:

   The size argument may be given as zero to obtain deferred optimal-size
   buffer allocation as usual.  If it is not zero, then except for
   unbuffered files, the buf argument should point to a buffer at least size
   bytes long; this buffer will be used instead of the current buffer.  (If
   the size argument is not zero but buf is NULL, a buffer of the given size
   will be allocated immediately, and released on close.  This is an extension
   to ANSI C; portable code should use a size of 0 with any NULL buffer.)
   --------------------
   Another issue is that on glibc-2.7 the following doesn't buffer
   the first write if it's greater than 1 byte.

       setvbuf(stdout,buf,_IOFBF,127);

   Now the POSIX standard says that "allocating a buffer of size bytes does
   not necessarily imply that all of size bytes are used for the buffer area".
   However I think it's just a buggy implementation due to the various
   inconsistencies with write sizes and subsequent writes.  */

static const char *
fileno_to_name (const int fd)
{
  const char *ret = NULL;

  switch (fd)
    {
    case 0:
      ret = "stdin";
      break;
    case 1:
      ret = "stdout";
      break;
    case 2:
      ret = "stderr";
      break;
    default:
      ret = "unknown";
      break;
    }

  return ret;
}

