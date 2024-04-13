int find_key_option_len(const char_u *arg, size_t len)
{
  int key;
  int modifiers;

  // Don't use get_special_key_code() for t_xx, we don't want it to call
  // add_termcap_entry().
  if (len >= 4 && arg[0] == 't' && arg[1] == '_') {
    key = TERMCAP2KEY(arg[2], arg[3]);
  } else {
    arg--;  // put arg at the '<'
    modifiers = 0;
    key = find_special_key(&arg, len + 1, &modifiers, true, true);
    if (modifiers) {  // can't handle modifiers here
      key = 0;
    }
  }
  return key;
}