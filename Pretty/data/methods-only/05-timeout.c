static void
install_signal_handlers (int sigterm)
{
  struct sigaction sa;
  sigemptyset (&sa.sa_mask);  /* Allow concurrent calls to handler */
  sa.sa_handler = cleanup;
  sa.sa_flags = SA_RESTART;  /* restart syscalls (like wait() below) */

  sigaction (SIGALRM, &sa, NULL); /* our timeout.  */
  sigaction (SIGINT, &sa, NULL);  /* Ctrl-C at terminal for example.  */
  sigaction (SIGQUIT, &sa, NULL); /* Ctrl-\ at terminal for example.  */
  sigaction (SIGHUP, &sa, NULL);  /* terminal closed for example.  */
  sigaction (SIGTERM, &sa, NULL); /* if we're killed, stop monitored proc.  */
  sigaction (sigterm, &sa, NULL); /* user specified termination signal.  */
}

