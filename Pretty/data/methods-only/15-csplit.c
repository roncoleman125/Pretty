/* Return the line number of the first line that has not yet been retrieved. */

static uintmax_t
get_first_line_in_buffer (void)
{
  if (head == NULL && !load_buffer ())
    error (EXIT_FAILURE, errno, _("input disappeared"));

  return head->first_available;
}

