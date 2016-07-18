/* Print the file name of `f' with appropriate quoting.
   Also print file size, inode number, and filetype indicator character,
   as requested by switches.  */

static size_t
print_file_name_and_frills (const struct fileinfo *f, size_t start_col)
{
  char buf[MAX (LONGEST_HUMAN_READABLE + 1, INT_BUFSIZE_BOUND (uintmax_t))];

  set_normal_color ();

  if (print_inode)
    printf ("%*s ", format == with_commas ? 0 : inode_number_width,
            format_inode (buf, sizeof buf, f));

  if (print_block_size)
    printf ("%*s ", format == with_commas ? 0 : block_size_width,
            ! f->stat_ok ? "?"
            : human_readable (ST_NBLOCKS (f->stat), buf, human_output_opts,
                              ST_NBLOCKSIZE, output_block_size));

  if (print_scontext)
    printf ("%*s ", format == with_commas ? 0 : scontext_width, f->scontext);

  size_t width = print_name_with_quoting (f, false, NULL, start_col);

  if (indicator_style != none)
    width += print_type_indicator (f->stat_ok, f->stat.st_mode, f->filetype);

  return width;
}

