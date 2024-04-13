static void exif_iif_add_value(image_info_type *image_info, int section_index,
                               char *name, int tag, int format, int length,
                               void* value, int motorola_intel) {
  size_t idex;
  void *vptr;
  image_info_value *info_value;
  image_info_data  *info_data;
  image_info_data  *list;

  if (length < 0) {
    return;
  }

  size_t realloc_size = (image_info->info_list[section_index].count+1) *
                        sizeof(image_info_data);
  list = (image_info_data*)
    IM_REALLOC(image_info->info_list[section_index].list, realloc_size);
  CHECK_ALLOC(list, realloc_size);
  image_info->info_list[section_index].list = list;

  info_data  = &image_info->info_list[section_index].
                list[image_info->info_list[section_index].count];
  memset(info_data, 0, sizeof(image_info_data));
  info_data->tag = tag;
  info_data->format = format;
  info_data->length = length;
  PHP_STRDUP(info_data->name, name);
  info_value = &info_data->value;

  switch (format) {
  case TAG_FMT_STRING:
    if (value) {
      length = php_strnlen((char*)value, length);
      // TODO
      // if (PG(magic_quotes_runtime)) {
      //   info_value->s = php_addslashes(value, length, &length, 0);
      // } else {
      PHP_STRNDUP(info_value->s, (const char *)value, length);
      // }
      info_data->length = (info_value->s ? length : 0);
    } else {
      info_data->length = 0;
      PHP_STRDUP(info_value->s, "");
    }
    break;

  default:
    /* Standard says more types possible but skip them...
     * but allow users to handle data if they know how to
     * So not return but use type UNDEFINED
     * return;
     */
    info_data->tag = TAG_FMT_UNDEFINED;/* otherwise not freed from memory */
  case TAG_FMT_SBYTE:
  case TAG_FMT_BYTE:
    /* in contrast to strings bytes do not need to allocate buffer for
       nullptr if length==0 */
    if (!length)
      break;
  case TAG_FMT_UNDEFINED:
    if (value) {
      /* do not recompute length here */
      // TODO
      // if (PG(magic_quotes_runtime)) {
      //   info_value->s = php_addslashes(value, length, &length, 0);
      // } else {
      PHP_STRNDUP(info_value->s, (const char *)value, length);
      // }
      info_data->length = (info_value->s ? length : 0);
    } else {
      info_data->length = 0;
      PHP_STRDUP(info_value->s, "");
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
    if (length==0) {
      break;
    } else if (length>1) {
      info_value->list =
        (image_info_value*)IM_CALLOC(length, sizeof(image_info_value));
      CHECK_ALLOC(info_value->list, sizeof(image_info_value));
    } else {
      info_value = &info_data->value;
    }
    for (idex=0,vptr=value; idex<(size_t)length;
         idex++,vptr=(char *) vptr + get_php_tiff_bytes_per_format(format)) {
      if (length>1) {
        info_value = &info_data->value.list[idex];
      }
      switch (format) {
      case TAG_FMT_USHORT:
        info_value->u = php_ifd_get16u(vptr, motorola_intel);
        break;

      case TAG_FMT_ULONG:
        info_value->u = php_ifd_get32u(vptr, motorola_intel);
        break;

      case TAG_FMT_URATIONAL:
        info_value->ur.num = php_ifd_get32u(vptr, motorola_intel);
        info_value->ur.den = php_ifd_get32u(4+(char *)vptr, motorola_intel);
        break;

      case TAG_FMT_SSHORT:
        info_value->i = php_ifd_get16s(vptr, motorola_intel);
        break;

      case TAG_FMT_SLONG:
        info_value->i = php_ifd_get32s(vptr, motorola_intel);
        break;

      case TAG_FMT_SRATIONAL:
        info_value->sr.num = php_ifd_get32u(vptr, motorola_intel);
        info_value->sr.den = php_ifd_get32u(4+(char *)vptr, motorola_intel);
        break;

      case TAG_FMT_SINGLE:
        info_value->f = *(float *)value;

      case TAG_FMT_DOUBLE:
        info_value->d = *(double *)value;
        break;
      }
    }
  }
  image_info->sections_found |= 1<<section_index;
  image_info->info_list[section_index].count++;
}