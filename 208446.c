static void check_redraw(uint32_t flags)
{
  /* Careful: P_RCLR and P_RALL are a combination of other P_ flags */
  bool doclear = (flags & P_RCLR) == P_RCLR;
  bool all = ((flags & P_RALL) == P_RALL || doclear);

  if ((flags & P_RSTAT) || all)         /* mark all status lines dirty */
    status_redraw_all();

  if ((flags & P_RBUF) || (flags & P_RWIN) || all)
    changed_window_setting();
  if (flags & P_RBUF)
    redraw_curbuf_later(NOT_VALID);
  if (doclear)
    redraw_all_later(CLEAR);
  else if (all)
    redraw_all_later(NOT_VALID);
}