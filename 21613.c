init_unix_command_map ()
{
  emacs_std_cmd_xmap = rl_make_bare_keymap ();

  emacs_std_cmd_xmap[CTRL('X')].type = ISKMAP;
  emacs_std_cmd_xmap[CTRL('X')].function = KEYMAP_TO_FUNCTION (rl_make_bare_keymap ());
  emacs_std_cmd_xmap[ESC].type = ISKMAP;
  emacs_std_cmd_xmap[ESC].function = KEYMAP_TO_FUNCTION (rl_make_bare_keymap ());

#if defined (VI_MODE)  
  vi_insert_cmd_xmap = rl_make_bare_keymap ();
  vi_movement_cmd_xmap = rl_make_bare_keymap ();
#endif
}