/* Recurse (sub)tree rooted at ROOT, calling ACTION for each node.
   Stop when ACTION returns true.  */

static bool
recurse_tree (struct item *root, bool (*action) (struct item *))
{
  if (root->left == NULL && root->right == NULL)
    return (*action) (root);
  else
    {
      if (root->left != NULL)
        if (recurse_tree (root->left, action))
          return true;
      if ((*action) (root))
        return true;
      if (root->right != NULL)
        if (recurse_tree (root->right, action))
          return true;
    }

  return false;
}

