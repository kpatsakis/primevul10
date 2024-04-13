size_t Frame::getName(char *result_buf, size_t result_len) {
  if (result_len <= 1) {
    return 0; // Insufficient result_bug. Bail!
  }

  // Add '@recurse_level' if required
  // NOTE: Dont use snprintf's return val as it is compiler dependent
  if (m_recursion) {
    snprintf(result_buf, result_len, "%s@%d", m_name, m_recursion);
  } else {
    snprintf(result_buf, result_len, "%s", m_name);
  }

  // Force null-termination at MAX
  result_buf[result_len - 1] = 0;
  return strlen(result_buf);
}