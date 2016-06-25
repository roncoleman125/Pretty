/* Open a file.  Return true if successful.

   With each file p, p->full_page_printed is initialized,
   see also init_fps. */

static bool
open_file (char *name, COLUMN *p)
{
  if (STREQ (name, "-"))
    {
      p->name = _("standard input");
      p->fp = stdin;
      have_read_stdin = true;
    }
  else
    {
      p->name = name;
      p->fp = fopen (name, "r");
    }
  if (p->fp == NULL)
    {
      failed_opens = true;
      if (!ignore_failed_opens)
        error (0, errno, "%s", name);
      return false;
    }
  fadvise (p->fp, FADVISE_SEQUENTIAL);
  p->status = OPEN;
  p->full_page_printed = false;
  ++total_files;
  return true;
}

