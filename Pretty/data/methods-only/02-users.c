/* Display a list of users on the system, according to utmp file FILENAME.
   Use read_utmp OPTIONS to read FILENAME.  */

static void
users (const char *filename, int options)
{
  size_t n_users;
  STRUCT_UTMP *utmp_buf;

  if (read_utmp (filename, &n_users, &utmp_buf, options) != 0)
    error (EXIT_FAILURE, errno, "%s", filename);

  list_entries_users (n_users, utmp_buf);

  free (utmp_buf);
}

