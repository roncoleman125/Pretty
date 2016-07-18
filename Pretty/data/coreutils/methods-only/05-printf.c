/* Evaluate a printf conversion specification.  START is the start of
   the directive, LENGTH is its length, and CONVERSION specifies the
   type of conversion.  LENGTH does not include any length modifier or
   the conversion specifier itself.  FIELD_WIDTH and PRECISION are the
   field width and precision for '*' values, if HAVE_FIELD_WIDTH and
   HAVE_PRECISION are true, respectively.  ARGUMENT is the argument to
   be formatted.  */

static void
print_direc (const char *start, size_t length, char conversion,
             bool have_field_width, int field_width,
             bool have_precision, int precision,
             char const *argument)
{
  char *p;		/* Null-terminated copy of % directive. */

  /* Create a null-terminated copy of the % directive, with an
     intmax_t-wide length modifier substituted for any existing
     integer length modifier.  */
  {
    char *q;
    char const *length_modifier;
    size_t length_modifier_len;

    switch (conversion)
      {
      case 'd': case 'i': case 'o': case 'u': case 'x': case 'X':
        length_modifier = PRIdMAX;
        length_modifier_len = sizeof PRIdMAX - 2;
        break;

      case 'a': case 'e': case 'f': case 'g':
      case 'A': case 'E': case 'F': case 'G':
        length_modifier = "L";
        length_modifier_len = 1;
        break;

      default:
        length_modifier = start;  /* Any valid pointer will do.  */
        length_modifier_len = 0;
        break;
      }

    p = xmalloc (length + length_modifier_len + 2);
    q = mempcpy (p, start, length);
    q = mempcpy (q, length_modifier, length_modifier_len);
    *q++ = conversion;
    *q = '\0';
  }

  switch (conversion)
    {
    case 'd':
    case 'i':
      {
        intmax_t arg = vstrtoimax (argument);
        if (!have_field_width)
          {
            if (!have_precision)
              xprintf (p, arg);
            else
              xprintf (p, precision, arg);
          }
        else
          {
            if (!have_precision)
              xprintf (p, field_width, arg);
            else
              xprintf (p, field_width, precision, arg);
          }
      }
      break;

    case 'o':
    case 'u':
    case 'x':
    case 'X':
      {
        uintmax_t arg = vstrtoumax (argument);
        if (!have_field_width)
          {
            if (!have_precision)
              xprintf (p, arg);
            else
              xprintf (p, precision, arg);
          }
        else
          {
            if (!have_precision)
              xprintf (p, field_width, arg);
            else
              xprintf (p, field_width, precision, arg);
          }
      }
      break;

    case 'a':
    case 'A':
    case 'e':
    case 'E':
    case 'f':
    case 'F':
    case 'g':
    case 'G':
      {
        long double arg = vstrtold (argument);
        if (!have_field_width)
          {
            if (!have_precision)
              xprintf (p, arg);
            else
              xprintf (p, precision, arg);
          }
        else
          {
            if (!have_precision)
              xprintf (p, field_width, arg);
            else
              xprintf (p, field_width, precision, arg);
          }
      }
      break;

    case 'c':
      if (!have_field_width)
        xprintf (p, *argument);
      else
        xprintf (p, field_width, *argument);
      break;

    case 's':
      if (!have_field_width)
        {
          if (!have_precision)
            xprintf (p, argument);
          else
            xprintf (p, precision, argument);
        }
      else
        {
          if (!have_precision)
            xprintf (p, field_width, argument);
          else
            xprintf (p, field_width, precision, argument);
        }
      break;
    }

  free (p);
}

