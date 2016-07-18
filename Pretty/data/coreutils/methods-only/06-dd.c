static void ATTRIBUTE_NORETURN
quit (int code)
{
  cleanup ();
  print_stats ();
  process_signals ();
  exit (code);
}

