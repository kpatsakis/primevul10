static struct gfxinfo *php_handle_tiff(const req::ptr<File>& stream,
                                       int motorola_intel) {
  struct gfxinfo *result = nullptr;
  int i, num_entries;
  unsigned char *dir_entry;
  size_t dir_size, entry_value, width=0, height=0, ifd_addr;
  int entry_tag , entry_type;

  String ifd_ptr = stream->read(4);
  if (ifd_ptr.length() != 4) return nullptr;
  ifd_addr = php_ifd_get32u((void*)ifd_ptr.c_str(), motorola_intel);
  if (!stream->seek(ifd_addr-8, SEEK_CUR)) return nullptr;
  String ifd_data = stream->read(2);
  if (ifd_data.length() != 2) return nullptr;
  num_entries = php_ifd_get16u((void*)ifd_data.c_str(), motorola_intel);
  dir_size = 2/*num dir entries*/ +12/*length of entry*/*
             num_entries +
             4/* offset to next ifd (points to thumbnail or NULL)*/;
  String ifd_data2 = stream->read(dir_size-2);
  if ((size_t)ifd_data2.length() != dir_size-2) return nullptr;
  ifd_data += ifd_data2;
  /* now we have the directory we can look how long it should be */
  for(i=0;i<num_entries;i++) {
    dir_entry = (unsigned char*)ifd_data.c_str()+2+i*12;
    entry_tag = php_ifd_get16u(dir_entry+0, motorola_intel);
    entry_type = php_ifd_get16u(dir_entry+2, motorola_intel);
    switch(entry_type) {
      case TAG_FMT_BYTE:
      case TAG_FMT_SBYTE:
        entry_value = (size_t)(dir_entry[8]);
        break;
      case TAG_FMT_USHORT:
        entry_value = php_ifd_get16u(dir_entry+8, motorola_intel);
        break;
      case TAG_FMT_SSHORT:
        entry_value = php_ifd_get16s(dir_entry+8, motorola_intel);
        break;
      case TAG_FMT_ULONG:
        entry_value = php_ifd_get32u(dir_entry+8, motorola_intel);
        break;
      case TAG_FMT_SLONG:
        entry_value = php_ifd_get32s(dir_entry+8, motorola_intel);
        break;
      default:
        continue;
    }
    switch(entry_tag) {
      case TAG_IMAGEWIDTH:
      case TAG_COMP_IMAGEWIDTH:
        width = entry_value;
        break;
      case TAG_IMAGEHEIGHT:
      case TAG_COMP_IMAGEHEIGHT:
        height = entry_value;
        break;
    }
  }
  if ( width && height) {
    /* not the same when in for-loop */
    result = (struct gfxinfo *)IM_CALLOC(1, sizeof(struct gfxinfo));
    CHECK_ALLOC_R(result, sizeof (struct gfxinfo), nullptr);
    result->height = height;
    result->width = width;
    result->bits = 0;
    result->channels = 0;
    return result;
  }
  return nullptr;
}