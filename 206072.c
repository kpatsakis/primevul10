CHARSET_INFO *Item::default_charset()
{
  return current_thd->variables.collation_connection;
}