int get_fileformat_force(buf_T *buf, exarg_T *eap)
{
  int c;

  if (eap != NULL && eap->force_ff != 0) {
    c = eap->cmd[eap->force_ff];
  } else {
    if ((eap != NULL && eap->force_bin != 0)
        ? (eap->force_bin == FORCE_BIN) : buf->b_p_bin) {
      return EOL_UNIX;
    }
    c = *buf->b_p_ff;
  }
  if (c == 'u') {
    return EOL_UNIX;
  }
  if (c == 'm') {
    return EOL_MAC;
  }
  return EOL_DOS;
}