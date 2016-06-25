/* Coerce V to a string value (can't fail).  */

static void
tostring (VALUE *v)
{
  switch (v->type)
    {
    case integer:
      {
        char *s = mpz_get_str (NULL, 10, v->u.i);
        mpz_clear (v->u.i);
        v->u.s = s;
        v->type = string;
      }
      break;
    case string:
      break;
    default:
      abort ();
    }
}

