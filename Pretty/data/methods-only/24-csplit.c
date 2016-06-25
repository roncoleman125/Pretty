static void
regexp_error (struct control *p, uintmax_t repetition, bool ignore)
{
  fprintf (stderr, _("%s: %s: match not found"),
           program_name, quote (global_argv[p->argnum]));

  if (repetition)
    {
      char buf[INT_BUFSIZE_BOUND (uintmax_t)];
      fprintf (stderr, _(" on repetition %s\n"), umaxtostr (repetition, buf));
    }
  else
    fprintf (stderr, "\n");

  if (!ignore)
    {
      dump_rest_of_file ();
      close_output_file ();
    }
  cleanup_fatal ();
}

