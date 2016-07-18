/* Return true if V is a null-string or zero-number.  */

static bool
null (VALUE *v)
{
  switch (v->type)
    {
    case integer:
      return mpz_sgn (v->u.i) == 0;
    case string:
      {
        char const *cp = v->u.s;
        if (*cp == '\0')
          return true;

        cp += (*cp == '-');

        do
          {
            if (*cp != '0')
              return false;
          }
        while (*++cp);

        return true;
      }
    default:
      abort ();
    }
}

