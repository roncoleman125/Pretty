static void
clear_files (void)
{
  size_t i;

  for (i = 0; i < cwd_n_used; i++)
    {
      struct fileinfo *f = sorted_file[i];
      free (f->name);
      free (f->linkname);
      if (f->scontext != UNKNOWN_SECURITY_CONTEXT)
        freecon (f->scontext);
    }

  cwd_n_used = 0;
  any_has_acl = false;
  inode_number_width = 0;
  block_size_width = 0;
  nlink_width = 0;
  owner_width = 0;
  group_width = 0;
  author_width = 0;
  scontext_width = 0;
  major_device_number_width = 0;
  minor_device_number_width = 0;
  file_size_width = 0;
}
