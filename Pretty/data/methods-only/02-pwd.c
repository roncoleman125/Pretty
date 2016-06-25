static struct file_name *
file_name_init (void)
{
  struct file_name *p = xmalloc (sizeof *p);

  /* Start with a buffer larger than PATH_MAX, but beware of systems
     on which PATH_MAX is very large -- e.g., INT_MAX.  */
  p->n_alloc = MIN (2 * PATH_MAX, 32 * 1024);

  p->buf = xmalloc (p->n_alloc);
  p->start = p->buf + (p->n_alloc - 1);
  p->start[0] = '\0';
  return p;
}

