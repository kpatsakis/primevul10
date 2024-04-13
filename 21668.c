get_cmd_xmap_from_keymap (kmap)
     Keymap kmap;
{
  if (emacs_std_cmd_xmap == 0)
    init_unix_command_map ();

  if (kmap == emacs_standard_keymap)
    return emacs_std_cmd_xmap;
  else if (kmap == emacs_meta_keymap)
    return (FUNCTION_TO_KEYMAP (emacs_std_cmd_xmap, ESC));
  else if (kmap == emacs_ctlx_keymap)
    return (FUNCTION_TO_KEYMAP (emacs_std_cmd_xmap, CTRL('X')));
#if defined (VI_MODE)
  else if (kmap == vi_insertion_keymap)
    return vi_insert_cmd_xmap;
  else if (kmap == vi_movement_keymap)
    return vi_movement_cmd_xmap;
#endif
  else
    return (Keymap)NULL;
}