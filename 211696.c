char *php_gcvt(double value, int ndigit, char dec_point,
                      char exponent, char *buf) {
  char *digits, *dst, *src;
  int i, decpt, sign;

  digits = zend_dtoa(value, 2, ndigit, &decpt, &sign, nullptr);
  if (decpt == 9999) {
    /*
     * Infinity or NaN, convert to inf or nan with sign.
     * We assume the buffer is at least ndigit long.
     */
    snprintf(buf, ndigit + 1, "%s%s", (sign && *digits == 'I') ? "-" : "",
             *digits == 'I' ? "INF" : "NAN");
    zend_freedtoa(digits);
    return (buf);
  }

  dst = buf;
  if (sign) {
    *dst++ = '-';
  }

  if ((decpt >= 0 && decpt > ndigit) || decpt < -3) { /* use E-style */
    /* exponential format (e.g. 1.2345e+13) */
    if (--decpt < 0) {
      sign = 1;
      decpt = -decpt;
    } else {
      sign = 0;
    }
    src = digits;
    *dst++ = *src++;
    *dst++ = dec_point;
    if (*src == '\0') {
      *dst++ = '0';
    } else {
      do {
        *dst++ = *src++;
      } while (*src != '\0');
    }
    *dst++ = exponent;
    if (sign) {
      *dst++ = '-';
    } else {
      *dst++ = '+';
    }
    if (decpt < 10) {
      *dst++ = '0' + decpt;
      *dst = '\0';
    } else {
      /* XXX - optimize */
      for (sign = decpt, i = 0; (sign /= 10) != 0; i++)
        continue;
      dst[i + 1] = '\0';
      while (decpt != 0) {
        dst[i--] = '0' + decpt % 10;
        decpt /= 10;
      }
    }
  } else if (decpt < 0) {
    /* standard format 0. */
    *dst++ = '0';   /* zero before decimal point */
    *dst++ = dec_point;
    do {
      *dst++ = '0';
    } while (++decpt < 0);
    src = digits;
    while (*src != '\0') {
      *dst++ = *src++;
    }
    *dst = '\0';
  } else {
    /* standard format */
    for (i = 0, src = digits; i < decpt; i++) {
      if (*src != '\0') {
        *dst++ = *src++;
      } else {
        *dst++ = '0';
      }
    }
    if (*src != '\0') {
      if (src == digits) {
        *dst++ = '0';   /* zero before decimal point */
      }
      *dst++ = dec_point;
      for (i = decpt; digits[i] != '\0'; i++) {
        *dst++ = digits[i];
      }
    }
    *dst = '\0';
  }
  zend_freedtoa(digits);
  return (buf);
}