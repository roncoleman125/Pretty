/* When effecting a move (e.g., for mv(1)), and given the name DST_NAME
   of the destination and a corresponding stat buffer, DST_SB, return
   true if the logical `move' operation should _not_ proceed.
   Otherwise, return false.
   Depending on options specified in X, this code may issue an
   interactive prompt asking whether it's ok to overwrite DST_NAME.  */
static bool
abandon_move (const struct cp_options *x,
              char const *dst_name,
              struct stat const *dst_sb)
{
  assert (x->move_mode);
  return (x->interactive == I_ALWAYS_NO
          || ((x->interactive == I_ASK_USER
               || (x->interactive == I_UNSPECIFIED
                   && x->stdin_tty
                   && ! writable_destination (dst_name, dst_sb->st_mode)))
              && (overwrite_prompt (dst_name, dst_sb), 1)
              && ! yesno ()));
}

