char * php_conv_fp(register char format, register double num,
                   bool add_dp, int precision, char dec_point,
                   int *is_negative, char *buf, int *len) {
  register char *s = buf;
  register char *p, *p_orig;
  int decimal_point;

  if (precision >= NDIG - 1) {
    precision = NDIG - 2;
  }

  if (format == 'F') {
    p_orig = p = php_fcvt(num, precision, &decimal_point, is_negative);
  } else { // either e or E format
    p_orig = p = php_ecvt(num, precision + 1, &decimal_point, is_negative);
  }

  // Check for Infinity and NaN
  if (isalpha((int)*p)) {
    *len = strlen(p);
    memcpy(buf, p, *len + 1);
    *is_negative = 0;
    free(p_orig);
    return (buf);
  }
  if (format == 'F') {
    if (decimal_point <= 0) {
      if (num != 0 || precision > 0) {
        *s++ = '0';
        if (precision > 0) {
          *s++ = dec_point;
          while (decimal_point++ < 0) {
            *s++ = '0';
          }
        } else if (add_dp) {
          *s++ = dec_point;
        }
      }
    } else {
      int addz = decimal_point >= NDIG ? decimal_point - NDIG + 1 : 0;
      decimal_point -= addz;
      while (decimal_point-- > 0) {
        *s++ = *p++;
      }
      while (addz-- > 0) {
        *s++ = '0';
      }
      if (precision > 0 || add_dp) {
        *s++ = dec_point;
      }
    }
  } else {
    *s++ = *p++;
    if (precision > 0 || add_dp) {
      *s++ = '.';
    }
  }

  // copy the rest of p, the NUL is NOT copied
  while (*p) {
    *s++ = *p++;
  }

  if (format != 'F') {
    char temp[EXPONENT_LENGTH]; // for exponent conversion
    int t_len;
    int exponent_is_negative;

    *s++ = format; // either e or E
    decimal_point--;
    if (decimal_point != 0) {
      p = ap_php_conv_10((int64_t) decimal_point, false,
                         &exponent_is_negative, &temp[EXPONENT_LENGTH],
                         &t_len);
      *s++ = exponent_is_negative ? '-' : '+';

      // Make sure the exponent has at least 2 digits
      while (t_len--) {
        *s++ = *p++;
      }
    } else {
      *s++ = '+';
      *s++ = '0';
    }
  }
  *len = s - buf;
  free(p_orig);
  return (buf);
}