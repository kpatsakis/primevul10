static void exif_iif_free(image_info_type *image_info, int section_index) {
  int  i;
  void *f; /* faster */

  if (image_info->info_list[section_index].count) {
    for (i=0; i < image_info->info_list[section_index].count; i++) {
      if ((f=image_info->info_list[section_index].list[i].name) != nullptr) {
        IM_FREE(f);
      }
      switch(image_info->info_list[section_index].list[i].format) {
      case TAG_FMT_SBYTE:
      case TAG_FMT_BYTE:
        /* in contrast to strings bytes do not need to allocate
           buffer for nullptr if length==0 */
        if (image_info->info_list[section_index].list[i].length<1)
          break;
      default:
      case TAG_FMT_UNDEFINED:
      case TAG_FMT_STRING:
        if ((f=image_info->info_list[section_index].list[i].value.s)
             != nullptr) {
          IM_FREE(f);
        }
        break;

      case TAG_FMT_USHORT:
      case TAG_FMT_ULONG:
      case TAG_FMT_URATIONAL:
      case TAG_FMT_SSHORT:
      case TAG_FMT_SLONG:
      case TAG_FMT_SRATIONAL:
      case TAG_FMT_SINGLE:
      case TAG_FMT_DOUBLE:
        /* nothing to do here */
        if (image_info->info_list[section_index].list[i].length > 1) {
          if ((f=image_info->info_list[section_index].list[i].value.list)
              != nullptr) {
            IM_FREE(f);
          }
        }
        break;
      }
    }
  }
  if (image_info->info_list[section_index].list) {
    IM_FREE(image_info->info_list[section_index].list);
  }
}