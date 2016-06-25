/* If `linkname' is a relative name and `name' contains one or more
   leading directories, return `linkname' with those directories
   prepended; otherwise, return a copy of `linkname'.
   If `linkname' is zero, return zero.  */

static char *
make_link_name (char const *name, char const *linkname)
{
  char *linkbuf;
  size_t bufsiz;

  if (!linkname)
    return NULL;

  if (*linkname == '/')
    return xstrdup (linkname);

  /* The link is to a relative name.  Prepend any leading directory
     in `name' to the link name.  */
  linkbuf = strrchr (name, '/');
  if (linkbuf == 0)
    return xstrdup (linkname);

  bufsiz = linkbuf - name + 1;
  linkbuf = xmalloc (bufsiz + strlen (linkname) + 1);
  strncpy (linkbuf, name, bufsiz);
  strcpy (linkbuf + bufsiz, linkname);
  return linkbuf;
}

