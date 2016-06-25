/* Just on general principles, wipe buffers containing information
   that may be related to the possibly-pseudorandom values used during
   shredding.  */
static void
clear_random_data (void)
{
  randint_all_free (randint_source);
}


