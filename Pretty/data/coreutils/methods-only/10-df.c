/* Add FSTYPE to the list of file system types to display. */

static void
add_fs_type (const char *fstype)
{
  struct fs_type_list *fsp;

  fsp = xmalloc (sizeof *fsp);
  fsp->fs_name = (char *) fstype;
  fsp->fs_next = fs_select_list;
  fs_select_list = fsp;
}

