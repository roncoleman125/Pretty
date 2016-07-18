/* Initialize existing line record P. */

static void
clear_line_control (struct line *p)
{
  p->used = 0;
  p->insert_index = 0;
  p->retrieve_index = 0;
}

