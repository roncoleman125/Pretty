static void
free_spareline (void)
{
  size_t i;

  for (i = 0; i < ARRAY_CARDINALITY (spareline); i++)
    {
      if (spareline[i])
        {
          freeline (spareline[i]);
          free (spareline[i]);
        }
    }
}

