static void
out_uint_x (char *pformat, size_t prefix_len, uintmax_t arg)
{
  make_format (pformat, prefix_len, "-#0", PRIxMAX);
  printf (pformat, arg);
}
