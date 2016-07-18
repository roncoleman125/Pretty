/* Print mount point.  Return zero upon success, nonzero upon failure.  */
static bool ATTRIBUTE_WARN_UNUSED_RESULT
out_mount_point (char const *filename, char *pformat, size_t prefix_len,
                 const struct stat *statp)
{

  char const *np = "?", *bp = NULL;
  char *mp = NULL;
  bool fail = true;

  /* Look for bind mounts first.  Note we output the immediate alias,
     rather than further resolving to a base device mount point.  */
  if (follow_links || !S_ISLNK (statp->st_mode))
    {
      char *resolved = canonicalize_file_name (filename);
      if (!resolved)
        {
          error (0, errno, _("failed to canonicalize %s"), quote (filename));
          goto print_mount_point;
        }
      bp = find_bind_mount (resolved);
      free (resolved);
      if (bp)
        {
          fail = false;
          goto print_mount_point;
        }
    }

  /* If there is no direct bind mount, then navigate
     back up the tree looking for a device change.
     Note we don't detect if any of the directory components
     are bind mounted to the same device, but that's OK
     since we've not directly queried them.  */
  if ((mp = find_mount_point (filename, statp)))
    {
      /* This dir might be bind mounted to another device,
         so we resolve the bound source in that case also.  */
      bp = find_bind_mount (mp);
      fail = false;
    }

print_mount_point:

  out_string (pformat, prefix_len, bp ? bp : mp ? mp : np);
  free (mp);
  return fail;
}

