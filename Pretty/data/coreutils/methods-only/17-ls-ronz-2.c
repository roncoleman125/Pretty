static void
parse_ls_color (void)
{
  const char *p;		/* Pointer to character being parsed */
  char *buf;			/* color_buf buffer pointer */
  int state;			/* State of parser */
  int ind_no;			/* Indicator number */
  char label[3];		/* Indicator label */
  struct color_ext_type *ext;	/* Extension we are working on */

  if ((p = getenv ("LS_COLORS")) == NULL || *p == '\0')
    return;

  ext = NULL;
  strcpy (label, "??");

  /* This is an overly conservative estimate, but any possible
     LS_COLORS string will *not* generate a color_buf longer than
     itself, so it is a safe way of allocating a buffer in
     advance.  */
  buf = color_buf = xstrdup (p);

  state = 1;
  while (state > 0)
    {
      switch (state)
        {
        case 1:		/* First label character */
          switch (*p)
            {
            case ':':
              ++p;
              break;

            case '*':
              /* Allocate new extension block and add to head of
                 linked list (this way a later definition will
                 override an earlier one, which can be useful for
                 having terminal-specific defs override global).  */

              ext = xmalloc (sizeof *ext);
              ext->next = color_ext_list;
              color_ext_list = ext;

              ++p;
              ext->ext.string = buf;

              state = (get_funky_string (&buf, &p, true, &ext->ext.len)
                       ? 4 : -1);
              break;

            case '\0':
              state = 0;	/* Done! */
              break;

            default:	/* Assume it is file type label */
              label[0] = *(p++);
              state = 2;
              break;
            }
          break;

        case 2:		/* Second label character */
          if (*p)
            {
              label[1] = *(p++);
              state = 3;
            }
          else
            state = -1;	/* Error */
          break;

        case 3:		/* Equal sign after indicator label */
          state = -1;	/* Assume failure...  */
          if (*(p++) == '=')/* It *should* be...  */
            {
              for (ind_no = 0; indicator_name[ind_no] != NULL; ++ind_no)
                {
                  if (STREQ (label, indicator_name[ind_no]))
                    {
                      color_indicator[ind_no].string = buf;
                      state = (get_funky_string (&buf, &p, false,
                                                 &color_indicator[ind_no].len)
                               ? 1 : -1);
                      break;
                    }
                }
              if (state == -1)
                error (0, 0, _("unrecognized prefix: %s"), quotearg (label));
            }
          break;

        case 4:		/* Equal sign after *.ext */
          if (*(p++) == '=')
            {
              ext->seq.string = buf;
              state = (get_funky_string (&buf, &p, false, &ext->seq.len)
                       ? 1 : -1);
            }
          else
            state = -1;
          break;
        }
    }

  if (state < 0)
    {
      struct color_ext_type *e;
      struct color_ext_type *e2;

      error (0, 0,
             _("unparsable value for LS_COLORS environment variable"));
      free (color_buf);
      for (e = color_ext_list; e != NULL; /* empty */)
        {
          e2 = e;
          e = e->next;
          free (e2);
        }
      print_with_color = false;
    }

  if (color_indicator[C_LINK].len == 6
      && !strncmp (color_indicator[C_LINK].string, "target", 6))
    color_symlink_as_referent = true;
}

