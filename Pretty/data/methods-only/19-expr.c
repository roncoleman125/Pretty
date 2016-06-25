/* Handle : operator (pattern matching).
   Calls docolon to do the real work.  */

static VALUE *
eval5 (bool evaluate)
{
  VALUE *l;
  VALUE *r;
  VALUE *v;

  trace ("eval5");
  l = eval6 (evaluate);
  while (1)
    {
      if (nextarg (":"))
        {
          r = eval6 (evaluate);
          if (evaluate)
            {
              v = docolon (l, r);
              freev (l);
              l = v;
            }
          freev (r);
        }
      else
        return l;
    }
}

