static void
free_pending_ent (struct pending *p)
{
  free (p->name);
  free (p->realname);
  free (p);
}

