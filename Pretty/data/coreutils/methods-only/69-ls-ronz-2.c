static size_t
print_name_with_quoting (const struct fileinfo *f,
                         bool symlink_target,
                         struct obstack *stack,
                         size_t start_col)
{
  const char* name = symlink_target ? f->linkname : f->name;

  bool used_color_this_time
    = (print_with_color
        && (print_color_indicator (f, symlink_target)
            || is_colored (C_NORM)));

  if (stack)
    PUSH_CURRENT_DIRED_POS (stack);

  size_t width = quote_name (stdout, name, filename_quoting_options, NULL);
  dired_pos += width;

  if (stack)
    PUSH_CURRENT_DIRED_POS (stack);

  if (used_color_this_time)
    {
      process_signals ();
      prep_non_filename_text ();
      if (start_col / line_length != (start_col + width - 1) / line_length)
        put_indicator (&color_indicator[C_CLR_TO_EOL]);
    }

  return width;
}

