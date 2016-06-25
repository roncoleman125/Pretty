/* Return a VALUE for S.  */

static VALUE *
str_value (char const *s)
{
  VALUE *v = xmalloc (sizeof *v);
  v->type = string;
  v->u.s = xstrdup (s);
  return v;
}

