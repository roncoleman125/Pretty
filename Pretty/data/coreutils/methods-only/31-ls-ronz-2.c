/* Put the name of the file that FILENAME is a symbolic link to
   into the LINKNAME field of `f'.  COMMAND_LINE_ARG indicates whether
   FILENAME is a command-line argument.  */

static void
get_link_name (char const *filename, struct fileinfo *f, bool command_line_arg)
{
  f->linkname = areadlink_with_size (filename, f->stat.st_size);
  if (f->linkname == NULL)
    file_failure (command_line_arg, _("cannot read symbolic link %s"),
                  filename);
}

