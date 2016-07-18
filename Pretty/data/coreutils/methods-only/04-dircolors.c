/* Read the file open on FP (with name FILENAME).  First, look for a
   `TERM name' directive where name matches the current terminal type.
   Once found, translate and accumulate the associated directives onto
   the global obstack LSC_OBSTACK.  Give a diagnostic
   upon failure (unrecognized keyword is the only way to fail here).
   Return true if successful.  */

static bool
dc_parse_stream (FILE *fp, const char *filename)
{
  size_t line_number = 0;
  char const *next_G_line = G_line;
  char *input_line = NULL;
  size_t input_line_size = 0;
  char const *line;
  char const *term;
  bool ok = true;

  /* State for the parser.  */
  enum { ST_TERMNO, ST_TERMYES, ST_TERMSURE, ST_GLOBAL } state = ST_GLOBAL;

  /* Get terminal type */
  term = getenv ("TERM");
  if (term == NULL || *term == '\0')
    term = "none";

  while (1)
    {
      char *keywd, *arg;
      bool unrecognized;

      ++line_number;

      if (fp)
        {
          if (getline (&input_line, &input_line_size, fp) <= 0)
            {
              free (input_line);
              break;
            }
          line = input_line;
        }
      else
        {
          if (next_G_line == G_line + sizeof G_line)
            break;
          line = next_G_line;
          next_G_line += strlen (next_G_line) + 1;
        }

      parse_line (line, &keywd, &arg);

      if (keywd == NULL)
        continue;

      if (arg == NULL)
        {
          error (0, 0, _("%s:%lu: invalid line;  missing second token"),
                 filename, (unsigned long int) line_number);
          ok = false;
          free (keywd);
          continue;
        }

      unrecognized = false;
      if (c_strcasecmp (keywd, "TERM") == 0)
        {
          if (STREQ (arg, term))
            state = ST_TERMSURE;
          else if (state != ST_TERMSURE)
            state = ST_TERMNO;
        }
      else
        {
          if (state == ST_TERMSURE)
            state = ST_TERMYES; /* Another TERM can cancel */

          if (state != ST_TERMNO)
            {
              if (keywd[0] == '.')
                {
                  APPEND_CHAR ('*');
                  append_quoted (keywd);
                  APPEND_CHAR ('=');
                  append_quoted (arg);
                  APPEND_CHAR (':');
                }
              else if (keywd[0] == '*')
                {
                  append_quoted (keywd);
                  APPEND_CHAR ('=');
                  append_quoted (arg);
                  APPEND_CHAR (':');
                }
              else if (c_strcasecmp (keywd, "OPTIONS") == 0
                       || c_strcasecmp (keywd, "COLOR") == 0
                       || c_strcasecmp (keywd, "EIGHTBIT") == 0)
                {
                  /* Ignore.  */
                }
              else
                {
                  int i;

                  for (i = 0; slack_codes[i] != NULL; ++i)
                    if (c_strcasecmp (keywd, slack_codes[i]) == 0)
                      break;

                  if (slack_codes[i] != NULL)
                    {
                      APPEND_TWO_CHAR_STRING (ls_codes[i]);
                      APPEND_CHAR ('=');
                      append_quoted (arg);
                      APPEND_CHAR (':');
                    }
                  else
                    {
                      unrecognized = true;
                    }
                }
            }
          else
            {
              unrecognized = true;
            }
        }

      if (unrecognized && (state == ST_TERMSURE || state == ST_TERMYES))
        {
          error (0, 0, _("%s:%lu: unrecognized keyword %s"),
                 (filename ? quote (filename) : _("<internal>")),
                 (unsigned long int) line_number, keywd);
          ok = false;
        }

      free (keywd);
      free (arg);
    }

  return ok;
}

