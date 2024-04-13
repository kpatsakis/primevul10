void do_crypt(const char *s, char *d, int size) {
   strlcpy(d, sha256_crypt(s, "$5$") + 4, size);
}