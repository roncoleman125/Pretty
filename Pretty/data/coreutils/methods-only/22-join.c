/* Set the join field *VAR to VAL, but report an error if *VAR is set
   more than once to incompatible values.  */

static void
set_join_field (size_t *var, size_t val)
{
  if (*var != SIZE_MAX && *var != val)
    {
      unsigned long int var1 = *var + 1;
      unsigned long int val1 = val + 1;
      error (EXIT_FAILURE, 0, _("incompatible join fields %lu, %lu"),
             var1, val1);
    }
  *var = val;
}

/* Status of command-line arguments.  */

enum operand_status
  {
    /* This argument must be an operand, i.e., one of the files to be
       joined.  */
    MUST_BE_OPERAND,

    /* This might be the argument of the preceding -j1 or -j2 option,
       or it might be an operand.  */
    MIGHT_BE_J1_ARG,
    MIGHT_BE_J2_ARG,

    /* This might be the argument of the preceding -o option, or it might be
       an operand.  */
    MIGHT_BE_O_ARG
  };

