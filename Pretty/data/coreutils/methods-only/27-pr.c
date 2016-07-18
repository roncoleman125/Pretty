/* Print (or store, depending on p->char_func) a clump of N
   characters. */

static void
print_clump (COLUMN *p, int n, char *clump)
{
  while (n--)
    (p->char_func) (*clump++);
}

