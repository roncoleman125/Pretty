/* Return true if KEY is a numeric key.  */

static inline bool
key_numeric (struct keyfield const *key)
{
  return key->numeric || key->general_numeric || key->human_numeric;
}

