bool Item_field::fix_fields(THD *thd, Item **reference)
{
  DBUG_ASSERT(fixed == 0);
  Field *from_field= (Field *)not_found_field;
  bool outer_fixed= false;

  if (!field)					// If field is not checked
  {
    TABLE_LIST *table_list;
    /*
      In case of view, find_field_in_tables() write pointer to view field
      expression to 'reference', i.e. it substitute that expression instead
      of this Item_field
    */
    if ((from_field= find_field_in_tables(thd, this,
                                          context->first_name_resolution_table,
                                          context->last_name_resolution_table,
                                          reference,
                                          thd->lex->use_only_table_context ?
                                            REPORT_ALL_ERRORS : 
                                            IGNORE_EXCEPT_NON_UNIQUE,
                                          !any_privileges,
                                          TRUE)) ==
	not_found_field)
    {
      int ret;
      /* Look up in current select's item_list to find aliased fields */
      if (thd->lex->current_select->is_item_list_lookup)
      {
        uint counter;
        enum_resolution_type resolution;
        Item** res= find_item_in_list(this, thd->lex->current_select->item_list,
                                      &counter, REPORT_EXCEPT_NOT_FOUND,
                                      &resolution);
        if (!res)
          return 1;
        if (resolution == RESOLVED_AGAINST_ALIAS)
          alias_name_used= TRUE;
        if (res != (Item **)not_found_item)
        {
          if ((*res)->type() == Item::FIELD_ITEM)
          {
            /*
              It's an Item_field referencing another Item_field in the select
              list.
              Use the field from the Item_field in the select list and leave
              the Item_field instance in place.
            */

            Field *new_field= (*((Item_field**)res))->field;

            if (new_field == NULL)
            {
              /* The column to which we link isn't valid. */
              my_error(ER_BAD_FIELD_ERROR, MYF(0), (*res)->name, 
                       current_thd->where);
              return(1);
            }

            set_field(new_field);
            return 0;
          }
          else
          {
            /*
              It's not an Item_field in the select list so we must make a new
              Item_ref to point to the Item in the select list and replace the
              Item_field created by the parser with the new Item_ref.
            */
            Item_ref *rf= new Item_ref(context, db_name,table_name,field_name);
            if (!rf)
              return 1;
            bool ret= rf->fix_fields(thd, (Item **) &rf) || rf->check_cols(1);
            if (ret)
              return TRUE;
           
            SELECT_LEX *select= thd->lex->current_select;
            thd->change_item_tree(reference,
                                  select->parsing_place == IN_GROUP_BY && 
				  alias_name_used  ?  *rf->ref : rf);

            return FALSE;
          }
        }
      }
      if ((ret= fix_outer_field(thd, &from_field, reference)) < 0)
        goto error;
      outer_fixed= TRUE;
      if (!ret)
        goto mark_non_agg_field;
    }
    else if (!from_field)
      goto error;

    table_list= (cached_table ? cached_table :
                 from_field != view_ref_found ?
                 from_field->table->pos_in_table_list : 0);
    if (!outer_fixed && table_list && table_list->select_lex &&
        context->select_lex &&
        table_list->select_lex != context->select_lex &&
        !context->select_lex->is_merged_child_of(table_list->select_lex) &&
        is_outer_table(table_list, context->select_lex))
    {
      int ret;
      if ((ret= fix_outer_field(thd, &from_field, reference)) < 0)
        goto error;
      outer_fixed= 1;
      if (!ret)
        goto mark_non_agg_field;
    }

    if (thd->lex->in_sum_func &&
        thd->lex->in_sum_func->nest_level == 
        thd->lex->current_select->nest_level)
      set_if_bigger(thd->lex->in_sum_func->max_arg_level,
                    thd->lex->current_select->nest_level);
    /*
      if it is not expression from merged VIEW we will set this field.

      We can leave expression substituted from view for next PS/SP rexecution
      (i.e. do not register this substitution for reverting on cleanup()
      (register_item_tree_changing())), because this subtree will be
      fix_field'ed during setup_tables()->setup_underlying() (i.e. before
      all other expressions of query, and references on tables which do
      not present in query will not make problems.

      Also we suppose that view can't be changed during PS/SP life.
    */
    if (from_field == view_ref_found)
      return FALSE;

    set_field(from_field);
  }
  else if (thd->mark_used_columns != MARK_COLUMNS_NONE)
  {
    TABLE *table= field->table;
    MY_BITMAP *current_bitmap, *other_bitmap;
    if (thd->mark_used_columns == MARK_COLUMNS_READ)
    {
      current_bitmap= table->read_set;
      other_bitmap=   table->write_set;
    }
    else
    {
      current_bitmap= table->write_set;
      other_bitmap=   table->read_set;
    }
    if (!bitmap_fast_test_and_set(current_bitmap, field->field_index))
    {
      if (!bitmap_is_set(other_bitmap, field->field_index))
      {
        /* First usage of column */
        table->used_fields++;                     // Used to optimize loops
        /* purecov: begin inspected */
        table->covering_keys.intersect(field->part_of_key);
        /* purecov: end */
      }
    }
  }
#ifndef NO_EMBEDDED_ACCESS_CHECKS
  if (any_privileges)
  {
    char *db, *tab;
    db=  field->table->s->db.str;
    tab= field->table->s->table_name.str;
    if (!(have_privileges= (get_column_grant(thd, &field->table->grant,
                                             db, tab, field_name) &
                            VIEW_ANY_ACL)))
    {
      my_error(ER_COLUMNACCESS_DENIED_ERROR, MYF(0),
               "ANY", thd->security_ctx->priv_user,
               thd->security_ctx->host_or_ip, field_name, tab);
      goto error;
    }
  }
#endif
  fixed= 1;
  if (thd->variables.sql_mode & MODE_ONLY_FULL_GROUP_BY &&
      !outer_fixed && !thd->lex->in_sum_func &&
      thd->lex->current_select->cur_pos_in_select_list != UNDEF_POS &&
      thd->lex->current_select->join)
  {
    thd->lex->current_select->join->non_agg_fields.push_back(this);
    marker= thd->lex->current_select->cur_pos_in_select_list;
  }
mark_non_agg_field:
  /*
    table->pos_in_table_list can be 0 when fixing partition functions
    or virtual fields.
  */
  if (fixed && (thd->variables.sql_mode & MODE_ONLY_FULL_GROUP_BY) &&
      field->table->pos_in_table_list)
  {
    /*
      Mark selects according to presence of non aggregated fields.
      Fields from outer selects added to the aggregate function
      outer_fields list as it's unknown at the moment whether it's
      aggregated or not.
      We're using the select lex of the cached table (if present).
    */
    SELECT_LEX *select_lex;
    if (cached_table)
      select_lex= cached_table->select_lex;
    else if (!(select_lex= field->table->pos_in_table_list->select_lex))
    {
      /*
        This can only happen when there is no real table in the query.
        We are using the field's resolution context. context->select_lex is eee
        safe for use because it's either the SELECT we want to use 
        (the current level) or a stub added by non-SELECT queries.
      */
      select_lex= context->select_lex;
    }
    if (!thd->lex->in_sum_func)
      select_lex->set_non_agg_field_used(true);
    else
    {
      if (outer_fixed)
        thd->lex->in_sum_func->outer_fields.push_back(this);
      else if (thd->lex->in_sum_func->nest_level !=
          thd->lex->current_select->nest_level)
        select_lex->set_non_agg_field_used(true);
    }
  }
  return FALSE;

error:
  context->process_error(thd);
  return TRUE;
}