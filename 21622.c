unset_bash_input (check_zero)
     int check_zero;
{
#if defined (BUFFERED_INPUT)
  if ((check_zero && default_buffered_input >= 0) ||
      (check_zero == 0 && default_buffered_input > 0))
    {
      close_buffered_fd (default_buffered_input);
      default_buffered_input = bash_input.location.buffered_fd = -1;
      bash_input.type = st_none;		/* XXX */
    }
#else /* !BUFFERED_INPUT */
  if (default_input)
    {
      fclose (default_input);
      default_input = (FILE *)NULL;
    }
#endif /* !BUFFERED_INPUT */
}