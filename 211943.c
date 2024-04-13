char* php_crypt_r(const char* key, const char* salt) {
  if (salt[0] == '$' && salt[1] == '1' && salt[2] == '$') {
    char output[MD5_HASH_MAX_LEN], *out;

    out = php_md5_crypt_r(key, salt, output);
    return out ? strdup(out) : nullptr;
  } else if (salt[0] == '$' && salt[1] == '6' && salt[2] == '$') {
    char output[PHP_MAX_SALT_LEN + 1];

    char* crypt_res = php_sha512_crypt_r(key, salt, output, PHP_MAX_SALT_LEN);
    if (!crypt_res) {
      SECURE_ZERO(output, PHP_MAX_SALT_LEN + 1);
      return nullptr;
    } else {
      char* result = strdup(output);
      SECURE_ZERO(output, PHP_MAX_SALT_LEN + 1);
      return result;
    }
  } else if (salt[0] == '$' && salt[1] == '5' && salt[2] == '$') {
    char output[PHP_MAX_SALT_LEN + 1];

    char* crypt_res = php_sha256_crypt_r(key, salt, output, PHP_MAX_SALT_LEN);
    if (!crypt_res) {
      SECURE_ZERO(output, PHP_MAX_SALT_LEN + 1);
      return nullptr;
    } else {
      char* result = strdup(output);
      SECURE_ZERO(output, PHP_MAX_SALT_LEN + 1);
      return result;
    }
  } else if (
    salt[0] == '$' &&
    salt[1] == '2' &&
    salt[3] == '$') {
    char output[PHP_MAX_SALT_LEN + 1];

    memset(output, 0, PHP_MAX_SALT_LEN + 1);

    char* crypt_res = php_crypt_blowfish_rn(key, salt, output, sizeof(output));
    if (!crypt_res) {
      SECURE_ZERO(output, PHP_MAX_SALT_LEN + 1);
      return nullptr;
    } else {
      char* result = strdup(output);
      SECURE_ZERO(output, PHP_MAX_SALT_LEN + 1);
      return result;
    }
  } else if (salt[0] == '*' && (salt[1] == '0' || salt[1] == '1')) {
    return nullptr;
  } else {
    struct php_crypt_extended_data buffer;
    /* DES Fallback */
    memset(&buffer, 0, sizeof(buffer));
    _crypt_extended_init_r();

    char* crypt_res = _crypt_extended_r(key, salt, &buffer);
    if (!crypt_res || (salt[0] == '*' && salt[1] == '0')) {
      return nullptr;
    } else {
      return strdup(crypt_res);
    }
  }
}