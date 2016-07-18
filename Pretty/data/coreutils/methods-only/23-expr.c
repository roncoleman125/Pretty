/* Handle &.  */

static VALUE *
eval1 (bool evaluate)
{
  VALUE *l;
  VALUE *r;

  trace ("eval1");
  l = eval2 (evaluate);
  while (1)
    {
      if (nextarg ("&"))
        {
          r = eval2 (evaluate && !null (l));
          if (null (l) || null (r))
            {
              freev (l);
              freev (r);
              l = int_value (0);
            }
          else
            freev (r);
        }
      else
        return l;
    }
}

