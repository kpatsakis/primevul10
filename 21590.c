posix_readline_initialize (on_or_off)
     int on_or_off;
{
  static char kseq[2] = { CTRL ('I'), 0 };		/* TAB */

  if (on_or_off)
    rl_variable_bind ("comment-begin", "#");
#if defined (VI_MODE)
  if (on_or_off)
    {
      vi_tab_binding = rl_function_of_keyseq (kseq, vi_insertion_keymap, (int *)NULL);
      rl_bind_key_in_map (CTRL ('I'), rl_insert, vi_insertion_keymap);
    }
  else
    {
      if (rl_function_of_keyseq (kseq, vi_insertion_keymap, (int *)NULL) == rl_insert)
        rl_bind_key_in_map (CTRL ('I'), vi_tab_binding, vi_insertion_keymap);
    }
#endif
}