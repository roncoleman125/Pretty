/* Process a command-line file name, for the -d option.  */
static int
process_dir (char *dir, struct savewd *wd, void *options)
{
  return (make_dir_parents (dir, wd,
                            make_ancestor, options,
                            dir_mode, announce_mkdir,
                            dir_mode_bits, owner_id, group_id, false)
          ? EXIT_SUCCESS
          : EXIT_FAILURE);
}

