static void
diagnose_copy_fd_failure (enum Copy_fd_status err, char const *filename)
{
  switch (err)
    {
    case COPY_FD_READ_ERROR:
      error (0, errno, _("error reading %s"), quote (filename));
      break;
    case COPY_FD_WRITE_ERROR:
      error (0, errno, _("error writing %s"), quote (filename));
      break;
    case COPY_FD_UNEXPECTED_EOF:
      error (0, errno, _("%s: file has shrunk too much"), quote (filename));
      break;
    default:
      abort ();
    }
}

