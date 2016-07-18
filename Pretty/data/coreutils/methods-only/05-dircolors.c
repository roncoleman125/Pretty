static bool
dc_parse_file (const char *filename)
{
  bool ok;

  if (! STREQ (filename, "-") && freopen (filename, "r", stdin) == NULL)
    {
      error (0, errno, "%s", filename);
      return false;
    }

  ok = dc_parse_stream (stdin, filename);

  if (fclose (stdin) != 0)
    {
      error (0, errno, "%s", quote (filename));
      return false;
    }

  return ok;
}

