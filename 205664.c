bool Item_trigger_field::fix_fields(THD *thd, Item **items)
{
  /*
    Since trigger is object tightly associated with TABLE object most
    of its set up can be performed during trigger loading i.e. trigger
    parsing! So we have little to do in fix_fields. :)
  */

  DBUG_ASSERT(fixed == 0);

  /* Set field. */

  if (field_idx != (uint)-1)
  {
#ifndef NO_EMBEDDED_ACCESS_CHECKS
    /*
      Check access privileges for the subject table. We check privileges only
      in runtime.
    */

    if (table_grants)
    {
      table_grants->want_privilege= want_privilege;

      if (check_grant_column(thd, table_grants, triggers->trigger_table->s->db.str,
                             triggers->trigger_table->s->table_name.str, field_name,
                             strlen(field_name), thd->security_ctx))
        return TRUE;
    }
#endif // NO_EMBEDDED_ACCESS_CHECKS

    field= (row_version == OLD_ROW) ? triggers->old_field[field_idx] :
                                      triggers->new_field[field_idx];
    set_field(field);
    fixed= 1;
    return FALSE;
  }

  my_error(ER_BAD_FIELD_ERROR, MYF(0), field_name,
           (row_version == NEW_ROW) ? "NEW" : "OLD");
  return TRUE;
}