static void
file_name_free (struct file_name *p)
{
  free (p->buf);
  free (p);
}

