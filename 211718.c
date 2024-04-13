static int php_write(void *buf, uint32_t size) {
  g_context->write((const char *)buf, size);
  return size;
}