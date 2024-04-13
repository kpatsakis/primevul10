entity_doctype determine_doctype(int flags) {
  int mask = flags & ENT_HTML_DOC_TYPE_MASK;
  switch (mask) {
    case ENT_HTML_DOC_HTML401: return entity_doctype::html401;
    case ENT_HTML_DOC_XML1: return entity_doctype::xml1;
    case ENT_HTML_DOC_XHTML: return entity_doctype::xhtml;
    case ENT_HTML_DOC_HTML5: return entity_doctype::html5;
  }
  not_reached();
}