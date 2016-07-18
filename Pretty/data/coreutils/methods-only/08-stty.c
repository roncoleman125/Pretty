static void
display_settings (enum output_type output_type, struct termios *mode,
                  char const *device_name)
{
  switch (output_type)
    {
    case changed:
      display_changed (mode);
      break;

    case all:
      display_all (mode, device_name);
      break;

    case recoverable:
      display_recoverable (mode);
      break;
    }
}

