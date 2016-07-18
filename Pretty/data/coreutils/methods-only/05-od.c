/* If S points to a single valid modern od format string, put
   a description of that format in *TSPEC, make *NEXT point at the
   character following the just-decoded format (if *NEXT is non-NULL),
   and return true.  If S is not valid, don't modify *NEXT or *TSPEC,
   give a diagnostic, and return false.  For example, if S were
   "d4afL" *NEXT would be set to "afL" and *TSPEC would be
     {
       fmt = SIGNED_DECIMAL;
       size = INT or LONG; (whichever integral_type_size[4] resolves to)
       print_function = print_int; (assuming size == INT)
       field_width = 11;
       fmt_string = "%*d";
      }
   pad_width is determined later, but is at least as large as the
   number of fields printed per row.
   S_ORIG is solely for reporting errors.  It should be the full format
   string argument.
   */

static bool
decode_one_format (const char *s_orig, const char *s, const char **next,
                   struct tspec *tspec)
{
  enum size_spec size_spec;
  unsigned long int size;
  enum output_format fmt;
  void (*print_function) (size_t, size_t, void const *, char const *,
                          int, int);
  const char *p;
  char c;
  int field_width;

  assert (tspec != NULL);

  switch (*s)
    {
    case 'd':
    case 'o':
    case 'u':
    case 'x':
      c = *s;
      ++s;
      switch (*s)
        {
        case 'C':
          ++s;
          size = sizeof (char);
          break;

        case 'S':
          ++s;
          size = sizeof (short int);
          break;

        case 'I':
          ++s;
          size = sizeof (int);
          break;

        case 'L':
          ++s;
          size = sizeof (long int);
          break;

        default:
          if (! simple_strtoul (s, &p, &size))
            {
              /* The integer at P in S would overflow an unsigned long int.
                 A digit string that long is sufficiently odd looking
                 that the following diagnostic is sufficient.  */
              error (0, 0, _("invalid type string %s"), quote (s_orig));
              return false;
            }
          if (p == s)
            size = sizeof (int);
          else
            {
              if (MAX_INTEGRAL_TYPE_SIZE < size
                  || integral_type_size[size] == NO_SIZE)
                {
                  error (0, 0, _("invalid type string %s;\n\
this system doesn't provide a %lu-byte integral type"), quote (s_orig), size);
                  return false;
                }
              s = p;
            }
          break;
        }

  ((Spec) == LONG_LONG ? (Max_format)					\
   : ((Spec) == LONG ? (Long_format)					\
      : (Min_format)))							\

      size_spec = integral_type_size[size];

      switch (c)
        {
        case 'd':
          fmt = SIGNED_DECIMAL;
          field_width = bytes_to_signed_dec_digits[size];
          sprintf (tspec->fmt_string, "%%*%s",
                   ISPEC_TO_FORMAT (size_spec, "d", "ld", PRIdMAX));
          break;

        case 'o':
          fmt = OCTAL;
          sprintf (tspec->fmt_string, "%%*.%d%s",
                   (field_width = bytes_to_oct_digits[size]),
                   ISPEC_TO_FORMAT (size_spec, "o", "lo", PRIoMAX));
          break;

        case 'u':
          fmt = UNSIGNED_DECIMAL;
          field_width = bytes_to_unsigned_dec_digits[size];
          sprintf (tspec->fmt_string, "%%*%s",
                   ISPEC_TO_FORMAT (size_spec, "u", "lu", PRIuMAX));
          break;

        case 'x':
          fmt = HEXADECIMAL;
          sprintf (tspec->fmt_string, "%%*.%d%s",
                   (field_width = bytes_to_hex_digits[size]),
                   ISPEC_TO_FORMAT (size_spec, "x", "lx", PRIxMAX));
          break;

        default:
          abort ();
        }

      assert (strlen (tspec->fmt_string) < FMT_BYTES_ALLOCATED);

      switch (size_spec)
        {
        case CHAR:
          print_function = (fmt == SIGNED_DECIMAL
                            ? print_s_char
                            : print_char);
          break;

        case SHORT:
          print_function = (fmt == SIGNED_DECIMAL
                            ? print_s_short
                            : print_short);
          break;

        case INT:
          print_function = print_int;
          break;

        case LONG:
          print_function = print_long;
          break;

        case LONG_LONG:
          print_function = print_long_long;
          break;

        default:
          abort ();
        }
      break;

    case 'f':
      fmt = FLOATING_POINT;
      ++s;
      switch (*s)
        {
        case 'F':
          ++s;
          size = sizeof (float);
          break;

        case 'D':
          ++s;
          size = sizeof (double);
          break;

        case 'L':
          ++s;
          size = sizeof (long double);
          break;

        default:
          if (! simple_strtoul (s, &p, &size))
            {
              /* The integer at P in S would overflow an unsigned long int.
                 A digit string that long is sufficiently odd looking
                 that the following diagnostic is sufficient.  */
              error (0, 0, _("invalid type string %s"), quote (s_orig));
              return false;
            }
          if (p == s)
            size = sizeof (double);
          else
            {
              if (size > MAX_FP_TYPE_SIZE
                  || fp_type_size[size] == NO_SIZE)
                {
                  error (0, 0, _("invalid type string %s;\n\
this system doesn't provide a %lu-byte floating point type"),
                         quote (s_orig), size);
                  return false;
                }
              s = p;
            }
          break;
        }
      size_spec = fp_type_size[size];

      struct lconv const *locale = localeconv ();
      size_t decimal_point_len =
        (locale->decimal_point[0] ? strlen (locale->decimal_point) : 1);

      switch (size_spec)
        {
        case FLOAT_SINGLE:
          print_function = print_float;
          field_width = FLT_STRLEN_BOUND_L (decimal_point_len);
          break;

        case FLOAT_DOUBLE:
          print_function = print_double;
          field_width = DBL_STRLEN_BOUND_L (decimal_point_len);
          break;

        case FLOAT_LONG_DOUBLE:
          print_function = print_long_double;
          field_width = LDBL_STRLEN_BOUND_L (decimal_point_len);
          break;

        default:
          abort ();
        }

      break;

    case 'a':
      ++s;
      fmt = NAMED_CHARACTER;
      size_spec = CHAR;
      print_function = print_named_ascii;
      field_width = 3;
      break;

    case 'c':
      ++s;
      fmt = CHARACTER;
      size_spec = CHAR;
      print_function = print_ascii;
      field_width = 3;
      break;

    default:
      error (0, 0, _("invalid character `%c' in type string %s"),
             *s, quote (s_orig));
      return false;
    }

  tspec->size = size_spec;
  tspec->fmt = fmt;
  tspec->print_function = print_function;

  tspec->field_width = field_width;
  tspec->hexl_mode_trailer = (*s == 'z');
  if (tspec->hexl_mode_trailer)
    s++;

  if (next != NULL)
    *next = s;

  return true;
}

