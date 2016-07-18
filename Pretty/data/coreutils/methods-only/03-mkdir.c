/* Process a command-line file name.  */
static int
process_dir (char *dir, struct savewd *wd, void *options)
{
  struct mkdir_options const *o = options;
  return (make_dir_parents (dir, wd, o->make_ancestor_function, options,
                            o->mode, announce_mkdir,
                            o->mode_bits, (uid_t) -1, (gid_t) -1, true)
          ? EXIT_SUCCESS
          : EXIT_FAILURE);
}

