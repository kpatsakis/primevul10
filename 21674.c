init_interactive ()
{
  expand_aliases = interactive_shell = startup_state = 1;
  interactive = 1;
#if defined (HISTORY)
  remember_on_history = enable_history_list = 1;	/* XXX */
#  if defined (BANG_HISTORY)
  histexp_flag = history_expansion;			/* XXX */
#  endif
#endif
}