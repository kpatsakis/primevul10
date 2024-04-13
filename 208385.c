bool set_tty_option(char *name, char *value)
{
  if (!strcmp(name, "t_Co")) {
    int colors = atoi(value);

    // Only reinitialize colors if t_Co value has really changed to
    // avoid expensive reload of colorscheme if t_Co is set to the
    // same value multiple times
    if (colors != t_colors) {
      t_colors = colors;
      // We now have a different color setup, initialize it again.
      init_highlight(TRUE, FALSE);
    }

    return true;
  }

  return is_tty_option(name) || !strcmp(name, "term")
    || !strcmp(name, "ttytype");
}