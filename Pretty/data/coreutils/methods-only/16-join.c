/* Print the join of LINE1 and LINE2.  */

static void
prjoin (struct line const *line1, struct line const *line2)
{
  const struct outlist *outlist;
  char output_separator = tab < 0 ? ' ' : tab;
  size_t field;
  struct line const *line;

  outlist = outlist_head.next;
  if (outlist)
    {
      const struct outlist *o;

      o = outlist;
      while (1)
        {
          if (o->file == 0)
            {
              if (line1 == &uni_blank)
                {
                  line = line2;
                  field = join_field_2;
                }
              else
                {
                  line = line1;
                  field = join_field_1;
                }
            }
          else
            {
              line = (o->file == 1 ? line1 : line2);
              field = o->field;
            }
          prfield (field, line);
          o = o->next;
          if (o == NULL)
            break;
          putchar (output_separator);
        }
      putchar ('\n');
    }
  else
    {
      if (line1 == &uni_blank)
        {
          line = line2;
          field = join_field_2;
        }
      else
        {
          line = line1;
          field = join_field_1;
        }

      /* Output the join field.  */
      prfield (field, line);

      /* Output other fields.  */
      prfields (line1, join_field_1, autocount_1);
      prfields (line2, join_field_2, autocount_2);

      putchar ('\n');
    }
}

