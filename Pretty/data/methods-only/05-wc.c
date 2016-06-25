/* Return a print width suitable for the NFILES files whose status is
   recorded in FSTATUS.  Optimize the same special case that
   get_input_fstatus optimizes.  */

static int
compute_number_width (int nfiles, struct fstatus const *fstatus)
{
  int width = 1;

  if (0 < nfiles && fstatus[0].failed <= 0)
    {
      int minimum_width = 1;
      uintmax_t regular_total = 0;
      int i;

      for (i = 0; i < nfiles; i++)
        if (! fstatus[i].failed)
          {
            if (S_ISREG (fstatus[i].st.st_mode))
              regular_total += fstatus[i].st.st_size;
            else
              minimum_width = 7;
          }

      for (; 10 <= regular_total; regular_total /= 10)
        width++;
      if (width < minimum_width)
        width = minimum_width;
    }

  return width;
}


