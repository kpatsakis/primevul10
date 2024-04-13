static bool mark_as_dependent(THD *thd, SELECT_LEX *last, SELECT_LEX *current,
                              Item_ident *resolved_item,
                              Item_ident *mark_item)
{
  DBUG_ENTER("mark_as_dependent");

  /* store pointer on SELECT_LEX from which item is dependent */
  if (mark_item && mark_item->can_be_depended)
  {
    DBUG_PRINT("info", ("mark_item: %p  lex: %p", mark_item, last));
    mark_item->depended_from= last;
  }
  if (current->mark_as_dependent(thd, last,
                                 /** resolved_item psergey-thu **/ mark_item))
    DBUG_RETURN(TRUE);
  if (thd->lex->describe & DESCRIBE_EXTENDED)
  {
    const char *db_name= (resolved_item->db_name ?
                          resolved_item->db_name : "");
    const char *table_name= (resolved_item->table_name ?
                             resolved_item->table_name : "");
    push_warning_printf(thd, MYSQL_ERROR::WARN_LEVEL_NOTE,
		 ER_WARN_FIELD_RESOLVED, ER(ER_WARN_FIELD_RESOLVED),
                 db_name, (db_name[0] ? "." : ""),
                 table_name, (table_name [0] ? "." : ""),
                 resolved_item->field_name,
                 current->select_number, last->select_number);
  }
  DBUG_RETURN(FALSE);
}