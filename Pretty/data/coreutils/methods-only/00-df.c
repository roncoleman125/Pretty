static void
print_header (void)
{
  char buf[MAX (LONGEST_HUMAN_READABLE + 1, INT_BUFSIZE_BOUND (uintmax_t))];

  if (print_type)
    /* TRANSLATORS:
       For best results (df header/column alignment), ensure that
       your translation has the same length as the original.  */
    fputs (_("Filesystem    Type"), stdout);
  else
    fputs (_("Filesystem        "), stdout);

  if (inode_format)
    /* TRANSLATORS:
       For best results (df header/column alignment), ensure that
       your translation has the same length as the original.
       Also, each column name translation should end at the same
       column as the corresponding original.  */
    fputs (_("    Inodes   IUsed   IFree IUse%"), stdout);
  else if (human_output_opts & human_autoscale)
    {
      if (human_output_opts & human_base_1024)
        fputs (_("    Size  Used Avail Use%"), stdout);
      else
        fputs (_("     Size   Used  Avail Use%"), stdout);
    }
  else if (posix_format)
    printf (_(" %s-blocks      Used Available Capacity"),
            umaxtostr (output_block_size, buf));
  else
    {
      int opts = (human_suppress_point_zero
                  | human_autoscale | human_SI
                  | (human_output_opts
                     & (human_group_digits | human_base_1024 | human_B)));

      /* Prefer the base that makes the human-readable value more exact,
         if there is a difference.  */

      uintmax_t q1000 = output_block_size;
      uintmax_t q1024 = output_block_size;
      bool divisible_by_1000;
      bool divisible_by_1024;

      do
        {
          divisible_by_1000 = q1000 % 1000 == 0;  q1000 /= 1000;
          divisible_by_1024 = q1024 % 1024 == 0;  q1024 /= 1024;
        }
      while (divisible_by_1000 & divisible_by_1024);

      if (divisible_by_1000 < divisible_by_1024)
        opts |= human_base_1024;
      if (divisible_by_1024 < divisible_by_1000)
        opts &= ~human_base_1024;
      if (! (opts & human_base_1024))
        opts |= human_B;

      printf (_(" %4s-blocks      Used Available Use%%"),
              human_readable (output_block_size, buf, opts, 1, 1));
    }

  fputs (_(" Mounted on\n"), stdout);
}

