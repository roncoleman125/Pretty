/* Print --verbose output on standard output, e.g. `new' -> `old'.
   If BACKUP_DST_NAME is non-NULL, then also indicate that it is
   the name of a backup file.  */
static void
emit_verbose (char const *src, char const *dst, char const *backup_dst_name)
{
  printf ("%s -> %s", quote_n (0, src), quote_n (1, dst));
  if (backup_dst_name)
    printf (_(" (backup: %s)"), quote (backup_dst_name));
  putchar ('\n');
}

