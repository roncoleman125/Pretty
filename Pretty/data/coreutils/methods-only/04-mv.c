/* Move file SOURCE onto DEST.  Handles the case when DEST is a directory.
   Treat DEST as a directory if DEST_IS_DIR.
   Return true if successful.  */

static bool
movefile (char *source, char *dest, bool dest_is_dir,
          const struct cp_options *x)
{
  bool ok;

  /* This code was introduced to handle the ambiguity in the semantics
     of mv that is induced by the varying semantics of the rename function.
     Some systems (e.g., GNU/Linux) have a rename function that honors a
     trailing slash, while others (like Solaris 5,6,7) have a rename
     function that ignores a trailing slash.  I believe the GNU/Linux
     rename semantics are POSIX and susv2 compliant.  */

  if (remove_trailing_slashes)
    strip_trailing_slashes (source);

  if (dest_is_dir)
    {
      /* Treat DEST as a directory; build the full filename.  */
      char const *src_basename = last_component (source);
      char *new_dest = file_name_concat (dest, src_basename, NULL);
      strip_trailing_slashes (new_dest);
      ok = do_move (source, new_dest, x);
      free (new_dest);
    }
  else
    {
      ok = do_move (source, dest, x);
    }

  return ok;
}

