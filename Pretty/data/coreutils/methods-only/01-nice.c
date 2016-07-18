static bool
perm_related_errno (int err)
{
  return err == EACCES || err == EPERM;
}

