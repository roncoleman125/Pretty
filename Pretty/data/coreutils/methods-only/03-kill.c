/* Send signal SIGNUM to all the processes or process groups specified
   by ARGV.  Return a suitable exit status.  */

static int
send_signals (int signum, char *const *argv)
{
  int status = EXIT_SUCCESS;
  char const *arg = *argv;

  do
    {
      char *endp;
      intmax_t n = (errno = 0, strtoimax (arg, &endp, 10));
      pid_t pid = n;

      if (errno == ERANGE || pid != n || arg == endp || *endp)
        {
          error (0, 0, _("%s: invalid process id"), arg);
          status = EXIT_FAILURE;
        }
      else if (kill (pid, signum) != 0)
        {
          error (0, errno, "%s", arg);
          status = EXIT_FAILURE;
        }
    }
  while ((arg = *++argv));

  return status;
}

