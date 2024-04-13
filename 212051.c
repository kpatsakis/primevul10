Array HHVM_FUNCTION(get_html_translation_table,
                    int table /* = 0 */,
                    int flags /* = k_ENT_HTML_QUOTE_DOUBLE */,
                    const String& encoding /* = "UTF-8" */) {
  using namespace entity_charset_enum;
  auto charset = determine_charset(encoding.data());
  if (charset == cs_unknown) {
    charset = cs_utf_8;
    if (!encoding.empty()) {
      raise_warning("get_html_translation_table(): charset `%s' not supported"
                    ", assuming utf-8", encoding.data());
    }
  }
  auto doctype = determine_doctype(flags);

  bool all = (table == k_HTML_ENTITIES);

  Array ret = Array::CreateDArray();
  switch (table) {
  case k_HTML_ENTITIES: {
    if (charset == cs_utf_8) {
      auto entity_map = get_doctype_entity_table(doctype);
      for (const auto& item : *entity_map) {
        auto key = encode_as_utf8(item.first);

        char buffer[32];
        snprintf(buffer, sizeof(buffer), "&%s;", item.second.c_str());
        ret.set(key, String(buffer, CopyString));
      }
      if (doctype == entity_doctype::html5) {
        for (const auto& item: *get_multicode_table()) {
          auto codes = item.first;
          String key = encode_as_utf8(codes.first);
          key += encode_as_utf8(codes.second);

          char buffer[32];
          snprintf(buffer, sizeof(buffer), "&%s", item.second.c_str());
          ret.set(key, String(buffer, CopyString));
        }
      }
    } else {
      const auto& entity_map = get_doctype_entity_table(doctype);
      auto charset_table = get_charset_table(charset);
      for (const auto& item : *charset_table) {
        const auto iter = entity_map->find(item.second);
        if (iter != entity_map->end()) {
          char buffer[16];
          snprintf(buffer, sizeof(buffer), "&%s;", iter->second.c_str());

          auto key = String::FromChar(item.first);
          ret.set(key, String(buffer, CopyString));
        }
      }
    }
    /* fall thru */
  }
  case k_HTML_SPECIALCHARS:
    const auto& basic_table = get_basic_table(all, doctype);
    for (int j = 0; basic_table[j].charcode != 0; j++) {
      const auto& item = basic_table[j];
      if (item.flags && (flags & item.flags) == 0)
        continue;

      ret.set(String::FromChar(item.charcode), item.entity);
    }
    ret.set(s_amp, s_ampsemi);
    break;
  }
  if (ret.empty()) {
    return Array();
  }

  return ret;
}