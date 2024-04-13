static inline uint16_t strtr_hash(const char *str, int len) {
    uint16_t  res = 0;
    for (int i = 0; i < len; i++) {
        res = res * 33 + (unsigned char)str[i];
    }

    return res;
}