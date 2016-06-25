/* Set the attributes of file or directory NAME.
   Return true if successful.  */

static bool
change_attributes (char const *name)
{
  bool ok = false;
  /* chown must precede chmod because on some systems,
     chown clears the set[ug]id bits for non-superusers,
     resulting in incorrect permissions.
     On System V, users can give away files with chown and then not
     be able to chmod them.  So don't give files away.

     We don't normally ignore errors from chown because the idea of
     the install command is that the file is supposed to end up with
     precisely the attributes that the user specified (or defaulted).
     If the file doesn't end up with the group they asked for, they'll
     want to know.  */

  if (! (owner_id == (uid_t) -1 && group_id == (gid_t) -1)
      && lchown (name, owner_id, group_id) != 0)
    error (0, errno, _("cannot change ownership of %s"), quote (name));
  else if (chmod (name, mode) != 0)
    error (0, errno, _("cannot change permissions of %s"), quote (name));
  else
    ok = true;

  if (use_default_selinux_context)
    setdefaultfilecon (name);

  return ok;
}

