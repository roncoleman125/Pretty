/* Return a new, initialized control record. */

static struct control *
new_control_record (void)
{
  static size_t control_allocated = 0; /* Total space allocated. */
  struct control *p;

  if (control_used == control_allocated)
    controls = X2NREALLOC (controls, &control_allocated);
  p = &controls[control_used++];
  p->regexpr = false;
  p->repeat = 0;
  p->repeat_forever = false;
  p->lines_required = 0;
  p->offset = 0;
  return p;
}

