/* Record the fact that J precedes K.  */

static void
record_relation (struct item *j, struct item *k)
{
  struct successor *p;

  if (!STREQ (j->str, k->str))
    {
      k->count++;
      p = xmalloc (sizeof *p);
      p->suc = k;
      p->next = j->top;
      j->top = p;
    }
}

