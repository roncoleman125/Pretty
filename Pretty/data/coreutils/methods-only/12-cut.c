static void
cut_stream (FILE *stream)
{
  if (operating_mode == byte_mode)
    cut_bytes (stream);
  else
    cut_fields (stream);
}

