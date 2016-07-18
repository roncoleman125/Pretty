/* Handle +, - operators.  */

static VALUE *
eval3 (bool evaluate)
{
  VALUE *l;
  VALUE *r;
  enum { plus, minus } fxn;

  trace ("eval3");
  l = eval4 (evaluate);
  while (1)
    {
      if (nextarg ("+"))
        fxn = plus;
      else if (nextarg ("-"))
        fxn = minus;
      else
        return l;
      r = eval4 (evaluate);
      if (evaluate)
        {
          if (!toarith (l) || !toarith (r))
            error (EXPR_INVALID, 0, _("non-integer argument"));
          (fxn == plus ? mpz_add : mpz_sub) (l->u.i, l->u.i, r->u.i);
        }
      freev (r);
    }
}

