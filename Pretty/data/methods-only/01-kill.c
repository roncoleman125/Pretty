/* Print a row of `kill -t' output.  NUM_WIDTH is the maximum signal
   number width, and SIGNUM is the signal number to print.  The
   maximum name width is NAME_WIDTH, and SIGNAME is the name to print.  */

static void
print_table_row (unsigned int num_width, int signum,
                 unsigned int name_width, char const *signame)
{
  char const *description = strsignal (signum);
  printf ("%*d %-*s %s\n", num_width, signum, name_width, signame,
          description ? description : "?");
}

