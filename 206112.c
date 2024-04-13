event_basic_db_equal(LEX_STRING db, Event_basic *et)
{
  return !sortcmp_lex_string(et->dbname, db, system_charset_info);
}