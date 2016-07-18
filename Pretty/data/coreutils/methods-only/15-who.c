/* Display a list of who is on the system, according to utmp file FILENAME.
   Use read_utmp OPTIONS to read the file.  */
static void
who (const char *filename, int options)
{
  size_t n_users;
  STRUCT_UTMP *utmp_buf;

  if (read_utmp (filename, &n_users, &utmp_buf, options) != 0)
    error (EXIT_FAILURE, errno, "%s", filename);

  if (short_list)
    list_entries_who (n_users, utmp_buf);
  else
    scan_entries (n_users, utmp_buf);

  free (utmp_buf);
}

