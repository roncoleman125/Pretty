static char *
human_size (size_t n)
{
  static char hbuf[LONGEST_HUMAN_READABLE + 1];
  int human_opts =
    (human_autoscale | human_round_to_nearest | human_base_1024
     | human_space_before_unit | human_SI | human_B);
  return human_readable (n, hbuf, human_opts, 1, 1);
}

