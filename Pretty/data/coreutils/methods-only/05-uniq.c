/* Process input file INFILE with output to OUTFILE.
   If either is "-", use the standard I/O stream for it instead. */

static void
check_file (const char *infile, const char *outfile, char delimiter)
{
  struct linebuffer lb1, lb2;
  struct linebuffer *thisline, *prevline;

  if (! (STREQ (infile, "-") || freopen (infile, "r", stdin)))
    error (EXIT_FAILURE, errno, "%s", infile);
  if (! (STREQ (outfile, "-") || freopen (outfile, "w", stdout)))
    error (EXIT_FAILURE, errno, "%s", outfile);

  fadvise (stdin, FADVISE_SEQUENTIAL);

  thisline = &lb1;
  prevline = &lb2;

  initbuffer (thisline);
  initbuffer (prevline);

  /* The duplication in the following `if' and `else' blocks is an
     optimization to distinguish the common case (in which none of
     the following options has been specified: --count, -repeated,
     --all-repeated, --unique) from the others.  In the common case,
     this optimization lets uniq output each different line right away,
     without waiting to see if the next one is different.  */

  if (output_unique && output_first_repeated && countmode == count_none)
    {
      char *prevfield IF_LINT ( = NULL);
      size_t prevlen IF_LINT ( = 0);

      while (!feof (stdin))
        {
          char *thisfield;
          size_t thislen;
          if (readlinebuffer_delim (thisline, stdin, delimiter) == 0)
            break;
          thisfield = find_field (thisline);
          thislen = thisline->length - 1 - (thisfield - thisline->buffer);
          if (prevline->length == 0
              || different (thisfield, prevfield, thislen, prevlen))
            {
              fwrite (thisline->buffer, sizeof (char),
                      thisline->length, stdout);

              SWAP_LINES (prevline, thisline);
              prevfield = thisfield;
              prevlen = thislen;
            }
        }
    }
  else
    {
      char *prevfield;
      size_t prevlen;
      uintmax_t match_count = 0;
      bool first_delimiter = true;

      if (readlinebuffer_delim (prevline, stdin, delimiter) == 0)
        goto closefiles;
      prevfield = find_field (prevline);
      prevlen = prevline->length - 1 - (prevfield - prevline->buffer);

      while (!feof (stdin))
        {
          bool match;
          char *thisfield;
          size_t thislen;
          if (readlinebuffer_delim (thisline, stdin, delimiter) == 0)
            {
              if (ferror (stdin))
                goto closefiles;
              break;
            }
          thisfield = find_field (thisline);
          thislen = thisline->length - 1 - (thisfield - thisline->buffer);
          match = !different (thisfield, prevfield, thislen, prevlen);
          match_count += match;

          if (match_count == UINTMAX_MAX)
            {
              if (count_occurrences)
                error (EXIT_FAILURE, 0, _("too many repeated lines"));
              match_count--;
            }

          if (delimit_groups != DM_NONE)
            {
              if (!match)
                {
                  if (match_count) /* a previous match */
                    first_delimiter = false; /* Only used when DM_SEPARATE */
                }
              else if (match_count == 1)
                {
                  if ((delimit_groups == DM_PREPEND)
                      || (delimit_groups == DM_SEPARATE
                          && !first_delimiter))
                    putchar (delimiter);
                }
            }

          if (!match || output_later_repeated)
            {
              writeline (prevline, match, match_count);
              SWAP_LINES (prevline, thisline);
              prevfield = thisfield;
              prevlen = thislen;
              if (!match)
                match_count = 0;
            }
        }

      writeline (prevline, false, match_count);
    }

 closefiles:
  if (ferror (stdin) || fclose (stdin) != 0)
    error (EXIT_FAILURE, 0, _("error reading %s"), infile);

  /* stdout is handled via the atexit-invoked close_stdout function.  */

  free (lb1.buffer);
  free (lb2.buffer);
}

enum Skip_field_option_type
  {
    SFO_NONE,
    SFO_OBSOLETE,
    SFO_NEW
  };

