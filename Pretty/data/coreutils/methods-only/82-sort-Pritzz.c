/* Sort NFILES FILES onto OUTPUT_FILE.  Use at most NTHREADS threads.  */

static void
sort (char *const *files, size_t nfiles, char const *output_file,
      size_t nthreads)
{
  struct buffer buf;
  IF_LINT (buf.buf = NULL);
  size_t ntemps = 0;
  bool output_file_created = false;

  buf.alloc = 0;

  while (nfiles)
    {
      char const *temp_output;
      char const *file = *files;
      FILE *fp = xfopen (file, "r");
      FILE *tfp;

      size_t bytes_per_line;
      if (nthreads > 1)
        {
          /* Get log P. */
          size_t tmp = 1;
          size_t mult = 1;
          while (tmp < nthreads)
            {
              tmp *= 2;
              mult++;
            }
          bytes_per_line = (mult * sizeof (struct line));
        }
      else
        bytes_per_line = sizeof (struct line) * 3 / 2;

      if (! buf.alloc)
        initbuf (&buf, bytes_per_line,
                 sort_buffer_size (&fp, 1, files, nfiles, bytes_per_line));
      buf.eof = false;
      files++;
      nfiles--;

      while (fillbuf (&buf, fp, file))
        {
          struct line *line;

          if (buf.eof && nfiles
              && (bytes_per_line + 1
                  < (buf.alloc - buf.used - bytes_per_line * buf.nlines)))
            {
              /* End of file, but there is more input and buffer room.
                 Concatenate the next input file; this is faster in
                 the usual case.  */
              buf.left = buf.used;
              break;
            }

          line = buffer_linelim (&buf);
          if (buf.eof && !nfiles && !ntemps && !buf.left)
            {
              xfclose (fp, file);
              tfp = xfopen (output_file, "w");
              temp_output = output_file;
              output_file_created = true;
            }
          else
            {
              ++ntemps;
              temp_output = create_temp (&tfp)->name;
            }
          if (1 < buf.nlines)
            {
              struct merge_node_queue queue;
              queue_init (&queue, nthreads);
              struct merge_node *merge_tree =
                merge_tree_init (nthreads, buf.nlines, line);
              struct merge_node *root = merge_tree + 1;

              sortlines (line, nthreads, buf.nlines, root,
                         true, &queue, tfp, temp_output);
              queue_destroy (&queue);
              pthread_mutex_destroy (&root->lock);
              merge_tree_destroy (merge_tree);
            }
          else
            write_unique (line - 1, tfp, temp_output);

          xfclose (tfp, temp_output);

          if (output_file_created)
            goto finish;
        }
      xfclose (fp, file);
    }

 finish:
  free (buf.buf);

  if (! output_file_created)
    {
      size_t i;
      struct tempnode *node = temphead;
      struct sortfile *tempfiles = xnmalloc (ntemps, sizeof *tempfiles);
      for (i = 0; node; i++)
        {
          tempfiles[i].name = node->name;
          tempfiles[i].temp = node;
          node = node->next;
        }
      merge (tempfiles, ntemps, ntemps, output_file);
      free (tempfiles);
    }

  reap_all ();
}

