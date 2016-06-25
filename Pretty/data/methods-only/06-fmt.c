/* Copy to the output a line that failed to match the prefix, or that
   was blank after the prefix.  In the former case, C is the character
   that failed to match the prefix.  In the latter, C is \n or EOF.
   Return the character (\n or EOF) ending the line.  */

static int
copy_rest (FILE *f, int c)
{
  const char *s;

  out_column = 0;
  if (in_column > next_prefix_indent || (c != '\n' && c != EOF))
    {
      put_space (next_prefix_indent);
      for (s = prefix; out_column != in_column && *s; out_column++)
        putchar (*s++);
      if (c != EOF && c != '\n')
        put_space (in_column - out_column);
      if (c == EOF && in_column >= next_prefix_indent + prefix_length)
        putchar ('\n');
    }
  while (c != '\n' && c != EOF)
    {
      putchar (c);
      c = getc (f);
    }
  return c;
}

