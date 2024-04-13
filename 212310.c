static void layoutEthereumData(const uint8_t *data, uint32_t len,
                               uint32_t total_len, char *out_str,
                               size_t out_str_len) {
  char hexdata[3][17];
  char summary[20];
  uint32_t printed = 0;
  for (int i = 0; i < 3; i++) {
    uint32_t linelen = len - printed;
    if (linelen > 8) {
      linelen = 8;
    }
    data2hex(data, linelen, hexdata[i]);
    data += linelen;
    printed += linelen;
  }

  strcpy(summary, "...          bytes");
  char *p = summary + 11;
  uint32_t number = total_len;
  while (number > 0) {
    *p-- = '0' + number % 10;
    number = number / 10;
  }
  char *summarystart = summary;
  if (total_len == printed) summarystart = summary + 4;

  if ((uint32_t)snprintf(out_str, out_str_len, "%s%s\n%s%s", hexdata[0],
                         hexdata[1], hexdata[2], summarystart) >= out_str_len) {
    /*error detected.  Clear the buffer */
    memset(out_str, 0, out_str_len);
  }
}