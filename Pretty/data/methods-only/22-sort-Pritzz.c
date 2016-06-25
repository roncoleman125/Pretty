/* Create a temporary file and, if asked for, start a compressor
   to that file.  Set *PFP to the file handle and return the address
   of the new temp node.  Die on failure.  */

static struct tempnode *
create_temp (FILE **pfp)
{
  return maybe_create_temp (pfp, false);
}

/* Open a compressed temp file and start a decompression process through
   which to filter the input.  Return NULL (setting errno to
   EMFILE) if we ran out of file descriptors, and die on any other
   kind of failure.  */

static FILE *
open_temp (struct tempnode *temp)
{
  int tempfd, pipefds[2];
  FILE *fp = NULL;

  if (temp->state == UNREAPED)
    wait_proc (temp->pid);

  tempfd = open (temp->name, O_RDONLY);
  if (tempfd < 0)
    return NULL;

  pid_t child = pipe_fork (pipefds, MAX_FORK_TRIES_DECOMPRESS);

  switch (child)
    {
    case -1:
      if (errno != EMFILE)
        error (SORT_FAILURE, errno, _("couldn't create process for %s -d"),
               compress_program);
      close (tempfd);
      errno = EMFILE;
      break;

    case 0:
      close (pipefds[0]);
      dup2_or_die (tempfd, STDIN_FILENO);
      close (tempfd);
      dup2_or_die (pipefds[1], STDOUT_FILENO);
      close (pipefds[1]);

      execlp (compress_program, compress_program, "-d", (char *) NULL);
      error (SORT_FAILURE, errno, _("couldn't execute %s -d"),
             compress_program);

    default:
      temp->pid = child;
      register_proc (temp);
      close (tempfd);
      close (pipefds[1]);

      fp = fdopen (pipefds[0], "r");
      if (! fp)
        {
          int saved_errno = errno;
          close (pipefds[0]);
          errno = saved_errno;
        }
      break;
    }

  return fp;
}

