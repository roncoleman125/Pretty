static size_t
abmon_init (void)
{
  required_mon_width = MAX_MON_WIDTH;
  size_t curr_max_width;
  do
    {
      curr_max_width = required_mon_width;
      required_mon_width = 0;
      for (int i = 0; i < 12; i++)
        {
          size_t width = curr_max_width;

          size_t req = mbsalign (nl_langinfo (ABMON_1 + i),
                                 abmon[i], sizeof (abmon[i]),
                                 &width, MBS_ALIGN_LEFT, 0);

          if (req == (size_t) -1 || req >= sizeof (abmon[i]))
            {
              required_mon_width = 0; /* ignore precomputed strings.  */
              return required_mon_width;
            }

          required_mon_width = MAX (required_mon_width, width);
        }
    }
  while (curr_max_width > required_mon_width);

  return required_mon_width;
}

