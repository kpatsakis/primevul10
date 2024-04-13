static void exif_iif_add_buffer(image_info_type *image_info,
                                int section_index, char *name,
                                int length, char *value) {
  image_info_data *info_data;
  image_info_data *list;

  if (value) {
    size_t realloc_size = (image_info->info_list[section_index].count+1) *
                          sizeof(image_info_data);
    list = (image_info_data *)
      IM_REALLOC(image_info->info_list[section_index].list, realloc_size);
    CHECK_ALLOC(list, realloc_size);
    image_info->info_list[section_index].list = list;
    info_data = &image_info->info_list[section_index].
                list[image_info->info_list[section_index].count];
    memset(info_data, 0, sizeof(image_info_data));
    info_data->tag = (unsigned short)TAG_NONE;
    info_data->format = TAG_FMT_UNDEFINED;
    info_data->length = length;
    PHP_STRDUP(info_data->name, name);
    // if (PG(magic_quotes_runtime)) {
    //   info_data->value.s = php_addslashes(value, length, &length, 0);
    //   info_data->length = length;
    // } else {
    info_data->value.s = (char *)IM_MALLOC(length + 1);
    if (!info_data->value.s) info_data->length = 0;
    CHECK_ALLOC(info_data->value.s, length + 1);
    memcpy(info_data->value.s, value, length);
    info_data->value.s[length] = 0;
    image_info->sections_found |= 1<<section_index;
    image_info->info_list[section_index].count++;
  }
}