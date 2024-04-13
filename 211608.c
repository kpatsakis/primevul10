static void exif_iif_add_str(image_info_type *image_info,
                             int section_index, char *name, char *value) {
  image_info_data  *info_data;
  image_info_data  *list;

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
    info_data->format = TAG_FMT_STRING;
    info_data->length = 1;
    PHP_STRDUP(info_data->name, name);
    // TODO
    // if (PG(magic_quotes_runtime)) {
    //   info_data->value.s = php_addslashes(value, strlen(value), nullptr, 0);
    // } else {
    PHP_STRDUP(info_data->value.s, value);
    image_info->sections_found |= 1<<section_index;
    image_info->info_list[section_index].count++;
  }
}