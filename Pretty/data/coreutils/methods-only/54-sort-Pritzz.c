/* Compare two lines A and B trying every key in sequence until there
   are no more keys or a difference is found. */

static int
keycompare (struct line const *a, struct line const *b)
{
  struct keyfield *key = keylist;

  /* For the first iteration only, the key positions have been
     precomputed for us. */
  char *texta = a->keybeg;
  char *textb = b->keybeg;
  char *lima = a->keylim;
  char *limb = b->keylim;

  int diff;

  while (true)
    {
      char const *translate = key->translate;
      bool const *ignore = key->ignore;

      /* Treat field ends before field starts as empty fields.  */
      lima = MAX (texta, lima);
      limb = MAX (textb, limb);

      /* Find the lengths. */
      size_t lena = lima - texta;
      size_t lenb = limb - textb;

      if (hard_LC_COLLATE || key_numeric (key)
          || key->month || key->random || key->version)
        {
          char *ta;
          char *tb;
          size_t tlena;
          size_t tlenb;

          char enda IF_LINT (= 0);
          char endb IF_LINT (= 0);
          void *allocated IF_LINT (= NULL);
          char stackbuf[4000];

          if (ignore || translate)
            {
              /* Compute with copies of the keys, which are the result of
                 translating or ignoring characters, and which need their
                 own storage.  */

              size_t i;

              /* Allocate space for copies.  */
              size_t size = lena + 1 + lenb + 1;
              if (size <= sizeof stackbuf)
                ta = stackbuf, allocated = NULL;
              else
                ta = allocated = xmalloc (size);
              tb = ta + lena + 1;

              /* Put into each copy a version of the key in which the
                 requested characters are ignored or translated.  */
              for (tlena = i = 0; i < lena; i++)
                if (! (ignore && ignore[to_uchar (texta[i])]))
                  ta[tlena++] = (translate
                                 ? translate[to_uchar (texta[i])]
                                 : texta[i]);
              ta[tlena] = '\0';

              for (tlenb = i = 0; i < lenb; i++)
                if (! (ignore && ignore[to_uchar (textb[i])]))
                  tb[tlenb++] = (translate
                                 ? translate[to_uchar (textb[i])]
                                 : textb[i]);
              tb[tlenb] = '\0';
            }
          else
            {
              /* Use the keys in-place, temporarily null-terminated.  */
              ta = texta; tlena = lena; enda = ta[tlena]; ta[tlena] = '\0';
              tb = textb; tlenb = lenb; endb = tb[tlenb]; tb[tlenb] = '\0';
            }

          if (key->numeric)
            diff = numcompare (ta, tb);
          else if (key->general_numeric)
            diff = general_numcompare (ta, tb);
          else if (key->human_numeric)
            diff = human_numcompare (ta, tb);
          else if (key->month)
            diff = getmonth (ta, NULL) - getmonth (tb, NULL);
          else if (key->random)
            diff = compare_random (ta, tlena, tb, tlenb);
          else if (key->version)
            diff = filevercmp (ta, tb);
          else
            {
              /* Locale-dependent string sorting.  This is slower than
                 C-locale sorting, which is implemented below.  */
              if (tlena == 0)
                diff = - NONZERO (tlenb);
              else if (tlenb == 0)
                diff = 1;
              else
                diff = xmemcoll0 (ta, tlena + 1, tb, tlenb + 1);
            }

          if (ignore || translate)
            free (allocated);
          else
            {
              ta[tlena] = enda;
              tb[tlenb] = endb;
            }
        }
      else if (ignore)
        {
  do									\
    {									\
          while (true)							\
            {								\
              while (texta < lima && ignore[to_uchar (*texta)])		\
                ++texta;						\
              while (textb < limb && ignore[to_uchar (*textb)])		\
                ++textb;						\
              if (! (texta < lima && textb < limb))			\
                break;							\
              diff = to_uchar (A) - to_uchar (B);			\
              if (diff)							\
                goto not_equal;						\
              ++texta;							\
              ++textb;							\
            }								\
                                                                        \
          diff = (texta < lima) - (textb < limb);			\
    }									\
  while (0)

          if (translate)
            CMP_WITH_IGNORE (translate[to_uchar (*texta)],
                             translate[to_uchar (*textb)]);
          else
            CMP_WITH_IGNORE (*texta, *textb);
        }
      else if (lena == 0)
        diff = - NONZERO (lenb);
      else if (lenb == 0)
        goto greater;
      else
        {
          if (translate)
            {
              while (texta < lima && textb < limb)
                {
                  diff = (to_uchar (translate[to_uchar (*texta++)])
                          - to_uchar (translate[to_uchar (*textb++)]));
                  if (diff)
                    goto not_equal;
                }
            }
          else
            {
              diff = memcmp (texta, textb, MIN (lena, lenb));
              if (diff)
                goto not_equal;
            }
          diff = lena < lenb ? -1 : lena != lenb;
        }

      if (diff)
        goto not_equal;

      key = key->next;
      if (! key)
        break;

      /* Find the beginning and limit of the next field.  */
      if (key->eword != SIZE_MAX)
        lima = limfield (a, key), limb = limfield (b, key);
      else
        lima = a->text + a->length - 1, limb = b->text + b->length - 1;

      if (key->sword != SIZE_MAX)
        texta = begfield (a, key), textb = begfield (b, key);
      else
        {
          texta = a->text, textb = b->text;
          if (key->skipsblanks)
            {
              while (texta < lima && blanks[to_uchar (*texta)])
                ++texta;
              while (textb < limb && blanks[to_uchar (*textb)])
                ++textb;
            }
        }
    }

  return 0;

 greater:
  diff = 1;
 not_equal:
  return key->reverse ? -diff : diff;
}

