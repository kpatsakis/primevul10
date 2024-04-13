long my_timezone() {
#if defined(OS_MACOSX) || defined(__FreeBSD__) || defined(__OpenBSD__)
   time_t tp;
   time(&tp);
   return -localtime(&tp)->tm_gmtoff;
#elif defined(OS_WINNT)
   return _timezone;
#else
   return timezone;
#endif
}