static bool
dev_ino_compare (void const *x, void const *y)
{
  struct dev_ino const *a = x;
  struct dev_ino const *b = y;
  return SAME_INODE (*a, *b) ? true : false;
}

