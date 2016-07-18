/* Test whether there have been errors on in_stream, and close it if
   it is not standard input.  Return false if there has been an error
   on in_stream or stdout; return true otherwise.  This function will
   report more than one error only if both a read and a write error
   have occurred.  IN_ERRNO, if nonzero, is the error number
   corresponding to the most recent action for IN_STREAM.  */

static bool
check_and_close (int in_errno)
{
  bool ok = true;

  if (in_stream != NULL)
    {
      if (ferror (in_stream))
        {
          error (0, in_errno, _("%s: read error"), input_filename);
          if (! STREQ (file_list[-1], "-"))
            fclose (in_stream);
          ok = false;
        }
      else if (! STREQ (file_list[-1], "-") && fclose (in_stream) != 0)
        {
          error (0, errno, "%s", input_filename);
          ok = false;
        }

      in_stream = NULL;
    }

  if (ferror (stdout))
    {
      error (0, 0, _("write error"));
      ok = false;
    }

  return ok;
}

