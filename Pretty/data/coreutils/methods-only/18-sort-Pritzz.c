/* Close FP, whose name is FILE, and report any errors.  */

static void
xfclose (FILE *fp, char const *file)
{
  switch (fileno (fp))
    {
    case STDIN_FILENO:
      /* Allow reading stdin from tty more than once.  */
      if (feof (fp))
        clearerr (fp);
      break;

    case STDOUT_FILENO:
      /* Don't close stdout just yet.  close_stdout does that.  */
      if (fflush (fp) != 0)
        die (_("fflush failed"), file);
      break;

    default:
      if (fclose (fp) != 0)
        die (_("close failed"), file);
      break;
    }
}

