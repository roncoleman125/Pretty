/* Record LINE_START, which is the address of the start of a line
   of length LINE_LEN in the large buffer, in the lines buffer of B. */

static void
keep_new_line (struct buffer_record *b, char *line_start, size_t line_len)
{
  struct line *l;

  /* If there is no existing area to keep line info, get some. */
  if (b->line_start == NULL)
    b->line_start = b->curr_line = new_line_control ();

  /* If existing area for lines is full, get more. */
  if (b->curr_line->used == CTRL_SIZE)
    {
      b->curr_line->next = new_line_control ();
      b->curr_line = b->curr_line->next;
    }

  l = b->curr_line;

  /* Record the start of the line, and update counters. */
  l->starts[l->insert_index].str = line_start;
  l->starts[l->insert_index].len = line_len;
  l->used++;
  l->insert_index++;
}

