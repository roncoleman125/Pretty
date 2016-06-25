/* Output data independent key warnings to stderr.  */

static void
key_warnings (struct keyfield const *gkey, bool gkey_only)
{
  struct keyfield const *key;
  struct keyfield ugkey = *gkey;
  unsigned long keynum = 1;

  for (key = keylist; key; key = key->next, keynum++)
    {
      if (key->obsolete_used)
        {
          size_t sword = key->sword;
          size_t eword = key->eword;
          char tmp[INT_BUFSIZE_BOUND (uintmax_t)];
          /* obsolescent syntax +A.x -B.y is equivalent to:
               -k A+1.x+1,B.y   (when y = 0)
               -k A+1.x+1,B+1.y (when y > 0)  */
          char obuf[INT_BUFSIZE_BOUND (sword) * 2 + 4]; /* +# -#  */
          char nbuf[INT_BUFSIZE_BOUND (sword) * 2 + 5]; /* -k #,#  */
          char *po = obuf;
          char *pn = nbuf;

          if (sword == SIZE_MAX)
            sword++;

          po = stpcpy (stpcpy (po, "+"), umaxtostr (sword, tmp));
          pn = stpcpy (stpcpy (pn, "-k "), umaxtostr (sword + 1, tmp));
          if (key->eword != SIZE_MAX)
            {
              stpcpy (stpcpy (po, " -"), umaxtostr (eword + 1, tmp));
              stpcpy (stpcpy (pn, ","),
                      umaxtostr (eword + 1
                                 + (key->echar == SIZE_MAX), tmp));
            }
          error (0, 0, _("obsolescent key `%s' used; consider `%s' instead"),
                 obuf, nbuf);
        }

      /* Warn about field specs that will never match.  */
      if (key->sword != SIZE_MAX && key->eword < key->sword)
        error (0, 0, _("key %lu has zero width and will be ignored"), keynum);

      /* Warn about significant leading blanks.  */
      bool implicit_skip = key_numeric (key) || key->month;
      bool maybe_space_aligned = !hard_LC_COLLATE && default_key_compare (key)
                                 && !(key->schar || key->echar);
      bool line_offset = key->eword == 0 && key->echar != 0; /* -k1.x,1.y  */
      if (!gkey_only && tab == TAB_DEFAULT && !line_offset
          && ((!key->skipsblanks && !(implicit_skip || maybe_space_aligned))
              || (!key->skipsblanks && key->schar)
              || (!key->skipeblanks && key->echar)))
        error (0, 0, _("leading blanks are significant in key %lu; "
                       "consider also specifying `b'"), keynum);

      /* Warn about numeric comparisons spanning fields,
         as field delimiters could be interpreted as part
         of the number (maybe only in other locales).  */
      if (!gkey_only && key_numeric (key))
        {
          size_t sword = key->sword + 1;
          size_t eword = key->eword + 1;
          if (!sword)
            sword++;
          if (sword != eword)
            error (0, 0, _("key %lu is numeric and spans multiple fields"),
                   keynum);
        }

      /* Flag global options not copied or specified in any key.  */
      if (ugkey.ignore && (ugkey.ignore == key->ignore))
        ugkey.ignore = NULL;
      if (ugkey.translate && (ugkey.translate == key->translate))
        ugkey.translate = NULL;
      ugkey.skipsblanks &= !key->skipsblanks;
      ugkey.skipeblanks &= !key->skipeblanks;
      ugkey.month &= !key->month;
      ugkey.numeric &= !key->numeric;
      ugkey.general_numeric &= !key->general_numeric;
      ugkey.human_numeric &= !key->human_numeric;
      ugkey.random &= !key->random;
      ugkey.version &= !key->version;
      ugkey.reverse &= !key->reverse;
    }

  /* Warn about ignored global options flagged above.
     Note if gkey is the only one in the list, all flags are cleared.  */
  if (!default_key_compare (&ugkey)
      || (ugkey.reverse && (stable || unique) && keylist))
    {
      bool ugkey_reverse = ugkey.reverse;
      if (!(stable || unique))
        ugkey.reverse = false;
      /* The following is too big, but guaranteed to be "big enough".  */
      char opts[sizeof short_options];
      key_to_opts (&ugkey, opts);
      error (0, 0,
             ngettext ("option `-%s' is ignored",
                       "options `-%s' are ignored",
                       select_plural (strlen (opts))), opts);
      ugkey.reverse = ugkey_reverse;
    }
  if (ugkey.reverse && !(stable || unique) && keylist)
    error (0, 0, _("option `-r' only applies to last-resort comparison"));
}

