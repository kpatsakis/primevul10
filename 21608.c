get_cmd_xmap_from_edit_mode ()
{
  if (emacs_std_cmd_xmap == 0)
    init_unix_command_map ();

  switch (rl_editing_mode)
    {
    case EMACS_EDITING_MODE:
      return emacs_std_cmd_xmap;
#if defined (VI_MODE)
    case VI_EDITING_MODE:
      return (get_cmd_xmap_from_keymap (rl_get_keymap ()));
#endif
    default:
      return (Keymap)NULL;
    }
}