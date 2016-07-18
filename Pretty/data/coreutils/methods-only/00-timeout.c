/* send sig to group but not ourselves.
 * FIXME: Is there a better way to achieve this?  */
static int
send_sig (int where, int sig)
{
  sigs_to_ignore[sig] = 1;
  return kill (where, sig);
}

