static void
long_pinky (const int argc_names, char *const argv_names[])
{
  int i;

  for (i = 0; i < argc_names; i++)
    print_long_entry (argv_names[i]);
}

