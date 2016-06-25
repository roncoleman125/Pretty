static inline int
cmp_size (struct fileinfo const *a, struct fileinfo const *b,
          int (*cmp) (char const *, char const *))
{
  int diff = longdiff (b->stat.st_size, a->stat.st_size);
  return diff ? diff : cmp (a->name, b->name);
}

