cdio_is_device_generic(const char *source_name)
{
  struct stat buf;
  if (0 != stat(source_name, &buf)) {
    cdio_warn ("Can't get file status for %s:\n%s", source_name,
                strerror(errno));
    return false;
  }
  return (S_ISBLK(buf.st_mode) || S_ISCHR(buf.st_mode));
}