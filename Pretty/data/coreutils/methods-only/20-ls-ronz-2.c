/* Request that the directory named NAME have its contents listed later.
   If REALNAME is nonzero, it will be used instead of NAME when the
   directory name is printed.  This allows symbolic links to directories
   to be treated as regular directories but still be listed under their
   real names.  NAME == NULL is used to insert a marker entry for the
   directory named in REALNAME.
   If NAME is non-NULL, we use its dev/ino information to save
   a call to stat -- when doing a recursive (-R) traversal.
   COMMAND_LINE_ARG means this directory was mentioned on the command line.  */

static void
queue_directory (char const *name, char const *realname, bool command_line_arg)
{
  struct pending *new = xmalloc (sizeof *new);
  new->realname = realname ? xstrdup (realname) : NULL;
  new->name = name ? xstrdup (name) : NULL;
  new->command_line_arg = command_line_arg;
  new->next = pending_dirs;
  pending_dirs = new;
}

