did_set_title (
    int icon                   /* Did set icon instead of title */
)
{
  if (starting != NO_SCREEN) {
    maketitle();
    if (icon) {
      if (!p_icon) {
        ui_set_icon(NULL);
      }
    } else {
      if (!p_title) {
        ui_set_title(NULL);
      }
    }
  }
}