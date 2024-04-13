base::Optional<base::TimeDelta> GetCursorBlinkInterval() {
#if defined(OS_MACOSX)
  base::TimeDelta interval;
  if (ui::TextInsertionCaretBlinkPeriod(&interval))
    return interval;
#elif defined(OS_LINUX)
  if (auto* linux_ui = views::LinuxUI::instance())
    return linux_ui->GetCursorBlinkInterval();
#elif defined(OS_WIN)
  const auto system_msec = ::GetCaretBlinkTime();
  if (system_msec != 0) {
    return (system_msec == INFINITE)
               ? base::TimeDelta()
               : base::TimeDelta::FromMilliseconds(system_msec);
  }
#endif
  return base::nullopt;
}