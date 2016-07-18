/* FIXME: Write a string to standard out, while watching for "dangerous"
   sequences like unescaped : and = characters.  */

static void
append_quoted (const char *str)
{
  bool need_backslash = true;

  while (*str != '\0')
    {
      switch (*str)
        {
        case '\'':
          APPEND_CHAR ('\'');
          APPEND_CHAR ('\\');
          APPEND_CHAR ('\'');
          need_backslash = true;
          break;

        case '\\':
        case '^':
          need_backslash = !need_backslash;
          break;

        case ':':
        case '=':
          if (need_backslash)
            APPEND_CHAR ('\\');
          /* Fall through */

        default:
          need_backslash = true;
          break;
        }

      APPEND_CHAR (*str);
      ++str;
    }
}

