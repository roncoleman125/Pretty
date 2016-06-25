/* For LINE, output a debugging line that underlines KEY in LINE.
   If KEY is null, underline the whole line.  */

static void
debug_key (struct line const *line, struct keyfield const *key)
{
  char *text = line->text;
  char *beg = text;
  char *lim = text + line->length - 1;

  if (key)
    {
      if (key->sword != SIZE_MAX)
        beg = begfield (line, key);
      if (key->eword != SIZE_MAX)
        lim = limfield (line, key);

      if (key->skipsblanks || key->month || key_numeric (key))
        {
          char saved = *lim;
          *lim = '\0';

          while (blanks[to_uchar (*beg)])
            beg++;

          char *tighter_lim = beg;

          if (lim < beg)
            tighter_lim = lim;
          else if (key->month)
            getmonth (beg, &tighter_lim);
          else if (key->general_numeric)
            ignore_value (strtold (beg, &tighter_lim));
          else if (key->numeric || key->human_numeric)
            {
              char *p = beg + (beg < lim && *beg == '-');
              bool found_digit = false;
              unsigned char ch;

              do
                {
                  while (ISDIGIT (ch = *p++))
                    found_digit = true;
                }
              while (ch == thousands_sep);

              if (ch == decimal_point)
                while (ISDIGIT (ch = *p++))
                  found_digit = true;

              if (found_digit)
                tighter_lim = p - ! (key->human_numeric && unit_order[ch]);
            }
          else
            tighter_lim = lim;

          *lim = saved;
          lim = tighter_lim;
        }
    }

  size_t offset = debug_width (text, beg);
  size_t width = debug_width (beg, lim);
  mark_key (offset, width);
}

