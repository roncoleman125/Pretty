/* Set the st_author field of DEST_DESC to the st_author field of
   SRC_SB. If DEST_DESC is undefined (-1), set the st_author field
   of DST_NAME instead.  DEST_DESC must refer to the same file as
   DEST_NAME if defined.  */

static void
set_author (const char *dst_name, int dest_desc, const struct stat *src_sb)
{
  /* FIXME: Modify the following code so that it does not
     follow symbolic links.  */

  /* Preserve the st_author field.  */
  file_t file = (dest_desc < 0
                 ? file_name_lookup (dst_name, 0, 0)
                 : getdport (dest_desc));
  if (file == MACH_PORT_NULL)
    error (0, errno, _("failed to lookup file %s"), quote (dst_name));
  else
    {
      error_t err = file_chauthor (file, src_sb->st_author);
      if (err)
        error (0, err, _("failed to preserve authorship for %s"),
               quote (dst_name));
      mach_port_deallocate (mach_task_self (), file);
    }
  (void) dst_name;
  (void) dest_desc;
  (void) src_sb;
}

