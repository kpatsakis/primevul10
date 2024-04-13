set_bash_input ()
{
  /* Make sure the fd from which we are reading input is not in
     no-delay mode. */
#if defined (BUFFERED_INPUT)
  if (interactive == 0)
    sh_unset_nodelay_mode (default_buffered_input);
  else
#endif /* !BUFFERED_INPUT */
    sh_unset_nodelay_mode (fileno (stdin));

  /* with_input_from_stdin really means `with_input_from_readline' */
  if (interactive && no_line_editing == 0)
    with_input_from_stdin ();
#if defined (BUFFERED_INPUT)
  else if (interactive == 0)
    with_input_from_buffered_stream (default_buffered_input, dollar_vars[0]);
#endif /* BUFFERED_INPUT */
  else
    with_input_from_stream (default_input, dollar_vars[0]);
}