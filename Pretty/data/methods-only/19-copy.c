static bool
valid_options (const struct cp_options *co)
{
  assert (co != NULL);
  assert (VALID_BACKUP_TYPE (co->backup_type));
  assert (VALID_SPARSE_MODE (co->sparse_mode));
  assert (VALID_REFLINK_MODE (co->reflink_mode));
  assert (!(co->hard_link && co->symbolic_link));
  assert (!
          (co->reflink_mode == REFLINK_ALWAYS
           && co->sparse_mode != SPARSE_AUTO));
  return true;
}

