/* OPERAND is of the form "X=...".  Return true if X is NAME.  */

static bool
operand_is (char const *operand, char const *name)
{
  return operand_matches (operand, name, '=');
}

