/* Return a new, initialized line record. */

static struct line *
new_line_control (void)
{
  struct line *p = xmalloc (sizeof *p);

  p->next = NULL;
  clear_line_control (p);

  return p;
}

