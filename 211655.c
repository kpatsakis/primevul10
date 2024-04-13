char *string_crypt(const char *key, const char *salt) {
  assertx(key);
  assertx(salt);

  char random_salt[12];
  if (!*salt) {
    memcpy(random_salt,"$1$",3);
    ito64(random_salt+3,rand(),8);
    random_salt[11] = '\0';
    return string_crypt(key, random_salt);
  }

  auto const saltLen = strlen(salt);
  if ((saltLen > sizeof("$2X$00$")) &&
    (salt[0] == '$') &&
    (salt[1] == '2') &&
    (salt[2] >= 'a') && (salt[2] <= 'z') &&
    (salt[3] == '$') &&
    (salt[4] >= '0') && (salt[4] <= '3') &&
    (salt[5] >= '0') && (salt[5] <= '9') &&
    (salt[6] == '$')) {
    // Bundled blowfish crypt()
    char output[61];

    static constexpr size_t maxSaltLength = 123;
    char paddedSalt[maxSaltLength + 1];
    paddedSalt[0] = paddedSalt[maxSaltLength] = '\0';

    memset(&paddedSalt[1], '$', maxSaltLength - 1);
    memcpy(paddedSalt, salt, std::min(maxSaltLength, saltLen));
    paddedSalt[std::min(maxSaltLength, saltLen)] = '\0';

    if (php_crypt_blowfish_rn(key, paddedSalt, output, sizeof(output))) {
      return strdup(output);
    }

  } else {
    // System crypt() function
#ifdef USE_PHP_CRYPT_R
    return php_crypt_r(key, salt);
#else
    static Mutex mutex;
    Lock lock(mutex);
    char *crypt_res = crypt(key,salt);

    if (crypt_res) {
      return strdup(crypt_res);
    }
#endif
  }

  return ((salt[0] == '*') && (salt[1] == '0'))
                  ? strdup("*1") : strdup("*0");
}