/* Handle bare operands and ( expr ) syntax.  */

static VALUE *
eval7 (bool evaluate)
{
  VALUE *v;

  trace ("eval7");
  if (nomoreargs ())
    syntax_error ();

  if (nextarg ("("))
    {
      v = eval (evaluate);
      if (!nextarg (")"))
        syntax_error ();
      return v;
    }

  if (nextarg (")"))
    syntax_error ();

  return str_value (*args++);
}

