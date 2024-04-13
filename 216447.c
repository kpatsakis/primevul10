void WebContents::Focus() {
  // Focusing on WebContents does not automatically focus the window on macOS
  // and Linux, do it manually to match the behavior on Windows.
#if defined(OS_MACOSX) || defined(OS_LINUX)
  if (owner_window())
    owner_window()->Focus(true);
#endif
  web_contents()->Focus();
}