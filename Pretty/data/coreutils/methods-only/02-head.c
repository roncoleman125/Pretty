static void
write_header (const char *filename)
{
  static bool first_file = true;

  printf ("%s==> %s <==\n", (first_file ? "" : "\n"), filename);
  first_file = false;
}

