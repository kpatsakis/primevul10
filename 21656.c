print_unix_command_map ()
{
  Keymap save, cmd_xmap;

  save = rl_get_keymap ();
  cmd_xmap = get_cmd_xmap_from_keymap (save);
  rl_set_keymap (cmd_xmap);
  rl_macro_dumper (1);
  rl_set_keymap (save);
  return 0;
}