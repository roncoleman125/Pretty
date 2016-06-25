static void
out_string (char *pformat, size_t prefix_len, char const *arg)
{
  make_format (pformat, prefix_len, "-", "s");
  printf (pformat, arg);
}
