cdio_is_device_quiet_generic(const char *source_name)
{
  struct stat buf;
  if (0 != stat(source_name, &buf)) {
    return false;
  }
  return (S_ISBLK(buf.st_mode) || S_ISCHR(buf.st_mode));
}