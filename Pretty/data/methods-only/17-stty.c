static unsigned long int
baud_to_value (speed_t speed)
{
  int i;

  for (i = 0; speeds[i].string != NULL; ++i)
    if (speed == speeds[i].speed)
      return speeds[i].value;
  return 0;
}

