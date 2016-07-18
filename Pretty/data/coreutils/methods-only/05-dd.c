static void
cleanup (void)
{
  if (close (STDIN_FILENO) < 0)
    error (EXIT_FAILURE, errno,
           _("closing input file %s"), quote (input_file));

  /* Don't remove this call to close, even though close_stdout
     closes standard output.  This close is necessary when cleanup
     is called as part of a signal handler.  */
  if (close (STDOUT_FILENO) < 0)
    error (EXIT_FAILURE, errno,
           _("closing output file %s"), quote (output_file));
}

