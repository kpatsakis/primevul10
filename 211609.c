const HtmlBasicEntity* get_basic_table(bool all, entity_doctype doctype) {
  if (doctype == entity_doctype::xhtml) {
    return all ? basic_entities_noapos : basic_entities_apos;
  }

  if (doctype == entity_doctype::html401) {
    return basic_entities_noapos;
  }

  return basic_entities_apos;
}