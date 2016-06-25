/* Create a temporary file and, if asked for, start a compressor
   to that file.  Set *PFP to the file handle and return
   the address of the new temp node.  If the creation
   fails, return NULL if the failure is due to file descriptor
   exhaustion and SURVIVE_FD_EXHAUSTION; otherwise, die.  */

static struct tempnode *
maybe_create_temp (FILE **pfp, bool survive_fd_exhaustion)
{
  int tempfd;
  struct tempnode *node = create_temp_file (&tempfd, survive_fd_exhaustion);
  if (! node)
    return NULL;

  node->state = UNCOMPRESSED;

  if (compress_program)
    {
      int pipefds[2];

      node->pid = pipe_fork (pipefds, MAX_FORK_TRIES_COMPRESS);
      if (0 < node->pid)
        {
          close (tempfd);
          close (pipefds[0]);
          tempfd = pipefds[1];

          register_proc (node);
        }
      else if (node->pid == 0)
        {
          close (pipefds[1]);
          dup2_or_die (tempfd, STDOUT_FILENO);
          close (tempfd);
          dup2_or_die (pipefds[0], STDIN_FILENO);
          close (pipefds[0]);

          if (execlp (compress_program, compress_program, (char *) NULL) < 0)
            error (SORT_FAILURE, errno, _("couldn't execute %s"),
                   compress_program);
        }
    }

  *pfp = fdopen (tempfd, "w");
  if (! *pfp)
    die (_("couldn't create temporary file"), node->name);

  return node;
}

