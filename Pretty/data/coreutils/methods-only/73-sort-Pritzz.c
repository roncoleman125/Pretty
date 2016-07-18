/* Output LINE to TFP, unless -u is specified and the line compares
   equal to the previous line.  TEMP_OUTPUT is the name of TFP, or
   is null if TFP is standard output.

   This function does not save the line for comparison later, so it is
   appropriate only for internal sort.  */

static void
write_unique (struct line const *line, FILE *tfp, char const *temp_output)
{
  static struct line saved;

  if (unique)
    {
      if (saved.text && ! compare (line, &saved))
        return;
      saved = *line;
    }

  write_line (line, tfp, temp_output);
}

