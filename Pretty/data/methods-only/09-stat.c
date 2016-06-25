static int
out_minus_zero (char *pformat, size_t prefix_len)
{
  make_format (pformat, prefix_len, "'-+ 0", ".0f");
  return printf (pformat, -0.25);
}

