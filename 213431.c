inline void FastMemcpy(void* dst, const void* src, size_t size) {
  // clang-format off
  switch (size) {
    // Most compilers will generate inline code for fixed sizes,
    // which is significantly faster for small copies.
    case  1: memcpy(dst, src, 1); break;
    case  2: memcpy(dst, src, 2); break;
    case  3: memcpy(dst, src, 3); break;
    case  4: memcpy(dst, src, 4); break;
    case  5: memcpy(dst, src, 5); break;
    case  6: memcpy(dst, src, 6); break;
    case  7: memcpy(dst, src, 7); break;
    case  8: memcpy(dst, src, 8); break;
    case  9: memcpy(dst, src, 9); break;
    case 10: memcpy(dst, src, 10); break;
    case 11: memcpy(dst, src, 11); break;
    case 12: memcpy(dst, src, 12); break;
    case 13: memcpy(dst, src, 13); break;
    case 14: memcpy(dst, src, 14); break;
    case 15: memcpy(dst, src, 15); break;
    case 16: memcpy(dst, src, 16); break;
#if defined(PLATFORM_GOOGLE) || defined(PLATFORM_POSIX) && \
    !defined(IS_MOBILE_PLATFORM)
    // On Linux, memmove appears to be faster than memcpy for
    // large sizes, strangely enough.
    default: memmove(dst, src, size); break;
#else
    default: memcpy(dst, src, size); break;
#endif
  }
  // clang-format on
}