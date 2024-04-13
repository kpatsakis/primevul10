init_interactive_script ()
{
  init_noninteractive ();
  expand_aliases = interactive_shell = startup_state = 1;
#if defined (HISTORY)
  remember_on_history = enable_history_list = 1;	/* XXX */
#endif
}