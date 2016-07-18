/* Sort lines, possibly in parallel.  The arguments are as in struct
   thread_args above.

   The algorithm has three phases: node creation, sequential sort,
   and binary merge.

   During node creation, sortlines recursively visits each node in the
   binary merge tree and creates a NODE structure corresponding to all the
   future line merging NODE is responsible for. For each call to
   sortlines, half the available threads are assigned to each recursive
   call, until a leaf node having only 1 available thread is reached.

   Each leaf node then performs two sequential sorts, one on each half of
   the lines it is responsible for. It records in its NODE structure that
   there are two sorted sublists available to merge from, and inserts its
   NODE into the priority queue.

   The binary merge phase then begins. Each thread drops into a loop
   where the thread retrieves a NODE from the priority queue, merges lines
   available to that NODE, and potentially insert NODE or its parent back
   into the queue if there are sufficient available lines for them to
   merge. This continues until all lines at all nodes of the merge tree
   have been merged. */

static void
sortlines (struct line *restrict lines, size_t nthreads,
           size_t total_lines, struct merge_node *node, bool is_lo_child,
           struct merge_node_queue *queue, FILE *tfp, char const *temp_output)
{
  size_t nlines = node->nlo + node->nhi;

  /* Calculate thread arguments. */
  size_t lo_threads = nthreads / 2;
  size_t hi_threads = nthreads - lo_threads;
  pthread_t thread;
  struct thread_args args = {lines, lo_threads, total_lines,
                             node->lo_child, true, queue, tfp, temp_output};

  if (nthreads > 1 && SUBTHREAD_LINES_HEURISTIC <= nlines
      && pthread_create (&thread, NULL, sortlines_thread, &args) == 0)
    {
      sortlines (lines - node->nlo, hi_threads, total_lines,
                 node->hi_child, false, queue, tfp, temp_output);
      pthread_join (thread, NULL);
    }
  else
    {
      /* Nthreads = 1, this is a leaf NODE, or pthread_create failed.
         Sort with 1 thread. */
      size_t nlo = node->nlo;
      size_t nhi = node->nhi;
      struct line *temp = lines - total_lines;
      if (1 < nhi)
        sequential_sort (lines - nlo, nhi, temp - nlo / 2, false);
      if (1 < nlo)
        sequential_sort (lines, nlo, temp, false);

      /* Update merge NODE. No need to lock yet. */
      node->lo = lines;
      node->hi = lines - nlo;
      node->end_lo = lines - nlo;
      node->end_hi = lines - nlo - nhi;

      queue_insert (queue, node);
      merge_loop (queue, total_lines, tfp, temp_output);
    }

  pthread_mutex_destroy (&node->lock);
}

