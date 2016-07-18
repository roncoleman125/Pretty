/* Handle *, /, % operators.  */

static VALUE *
eval4 (bool evaluate)
{
  VALUE *l;
  VALUE *r;
  enum { multiply, divide, mod } fxn;

  trace ("eval4");
  l = eval5 (evaluate);
  while (1)
    {
      if (nextarg ("*"))
        fxn = multiply;
      else if (nextarg ("/"))
        fxn = divide;
      else if (nextarg ("%"))
        fxn = mod;
      else
        return l;
      r = eval5 (evaluate);
      if (evaluate)
        {
          if (!toarith (l) || !toarith (r))
            error (EXPR_INVALID, 0, _("non-integer argument"));
          if (fxn != multiply && mpz_sgn (r->u.i) == 0)
            error (EXPR_INVALID, 0, _("division by zero"));
          ((fxn == multiply ? mpz_mul
            : fxn == divide ? mpz_tdiv_q
            : mpz_tdiv_r)
           (l->u.i, l->u.i, r->u.i));
        }
      freev (r);
    }
}

