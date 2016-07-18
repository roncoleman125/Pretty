static void
input_from_argv (char **operand, int n_operands, char eolbyte)
{
  char *p;
  size_t size = n_operands;
  int i;

  for (i = 0; i < n_operands; i++)
    size += strlen (operand[i]);
  p = xmalloc (size);

  for (i = 0; i < n_operands; i++)
    {
      char *p1 = stpcpy (p, operand[i]);
      operand[i] = p;
      p = p1;
      *p++ = eolbyte;
    }

  operand[n_operands] = p;
}

