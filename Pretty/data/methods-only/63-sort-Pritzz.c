/* Create and return a merge tree for NTHREADS threads, sorting NLINES
   lines, with destination DEST.  */
static struct merge_node *
merge_tree_init (size_t nthreads, size_t nlines, struct line *dest)
{
  struct merge_node *merge_tree = xmalloc (2 * sizeof *merge_tree * nthreads);

  struct merge_node *root = merge_tree;
  root->lo = root->hi = root->end_lo = root->end_hi = NULL;
  root->dest = NULL;
  root->nlo = root->nhi = nlines;
  root->parent = NULL;
  root->level = MERGE_END;
  root->queued = false;
  pthread_mutex_init (&root->lock, NULL);

  init_node (root, root + 1, dest, nthreads, nlines, false);
  return merge_tree;
}

