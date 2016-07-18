static int
out_int (char *pformat, size_t prefix_len, intmax_t arg)
{
  make_format (pformat, prefix_len, "'-+ 0", PRIdMAX);
  return printf (pformat, arg);
}
