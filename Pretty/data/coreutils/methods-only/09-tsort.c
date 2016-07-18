/* Do a topological sort on FILE.   Return true if successful.  */

static bool
tsort (const char *file)
{
  bool ok = true;
  struct item *root;
  struct item *j = NULL;
  struct item *k = NULL;
  token_buffer tokenbuffer;
  bool is_stdin = STREQ (file, "-");

  /* Intialize the head of the tree will hold the strings we're sorting.  */
  root = new_item (NULL);

  if (!is_stdin && ! freopen (file, "r", stdin))
    error (EXIT_FAILURE, errno, "%s", file);

  fadvise (stdin, FADVISE_SEQUENTIAL);

  init_tokenbuffer (&tokenbuffer);

  while (1)
    {
      /* T2. Next Relation.  */
      size_t len = readtoken (stdin, DELIM, sizeof (DELIM) - 1, &tokenbuffer);
      if (len == (size_t) -1)
        break;

      assert (len != 0);

      k = search_item (root, tokenbuffer.buffer);
      if (j)
        {
          /* T3. Record the relation.  */
          record_relation (j, k);
          k = NULL;
        }

      j = k;
    }

  if (k != NULL)
    error (EXIT_FAILURE, 0, _("%s: input contains an odd number of tokens"),
           file);

  /* T1. Initialize (N <- n).  */
  walk_tree (root, count_items);

  while (n_strings > 0)
    {
      /* T4. Scan for zeros.  */
      walk_tree (root, scan_zeros);

      while (head)
        {
          struct successor *p = head->top;

          /* T5. Output front of queue.  */
          puts (head->str);
          /* suppress valgrind "definitely lost" warnings.  */
          void *head_str = (void *) head->str;
          free (head_str);
          head->str = NULL;	/* Avoid printing the same string twice.  */
          n_strings--;

          /* T6. Erase relations.  */
          while (p)
            {
              p->suc->count--;
              if (p->suc->count == 0)
                {
                  zeros->qlink = p->suc;
                  zeros = p->suc;
                }

              p = p->next;
            }

          /* T7. Remove from queue.  */
          head = head->qlink;
        }

      /* T8.  End of process.  */
      if (n_strings > 0)
        {
          /* The input contains a loop.  */
          error (0, 0, _("%s: input contains a loop:"), file);
          ok = false;

          /* Print the loop and remove a relation to break it.  */
          do
            walk_tree (root, detect_loop);
          while (loop);
        }
    }

  if (fclose (stdin) != 0)
    error (EXIT_FAILURE, errno, "%s",
           is_stdin ? _("standard input") : quote (file));

  return ok;
}

