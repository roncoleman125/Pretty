static bool
scan_zeros (struct item *k)
{
  /* Ignore strings that have already been printed.  */
  if (k->count == 0 && k->str)
    {
      if (head == NULL)
        head = k;
      else
        zeros->qlink = k;

      zeros = k;
    }

  return false;
}

