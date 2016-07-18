/* Coerce V to an integer value.  Return true on success, false on failure.  */

static bool
toarith (VALUE *v)
{
  switch (v->type)
    {
    case integer:
      return true;
    case string:
      {
        char *s = v->u.s;

        if (! looks_like_integer (s))
          return false;
        if (mpz_init_set_str (v->u.i, s, 10) != 0 && !HAVE_GMP)
          error (EXPR_FAILURE, ERANGE, "%s", s);
        free (s);
        v->type = integer;
        return true;
      }
    default:
      abort ();
    }
}

