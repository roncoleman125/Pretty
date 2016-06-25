static speed_t
string_to_baud (const char *arg)
{
  int i;

  for (i = 0; speeds[i].string != NULL; ++i)
    if (STREQ (arg, speeds[i].string))
      return speeds[i].speed;
  return (speed_t) -1;
}

