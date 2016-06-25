/* Read a single byte into *C from the concatenation of the input files
   named in the global array FILE_LIST.  On the first call to this
   function, the global variable IN_STREAM is expected to be an open
   stream associated with the input file INPUT_FILENAME.  If IN_STREAM
   is at end-of-file, close it and update the global variables IN_STREAM
   and INPUT_FILENAME so they correspond to the next file in the list.
   Then try to read a byte from the newly opened file.  Repeat if
   necessary until EOF is reached for the last file in FILE_LIST, then
   set *C to EOF and return.  Subsequent calls do likewise.  Return
   true if successful.  */

static bool
read_char (int *c)
{
  bool ok = true;

  *c = EOF;

  while (in_stream != NULL)	/* EOF.  */
    {
      *c = fgetc (in_stream);

      if (*c != EOF)
        break;

      ok &= check_and_close (errno);

      ok &= open_next_file ();
    }

  return ok;
}

