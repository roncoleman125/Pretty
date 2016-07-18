/* Return <0 if the join field in LINE1 compares less than the one in LINE2;
   >0 if it compares greater; 0 if it compares equal.
   Report an error and exit if the comparison fails.
   Use join fields JF_1 and JF_2 respectively.  */

static int
keycmp (struct line const *line1, struct line const *line2,
        size_t jf_1, size_t jf_2)
{
  /* Start of field to compare in each file.  */
  char *beg1;
  char *beg2;

  size_t len1;
  size_t len2;		/* Length of fields to compare.  */
  int diff;

  if (jf_1 < line1->nfields)
    {
      beg1 = line1->fields[jf_1].beg;
      len1 = line1->fields[jf_1].len;
    }
  else
    {
      beg1 = NULL;
      len1 = 0;
    }

  if (jf_2 < line2->nfields)
    {
      beg2 = line2->fields[jf_2].beg;
      len2 = line2->fields[jf_2].len;
    }
  else
    {
      beg2 = NULL;
      len2 = 0;
    }

  if (len1 == 0)
    return len2 == 0 ? 0 : -1;
  if (len2 == 0)
    return 1;

  if (ignore_case)
    {
      /* FIXME: ignore_case does not work with NLS (in particular,
         with multibyte chars).  */
      diff = memcasecmp (beg1, beg2, MIN (len1, len2));
    }
  else
    {
      if (hard_LC_COLLATE)
        return xmemcoll (beg1, len1, beg2, len2);
      diff = memcmp (beg1, beg2, MIN (len1, len2));
    }

  if (diff)
    return diff;
  return len1 < len2 ? -1 : len1 != len2;
}

