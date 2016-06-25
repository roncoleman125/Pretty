static int
optc_to_fileno (int c)
{
  int ret = -1;

  switch (c)
    {
    case 'e':
      ret = STDERR_FILENO;
      break;
    case 'i':
      ret = STDIN_FILENO;
      break;
    case 'o':
      ret = STDOUT_FILENO;
      break;
    }

  return ret;
}

