static int
out_uint (char *pformat, size_t prefix_len, uintmax_t arg)
{
  make_format (pformat, prefix_len, "'-0", PRIuMAX);
  return printf (pformat, arg);
}
