size_t Frame::getStack(int level, char *result_buf, size_t result_len) {
  // End recursion if we dont need deeper levels or
  // we dont have any deeper levels
  if (!m_parent || level <= 1) {
    return getName(result_buf, result_len);
  }

  // Take care of all ancestors first
  size_t len = m_parent->getStack(level - 1, result_buf, result_len);
  if (result_len < (len + HP_STACK_DELIM_LEN)) {
    return len; // Insufficient result_buf. Bail out!
  }

  // Add delimiter only if entry had ancestors
  if (len) {
    strncat(result_buf + len, HP_STACK_DELIM, result_len - len);
    len += HP_STACK_DELIM_LEN;
  }

  // Append the current function name
  return len + getName(result_buf + len, result_len - len);
}