void win_copy_options(win_T *wp_from, win_T *wp_to)
{
  copy_winopt(&wp_from->w_onebuf_opt, &wp_to->w_onebuf_opt);
  copy_winopt(&wp_from->w_allbuf_opt, &wp_to->w_allbuf_opt);
  /* Is this right? */
  wp_to->w_farsi = wp_from->w_farsi;
  briopt_check(wp_to);
}