get_line (void)
{
   return pget_line (p_indent, p_rfc934_nesting, p_strip_trailing_cr,
		     p_pass_comments_through);
}