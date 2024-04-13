get_hash_secret_salt(XML_Parser parser) {
  if (parser->m_parentParser != NULL)
    return get_hash_secret_salt(parser->m_parentParser);
  return parser->m_hash_secret_salt;
}