/* Handle match, substr, index, and length keywords, and quoting "+".  */

static VALUE *
eval6 (bool evaluate)
{
  VALUE *l;
  VALUE *r;
  VALUE *v;
  VALUE *i1;
  VALUE *i2;

  trace ("eval6");
  if (nextarg ("+"))
    {
      if (nomoreargs ())
        syntax_error ();
      return str_value (*args++);
    }
  else if (nextarg ("length"))
    {
      r = eval6 (evaluate);
      tostring (r);
      v = int_value (strlen (r->u.s));
      freev (r);
      return v;
    }
  else if (nextarg ("match"))
    {
      l = eval6 (evaluate);
      r = eval6 (evaluate);
      if (evaluate)
        {
          v = docolon (l, r);
          freev (l);
        }
      else
        v = l;
      freev (r);
      return v;
    }
  else if (nextarg ("index"))
    {
      size_t pos;

      l = eval6 (evaluate);
      r = eval6 (evaluate);
      tostring (l);
      tostring (r);
      pos = strcspn (l->u.s, r->u.s);
      v = int_value (l->u.s[pos] ? pos + 1 : 0);
      freev (l);
      freev (r);
      return v;
    }
  else if (nextarg ("substr"))
    {
      size_t llen;
      l = eval6 (evaluate);
      i1 = eval6 (evaluate);
      i2 = eval6 (evaluate);
      tostring (l);
      llen = strlen (l->u.s);

      if (!toarith (i1) || !toarith (i2))
        v = str_value ("");
      else
        {
          size_t pos = getsize (i1->u.i);
          size_t len = getsize (i2->u.i);

          if (llen < pos || pos == 0 || len == 0 || len == SIZE_MAX)
            v = str_value ("");
          else
            {
              size_t vlen = MIN (len, llen - pos + 1);
              char *vlim;
              v = xmalloc (sizeof *v);
              v->type = string;
              v->u.s = xmalloc (vlen + 1);
              vlim = mempcpy (v->u.s, l->u.s + pos - 1, vlen);
              *vlim = '\0';
            }
        }
      freev (l);
      freev (i1);
      freev (i2);
      return v;
    }
  else
    return eval7 (evaluate);
}

