/* Walk the tree specified by the head ROOT, calling ACTION for
   each node.  */

static void
walk_tree (struct item *root, bool (*action) (struct item *))
{
  if (root->right)
    recurse_tree (root->right, action);
}

