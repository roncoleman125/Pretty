/* Display a space listing for the disk device with absolute file name DISK.
   If MOUNT_POINT is non-NULL, it is the name of the root of the
   file system on DISK.
   If STAT_FILE is non-null, it is the name of a file within the file
   system that the user originally asked for; this provides better
   diagnostics, and sometimes it provides better results on networked
   file systems that give different free-space results depending on
   where in the file system you probe.
   If FSTYPE is non-NULL, it is the type of the file system on DISK.
   If MOUNT_POINT is non-NULL, then DISK may be NULL -- certain systems may
   not be able to produce statistics in this case.
   ME_DUMMY and ME_REMOTE are the mount entry flags.  */

static void
show_dev (char const *disk, char const *mount_point,
          char const *stat_file, char const *fstype,
          bool me_dummy, bool me_remote,
          const struct fs_usage *force_fsu)
{
  struct fs_usage fsu;
  char buf[3][LONGEST_HUMAN_READABLE + 2];
  int width;
  int col1_adjustment = 0;
  int use_width;
  uintmax_t input_units;
  uintmax_t output_units;
  uintmax_t total;
  uintmax_t available;
  bool negate_available;
  uintmax_t available_to_root;
  uintmax_t used;
  bool negate_used;
  double pct = -1;

  if (me_remote && show_local_fs)
    return;

  if (me_dummy && !show_all_fs && !show_listed_fs)
    return;

  if (!selected_fstype (fstype) || excluded_fstype (fstype))
    return;

  /* If MOUNT_POINT is NULL, then the file system is not mounted, and this
     program reports on the file system that the special file is on.
     It would be better to report on the unmounted file system,
     but statfs doesn't do that on most systems.  */
  if (!stat_file)
    stat_file = mount_point ? mount_point : disk;

  if (force_fsu)
    fsu = *force_fsu;
  else if (get_fs_usage (stat_file, disk, &fsu))
    {
      error (0, errno, "%s", quote (stat_file));
      exit_status = EXIT_FAILURE;
      return;
    }

  if (fsu.fsu_blocks == 0 && !show_all_fs && !show_listed_fs)
    return;

  if (! file_systems_processed)
    {
      file_systems_processed = true;
      print_header ();
    }

  if (! disk)
    disk = "-";			/* unknown */
  if (! fstype)
    fstype = "-";		/* unknown */

  /* df.c reserved 5 positions for fstype,
     but that does not suffice for type iso9660 */
  if (print_type)
    {
      size_t disk_name_len = strlen (disk);
      size_t fstype_len = strlen (fstype);
      if (disk_name_len + fstype_len < 18)
        printf ("%s%*s  ", disk, 18 - (int) disk_name_len, fstype);
      else if (!posix_format)
        printf ("%s\n%18s  ", disk, fstype);
      else
        printf ("%s %s", disk, fstype);
    }
  else
    {
      if (strlen (disk) > 20 && !posix_format)
        printf ("%s\n%20s", disk, "");
      else
        printf ("%-20s", disk);
    }

  if (inode_format)
    {
      width = 7;
      use_width = 5;
      input_units = output_units = 1;
      total = fsu.fsu_files;
      available = fsu.fsu_ffree;
      negate_available = false;
      available_to_root = available;

      if (known_value (total))
        grand_fsu.fsu_files += total;
      if (known_value (available))
        grand_fsu.fsu_ffree += available;
    }
  else
    {
      if (human_output_opts & human_autoscale)
        width = 5 + ! (human_output_opts & human_base_1024);
      else
        {
          width = 9;
          if (posix_format)
            {
              uintmax_t b;
              col1_adjustment = -3;
              for (b = output_block_size; 9 < b; b /= 10)
                col1_adjustment++;
            }
        }
      use_width = ((posix_format
                    && ! (human_output_opts & human_autoscale))
                   ? 8 : 4);
      input_units = fsu.fsu_blocksize;
      output_units = output_block_size;
      total = fsu.fsu_blocks;
      available = fsu.fsu_bavail;
      negate_available = (fsu.fsu_bavail_top_bit_set
                          && known_value (available));
      available_to_root = fsu.fsu_bfree;

      if (known_value (total))
        grand_fsu.fsu_blocks += input_units * total;
      if (known_value (available_to_root))
        grand_fsu.fsu_bfree  += input_units * available_to_root;
      if (known_value (available))
        add_uint_with_neg_flag (&grand_fsu.fsu_bavail,
                                &grand_fsu.fsu_bavail_top_bit_set,
                                input_units * available, negate_available);
    }

  used = UINTMAX_MAX;
  negate_used = false;
  if (known_value (total) && known_value (available_to_root))
    {
      used = total - available_to_root;
      negate_used = (total < available_to_root);
    }

  printf (" %*s %*s %*s ",
          width + col1_adjustment,
          df_readable (false, total,
                       buf[0], input_units, output_units),
          width, df_readable (negate_used, used,
                              buf[1], input_units, output_units),
          width, df_readable (negate_available, available,
                              buf[2], input_units, output_units));

  if (! known_value (used) || ! known_value (available))
    ;
  else if (!negate_used
           && used <= TYPE_MAXIMUM (uintmax_t) / 100
           && used + available != 0
           && (used + available < used) == negate_available)
    {
      uintmax_t u100 = used * 100;
      uintmax_t nonroot_total = used + available;
      pct = u100 / nonroot_total + (u100 % nonroot_total != 0);
    }
  else
    {
      /* The calculation cannot be done easily with integer
         arithmetic.  Fall back on floating point.  This can suffer
         from minor rounding errors, but doing it exactly requires
         multiple precision arithmetic, and it's not worth the
         aggravation.  */
      double u = negate_used ? - (double) - used : used;
      double a = negate_available ? - (double) - available : available;
      double nonroot_total = u + a;
      if (nonroot_total)
        {
          long int lipct = pct = u * 100 / nonroot_total;
          double ipct = lipct;

          /* Like `pct = ceil (dpct);', but avoid ceil so that
             the math library needn't be linked.  */
          if (ipct - 1 < pct && pct <= ipct + 1)
            pct = ipct + (ipct < pct);
        }
    }

  if (0 <= pct)
    printf ("%*.0f%%", use_width - 1, pct);
  else
    printf ("%*s", use_width, "- ");

  if (mount_point)
    {
      /* Don't print the first directory name in MOUNT_POINT if it's an
         artifact of an automounter.  This is a bit too aggressive to be
         the default.  */
      if (strncmp ("/auto/", mount_point, 6) == 0)
        mount_point += 5;
      else if (strncmp ("/tmp_mnt/", mount_point, 9) == 0)
        mount_point += 8;
      printf (" %s", mount_point);
    }
  putchar ('\n');
}

