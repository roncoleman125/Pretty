static char const *
pretty_name (struct File_spec const *f)
{
  return (STREQ (f->name, "-") ? _("standard input") : f->name);
}

