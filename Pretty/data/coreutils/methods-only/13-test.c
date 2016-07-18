/* Return true if OP is one of the test command's unary operators. */
static bool
test_unop (char const *op)
{
  if (op[0] != '-')
    return false;

  switch (op[1])
    {
    case 'a': case 'b': case 'c': case 'd': case 'e':
    case 'f': case 'g': case 'h': case 'k': case 'n':
    case 'o': case 'p': case 'r': case 's': case 't':
    case 'u': case 'w': case 'x': case 'z':
    case 'G': case 'L': case 'O': case 'S': case 'N':
      return true;
    default:
      return false;
    }
}

