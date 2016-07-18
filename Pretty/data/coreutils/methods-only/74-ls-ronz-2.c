/* Returns whether any color sequence was printed. */
static bool
print_color_indicator (const struct fileinfo *f, bool symlink_target)
{
  enum indicator_no type;
  struct color_ext_type *ext;	/* Color extension */
  size_t len;			/* Length of name */

  const char* name;
  mode_t mode;
  int linkok;
  if (symlink_target)
    {
      name = f->linkname;
      mode = f->linkmode;
      linkok = f->linkok - 1;
    }
  else
    {
      name = f->name;
      mode = FILE_OR_LINK_MODE (f);
      linkok = f->linkok;
    }

  /* Is this a nonexistent file?  If so, linkok == -1.  */

  if (linkok == -1 && color_indicator[C_MISSING].string != NULL)
    type = C_MISSING;
  else if (!f->stat_ok)
    {
      static enum indicator_no filetype_indicator[] = FILETYPE_INDICATORS;
      type = filetype_indicator[f->filetype];
    }
  else
    {
      if (S_ISREG (mode))
        {
          type = C_FILE;

          if ((mode & S_ISUID) != 0 && is_colored (C_SETUID))
            type = C_SETUID;
          else if ((mode & S_ISGID) != 0 && is_colored (C_SETGID))
            type = C_SETGID;
          else if (is_colored (C_CAP) && f->has_capability)
            type = C_CAP;
          else if ((mode & S_IXUGO) != 0 && is_colored (C_EXEC))
            type = C_EXEC;
          else if ((1 < f->stat.st_nlink) && is_colored (C_MULTIHARDLINK))
            type = C_MULTIHARDLINK;
        }
      else if (S_ISDIR (mode))
        {
          type = C_DIR;

          if ((mode & S_ISVTX) && (mode & S_IWOTH)
              && is_colored (C_STICKY_OTHER_WRITABLE))
            type = C_STICKY_OTHER_WRITABLE;
          else if ((mode & S_IWOTH) != 0 && is_colored (C_OTHER_WRITABLE))
            type = C_OTHER_WRITABLE;
          else if ((mode & S_ISVTX) != 0 && is_colored (C_STICKY))
            type = C_STICKY;
        }
      else if (S_ISLNK (mode))
        type = ((!linkok
                 && (!strncmp (color_indicator[C_LINK].string, "target", 6)
                     || color_indicator[C_ORPHAN].string))
                ? C_ORPHAN : C_LINK);
      else if (S_ISFIFO (mode))
        type = C_FIFO;
      else if (S_ISSOCK (mode))
        type = C_SOCK;
      else if (S_ISBLK (mode))
        type = C_BLK;
      else if (S_ISCHR (mode))
        type = C_CHR;
      else if (S_ISDOOR (mode))
        type = C_DOOR;
      else
        {
          /* Classify a file of some other type as C_ORPHAN.  */
          type = C_ORPHAN;
        }
    }

  /* Check the file's suffix only if still classified as C_FILE.  */
  ext = NULL;
  if (type == C_FILE)
    {
      /* Test if NAME has a recognized suffix.  */

      len = strlen (name);
      name += len;		/* Pointer to final \0.  */
      for (ext = color_ext_list; ext != NULL; ext = ext->next)
        {
          if (ext->ext.len <= len
              && strncmp (name - ext->ext.len, ext->ext.string,
                          ext->ext.len) == 0)
            break;
        }
    }

  {
    const struct bin_str *const s
      = ext ? &(ext->seq) : &color_indicator[type];
    if (s->string != NULL)
      {
        /* Need to reset so not dealing with attribute combinations */
        if (is_colored (C_NORM))
          restore_default_color ();
        put_indicator (&color_indicator[C_LEFT]);
        put_indicator (s);
        put_indicator (&color_indicator[C_RIGHT]);
        return true;
      }
    else
      return false;
  }
}

