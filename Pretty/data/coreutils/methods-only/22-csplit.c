static void
handle_line_error (const struct control *p, uintmax_t repetition)
{
  char buf[INT_BUFSIZE_BOUND (uintmax_t)];

  fprintf (stderr, _("%s: %s: line number out of range"),
           program_name, quote (umaxtostr (p->lines_required, buf)));
  if (repetition)
    fprintf (stderr, _(" on repetition %s\n"), umaxtostr (repetition, buf));
  else
    fprintf (stderr, "\n");

  cleanup_fatal ();
}

