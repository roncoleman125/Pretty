/* Output a single byte, reporting any write errors.  */

static inline void
xputchar (char c)
{
  if (putchar (c) < 0)
    write_error ();
}

