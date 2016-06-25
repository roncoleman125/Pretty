/* Display a list of who is on the system, according to utmp file FILENAME. */

static void
short_pinky (const char *filename,
             const int argc_names, char *const argv_names[])
{
  size_t n_users;
  STRUCT_UTMP *utmp_buf;

  if (read_utmp (filename, &n_users, &utmp_buf, 0) != 0)
    error (EXIT_FAILURE, errno, "%s", filename);

  scan_entries (n_users, utmp_buf, argc_names, argv_names);
}

