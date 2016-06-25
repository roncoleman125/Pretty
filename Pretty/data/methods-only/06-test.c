/* Return true if S is one of the test command's binary operators.  */
static bool
binop (char const *s)
{
  return ((STREQ (s,   "=")) || (STREQ (s,  "!=")) || (STREQ (s, "-nt")) ||
          (STREQ (s, "-ot")) || (STREQ (s, "-ef")) || (STREQ (s, "-eq")) ||
          (STREQ (s, "-ne")) || (STREQ (s, "-lt")) || (STREQ (s, "-le")) ||
          (STREQ (s, "-gt")) || (STREQ (s, "-ge")));
}

