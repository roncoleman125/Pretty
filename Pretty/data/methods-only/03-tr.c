/* Return true if the character C is a member of the
   character class CHAR_CLASS.  */

static bool
is_char_class_member (enum Char_class char_class, unsigned char c)
{
  int result;

  switch (char_class)
    {
    case CC_ALNUM:
      result = isalnum (c);
      break;
    case CC_ALPHA:
      result = isalpha (c);
      break;
    case CC_BLANK:
      result = isblank (c);
      break;
    case CC_CNTRL:
      result = iscntrl (c);
      break;
    case CC_DIGIT:
      result = isdigit (c);
      break;
    case CC_GRAPH:
      result = isgraph (c);
      break;
    case CC_LOWER:
      result = islower (c);
      break;
    case CC_PRINT:
      result = isprint (c);
      break;
    case CC_PUNCT:
      result = ispunct (c);
      break;
    case CC_SPACE:
      result = isspace (c);
      break;
    case CC_UPPER:
      result = isupper (c);
      break;
    case CC_XDIGIT:
      result = isxdigit (c);
      break;
    default:
      abort ();
      break;
    }

  return !! result;
}

