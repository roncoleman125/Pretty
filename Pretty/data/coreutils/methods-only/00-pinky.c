/* Count and return the number of ampersands in STR.  */

static size_t
count_ampersands (const char *str)
{
  size_t count = 0;
  do
    {
      if (*str == '&')
        count++;
    } while (*str++);
  return count;
}

