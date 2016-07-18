static void
out_uint_o (char *pformat, size_t prefix_len, uintmax_t arg)
{
  make_format (pformat, prefix_len, "-#0", PRIoMAX);
  printf (pformat, arg);
}
