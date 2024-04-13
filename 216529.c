bool WebContents::IsOffScreen() const {
#if BUILDFLAG(ENABLE_OSR)
  return type_ == Type::OFF_SCREEN;
#else
  return false;
#endif
}