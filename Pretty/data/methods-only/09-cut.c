/* Given the list of field or byte range specifications FIELDSTR, set
   MAX_RANGE_ENDPOINT and allocate and initialize the PRINTABLE_FIELD
   array.  If there is a right-open-ended range, set EOL_RANGE_START
   to its starting index.  FIELDSTR should be composed of one or more
   numbers or ranges of numbers, separated by blanks or commas.
   Incomplete ranges may be given: `-m' means `1-m'; `n-' means `n'
   through end of line.  Return true if FIELDSTR contains at least
   one field specification, false otherwise.  */

/* FIXME-someday:  What if the user wants to cut out the 1,000,000-th
   field of some huge input file?  This function shouldn't have to
   allocate a table of a million bits just so we can test every
   field < 10^6 with an array dereference.  Instead, consider using
   an adaptive approach: if the range of selected fields is too large,
   but only a few fields/byte-offsets are actually selected, use a
   hash table.  If the range of selected fields is too large, and
   too many are selected, then resort to using the range-pairs (the
   `rp' array) directly.  */

static bool
set_fields (const char *fieldstr)
{
  size_t initial = 1;		/* Value of first number in a range.  */
  size_t value = 0;		/* If nonzero, a number being accumulated.  */
  bool lhs_specified = false;
  bool rhs_specified = false;
  bool dash_found = false;	/* True if a '-' is found in this field.  */
  bool field_found = false;	/* True if at least one field spec
                                   has been processed.  */

  struct range_pair *rp = NULL;
  size_t n_rp = 0;
  size_t n_rp_allocated = 0;
  size_t i;
  bool in_digits = false;

  /* Collect and store in RP the range end points.
     It also sets EOL_RANGE_START if appropriate.  */

  while (true)
    {
      if (*fieldstr == '-')
        {
          in_digits = false;
          /* Starting a range. */
          if (dash_found)
            FATAL_ERROR (_("invalid byte or field list"));
          dash_found = true;
          fieldstr++;

          initial = (lhs_specified ? value : 1);
          value = 0;
        }
      else if (*fieldstr == ',' ||
               isblank (to_uchar (*fieldstr)) || *fieldstr == '\0')
        {
          in_digits = false;
          /* Ending the string, or this field/byte sublist. */
          if (dash_found)
            {
              dash_found = false;

              if (!lhs_specified && !rhs_specified)
                FATAL_ERROR (_("invalid range with no endpoint: -"));

              /* A range.  Possibilities: -n, m-n, n-.
                 In any case, `initial' contains the start of the range. */
              if (!rhs_specified)
                {
                  /* `n-'.  From `initial' to end of line. */
                  eol_range_start = initial;
                  field_found = true;
                }
              else
                {
                  /* `m-n' or `-n' (1-n). */
                  if (value < initial)
                    FATAL_ERROR (_("invalid decreasing range"));

                  /* Is there already a range going to end of line? */
                  if (eol_range_start != 0)
                    {
                      /* Yes.  Is the new sequence already contained
                         in the old one?  If so, no processing is
                         necessary. */
                      if (initial < eol_range_start)
                        {
                          /* No, the new sequence starts before the
                             old.  Does the old range going to end of line
                             extend into the new range?  */
                          if (eol_range_start <= value)
                            {
                              /* Yes.  Simply move the end of line marker. */
                              eol_range_start = initial;
                            }
                          else
                            {
                              /* No.  A simple range, before and disjoint from
                                 the range going to end of line.  Fill it. */
                              ADD_RANGE_PAIR (rp, initial, value);
                            }

                          /* In any case, some fields were selected. */
                          field_found = true;
                        }
                    }
                  else
                    {
                      /* There is no range going to end of line. */
                      ADD_RANGE_PAIR (rp, initial, value);
                      field_found = true;
                    }
                  value = 0;
                }
            }
          else
            {
              /* A simple field number, not a range. */
              ADD_RANGE_PAIR (rp, value, value);
              value = 0;
              field_found = true;
            }

          if (*fieldstr == '\0')
            {
              break;
            }

          fieldstr++;
          lhs_specified = false;
          rhs_specified = false;
        }
      else if (ISDIGIT (*fieldstr))
        {
          /* Record beginning of digit string, in case we have to
             complain about it.  */
          static char const *num_start;
          if (!in_digits || !num_start)
            num_start = fieldstr;
          in_digits = true;

          if (dash_found)
            rhs_specified = 1;
          else
            lhs_specified = 1;

          /* Detect overflow.  */
          if (!DECIMAL_DIGIT_ACCUMULATE (value, *fieldstr - '0', size_t))
            {
              /* In case the user specified -c$(echo 2^64|bc),22,
                 complain only about the first number.  */
              /* Determine the length of the offending number.  */
              size_t len = strspn (num_start, "0123456789");
              char *bad_num = xstrndup (num_start, len);
              if (operating_mode == byte_mode)
                error (0, 0,
                       _("byte offset %s is too large"), quote (bad_num));
              else
                error (0, 0,
                       _("field number %s is too large"), quote (bad_num));
              free (bad_num);
              exit (EXIT_FAILURE);
            }

          fieldstr++;
        }
      else
        FATAL_ERROR (_("invalid byte or field list"));
    }

  max_range_endpoint = 0;
  for (i = 0; i < n_rp; i++)
    {
      if (rp[i].hi > max_range_endpoint)
        max_range_endpoint = rp[i].hi;
    }

  /* Allocate an array large enough so that it may be indexed by
     the field numbers corresponding to all finite ranges
     (i.e. `2-6' or `-4', but not `5-') in FIELDSTR.  */

  printable_field = xzalloc (max_range_endpoint / CHAR_BIT + 1);

  qsort (rp, n_rp, sizeof (rp[0]), compare_ranges);

  /* Set the array entries corresponding to integers in the ranges of RP.  */
  for (i = 0; i < n_rp; i++)
    {
      size_t j;
      size_t rsi_candidate;

      /* Record the range-start indices, i.e., record each start
         index that is not part of any other (lo..hi] range.  */
      rsi_candidate = complement ? rp[i].hi + 1 : rp[i].lo;
      if (output_delimiter_specified
          && !is_printable_field (rsi_candidate))
        mark_range_start (rsi_candidate);

      for (j = rp[i].lo; j <= rp[i].hi; j++)
        mark_printable_field (j);
    }

  if (output_delimiter_specified
      && !complement
      && eol_range_start && !is_printable_field (eol_range_start))
    mark_range_start (eol_range_start);

  free (rp);

  return field_found;
}

