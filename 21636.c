bash_kill_shellword (count, key)
     int count, key;
{
  int p;

  if (count < 0)
    return (bash_backward_kill_shellword (-count, key));

  p = rl_point;
  bash_forward_shellword (count, key);

  if (rl_point != p)
    rl_kill_text (p, rl_point);

  rl_point = p;
  if (rl_editing_mode == EMACS_EDITING_MODE)	/* 1 == emacs_mode */
    rl_mark = rl_point;

  return 0;
}