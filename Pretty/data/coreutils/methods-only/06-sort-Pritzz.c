static pid_t
reap (pid_t pid)
{
  int status;
  pid_t cpid = waitpid ((pid ? pid : -1), &status, (pid ? 0 : WNOHANG));

  if (cpid < 0)
    error (SORT_FAILURE, errno, _("waiting for %s [-d]"),
           compress_program);
  else if (0 < cpid && (0 < pid || delete_proc (cpid)))
    {
      if (! WIFEXITED (status) || WEXITSTATUS (status))
        error (SORT_FAILURE, 0, _("%s [-d] terminated abnormally"),
               compress_program);
      --nprocs;
    }

  return cpid;
}

