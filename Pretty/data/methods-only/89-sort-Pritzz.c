/* Initialize KEY.  */

static struct keyfield *
key_init (struct keyfield *key)
{
  memset (key, 0, sizeof *key);
  key->eword = SIZE_MAX;
  return key;
}

