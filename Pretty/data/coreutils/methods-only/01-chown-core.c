extern void
chopt_free (struct Chown_option *chopt ATTRIBUTE_UNUSED)
{
  /* Deliberately do not free chopt->user_name or ->group_name.
     They're not always allocated.  */
}

