/* Add NAME to the array of input file NAMES with operand statuses
   OPERAND_STATUS; currently there are NFILES names in the list.  */

static void
add_file_name (char *name, char *names[2],
               int operand_status[2], int joption_count[2], int *nfiles,
               int *prev_optc_status, int *optc_status)
{
  int n = *nfiles;

  if (n == 2)
    {
      bool op0 = (operand_status[0] == MUST_BE_OPERAND);
      char *arg = names[op0];
      switch (operand_status[op0])
        {
        case MUST_BE_OPERAND:
          error (0, 0, _("extra operand %s"), quote (name));
          usage (EXIT_FAILURE);

        case MIGHT_BE_J1_ARG:
          joption_count[0]--;
          set_join_field (&join_field_1, string_to_join_field (arg));
          break;

        case MIGHT_BE_J2_ARG:
          joption_count[1]--;
          set_join_field (&join_field_2, string_to_join_field (arg));
          break;

        case MIGHT_BE_O_ARG:
          add_field_list (arg);
          break;
        }
      if (!op0)
        {
          operand_status[0] = operand_status[1];
          names[0] = names[1];
        }
      n = 1;
    }

  operand_status[n] = *prev_optc_status;
  names[n] = name;
  *nfiles = n + 1;
  if (*prev_optc_status == MIGHT_BE_O_ARG)
    *optc_status = MIGHT_BE_O_ARG;
}

