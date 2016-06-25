/* Handle |.  */

static VALUE *
eval (bool evaluate)
{
  VALUE *l;
  VALUE *r;

  trace ("eval");
  l = eval1 (evaluate);
  while (1)
    {
      if (nextarg ("|"))
        {
          r = eval1 (evaluate && null (l));
          if (null (l))
            {
              freev (l);
              l = r;
              if (null (l))
                {
                  freev (l);
                  l = int_value (0);
                }
            }
          else
            freev (r);
        }
      else
        return l;
    }
}
