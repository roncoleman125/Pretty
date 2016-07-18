/* Destroy the merge tree. */
static void
merge_tree_destroy (struct merge_node *merge_tree)
{
  free (merge_tree);
}

