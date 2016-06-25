/* Search binary tree rooted at *ROOT for STR.  Allocate a new tree if
   *ROOT is NULL.  Insert a node/item for STR if not found.  Return
   the node/item found/created for STR.

   This is done according to Algorithm A (Balanced tree search and
   insertion) in Donald E. Knuth, The Art of Computer Programming,
   Volume 3/Searching and Sorting, pages 455--457.  */

static struct item *
search_item (struct item *root, const char *str)
{
  struct item *p, *q, *r, *s, *t;
  int a;

  assert (root);

  /* Make sure the tree is not empty, since that is what the algorithm
     below expects.  */
  if (root->right == NULL)
    return (root->right = new_item (str));

  /* A1. Initialize.  */
  t = root;
  s = p = root->right;

  while (true)
    {
      /* A2. Compare.  */
      a = strcmp (str, p->str);
      if (a == 0)
        return p;

      /* A3 & A4.  Move left & right.  */
      if (a < 0)
        q = p->left;
      else
        q = p->right;

      if (q == NULL)
        {
          /* A5. Insert.  */
          q = new_item (str);

          /* A3 & A4.  (continued).  */
          if (a < 0)
            p->left = q;
          else
            p->right = q;

          /* A6. Adjust balance factors.  */
          assert (!STREQ (str, s->str));
          if (strcmp (str, s->str) < 0)
            {
              r = p = s->left;
              a = -1;
            }
          else
            {
              r = p = s->right;
              a = 1;
            }

          while (p != q)
            {
              assert (!STREQ (str, p->str));
              if (strcmp (str, p->str) < 0)
                {
                  p->balance = -1;
                  p = p->left;
                }
              else
                {
                  p->balance = 1;
                  p = p->right;
                }
            }

          /* A7. Balancing act.  */
          if (s->balance == 0 || s->balance == -a)
            {
              s->balance += a;
              return q;
            }

          if (r->balance == a)
            {
              /* A8. Single Rotation.  */
              p = r;
              if (a < 0)
                {
                  s->left = r->right;
                  r->right = s;
                }
              else
                {
                  s->right = r->left;
                  r->left = s;
                }
              s->balance = r->balance = 0;
            }
          else
            {
              /* A9. Double rotation.  */
              if (a < 0)
                {
                  p = r->right;
                  r->right = p->left;
                  p->left = r;
                  s->left = p->right;
                  p->right = s;
                }
              else
                {
                  p = r->left;
                  r->left = p->right;
                  p->right = r;
                  s->right = p->left;
                  p->left = s;
                }

              s->balance = 0;
              r->balance = 0;
              if (p->balance == a)
                s->balance = -a;
              else if (p->balance == -a)
                r->balance = a;
              p->balance = 0;
            }

          /* A10. Finishing touch.  */
          if (s == t->right)
            t->right = p;
          else
            t->left = p;

          return q;
        }

      /* A3 & A4.  (continued).  */
      if (q->balance)
        {
          t = p;
          s = q;
        }

      p = q;
    }

  /* NOTREACHED */
}

