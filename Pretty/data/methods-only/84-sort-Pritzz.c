static void
badfieldspec (char const *spec, char const *msgid)
{
  error (SORT_FAILURE, 0, _("%s: invalid field specification %s"),
         _(msgid), quote (spec));
  abort ();
}

/* Report incompatible options.  */
