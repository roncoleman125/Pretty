/* Set up SORTED_FILE to point to the in-use entries in CWD_FILE, in order.  */

static void
initialize_ordering_vector (void)
{
  size_t i;
  for (i = 0; i < cwd_n_used; i++)
    sorted_file[i] = &cwd_file[i];
}

