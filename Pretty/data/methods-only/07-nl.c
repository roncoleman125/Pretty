/* Return the type of line in `line_buf'. */

static enum section
check_section (void)
{
  size_t len = line_buf.length - 1;

  if (len < 2 || memcmp (line_buf.buffer, section_del, 2))
    return Text;
  if (len == header_del_len
      && !memcmp (line_buf.buffer, header_del, header_del_len))
    return Header;
  if (len == body_del_len
      && !memcmp (line_buf.buffer, body_del, body_del_len))
    return Body;
  if (len == footer_del_len
      && !memcmp (line_buf.buffer, footer_del, footer_del_len))
    return Footer;
  return Text;
}

