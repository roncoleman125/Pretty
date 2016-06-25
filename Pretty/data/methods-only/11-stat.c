/* Print the context information of FILENAME, and return true iff the
   context could not be obtained.  */
static bool ATTRIBUTE_WARN_UNUSED_RESULT
out_file_context (char *pformat, size_t prefix_len, char const *filename)
{
  char *scontext;
  bool fail = false;

  if ((follow_links
       ? getfilecon (filename, &scontext)
       : lgetfilecon (filename, &scontext)) < 0)
    {
      error (0, errno, _("failed to get security context of %s"),
             quote (filename));
      scontext = NULL;
      fail = true;
    }
  strcpy (pformat + prefix_len, "s");
  printf (pformat, (scontext ? scontext : "?"));
  if (scontext)
    freecon (scontext);
  return fail;
}

