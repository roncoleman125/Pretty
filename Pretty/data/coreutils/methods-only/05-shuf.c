static int
write_permuted_output (size_t n_lines, char * const *line, size_t lo_input,
                       size_t const *permutation, char eolbyte)
{
  size_t i;

  if (line)
    for (i = 0; i < n_lines; i++)
      {
        char * const *p = line + permutation[i];
        size_t len = p[1] - p[0];
        if (fwrite (p[0], sizeof *p[0], len, stdout) != len)
          return -1;
      }
  else
    for (i = 0; i < n_lines; i++)
      {
        unsigned long int n = lo_input + permutation[i];
        if (printf ("%lu%c", n, eolbyte) < 0)
          return -1;
      }

  return 0;
}

