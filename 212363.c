static inline uint16_t bswap16(const uint16_t u16)
{
#ifndef WORDS_BIGENDIAN
#if defined (__GNUC__) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 8)))
    return __builtin_bswap16(u16);
#elif defined (_MSC_VER)
    return _byteswap_ushort(u16);
#else
    return (u16 << 8) | (u16 >> 8);
#endif
#else
    return u16;
#endif
}