/* Initialize the randomly chosen MD5 state.  */

static void
random_md5_state_init (char const *random_source)
{
  unsigned char buf[MD5_DIGEST_SIZE];
  struct randread_source *r = randread_new (random_source, sizeof buf);
  if (! r)
    die (_("open failed"), random_source);
  randread (r, buf, sizeof buf);
  if (randread_free (r) != 0)
    die (_("close failed"), random_source);
  md5_init_ctx (&random_md5_state);
  md5_process_bytes (buf, sizeof buf, &random_md5_state);
}

