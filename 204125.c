print_resources_callback (WinLibrary *fi, WinResource *wr,
                          WinResource *type_wr, WinResource *name_wr,
                          WinResource *lang_wr)
{
    const char *type, *offset;
    int32_t id;
    size_t size;

    /* get named resource type if possible */
    type = NULL;
    if (parse_int32(type_wr->id, &id))
        type = res_type_id_to_string(id);

    /* get offset and size info on resource */
    offset = get_resource_entry(fi, wr, &size);
    if (offset == NULL)
        return;

    printf(_("--type=%s --name=%s%s%s [%s%s%soffset=0x%x size=%zu]\n"),
      get_resource_id_quoted(type_wr),
      get_resource_id_quoted(name_wr),
      (lang_wr->id[0] != '\0' ? _(" --language=") : ""),
      get_resource_id_quoted(lang_wr),
      (type != NULL ? "type=" : ""),
      (type != NULL ? type : ""),
      (type != NULL ? " " : ""),
      (uint32_t) (offset - fi->memory), size);
}