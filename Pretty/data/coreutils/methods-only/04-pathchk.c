/* Return the address of the start of the next file name component in F.  */

static char *
component_start (char *f)
{
  while (*f == '/')
    f++;
  return f;
}

