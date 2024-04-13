Item_field::fix_outer_field(THD *thd, Field **from_field, Item **reference)
{
  enum_parsing_place place= NO_MATTER;
  bool field_found= (*from_field != not_found_field);
  bool upward_lookup= FALSE;
  TABLE_LIST *table_list;

  /* Calulate the TABLE_LIST for the table */
  table_list= (cached_table ? cached_table :
               field_found && (*from_field) != view_ref_found ?
               (*from_field)->table->pos_in_table_list : 0);
  /*
    If there are outer contexts (outer selects, but current select is
    not derived table or view) try to resolve this reference in the
    outer contexts.

    We treat each subselect as a separate namespace, so that different
    subselects may contain columns with the same names. The subselects
    are searched starting from the innermost.
  */
  Name_resolution_context *last_checked_context= context;
  Item **ref= (Item **) not_found_item;
  SELECT_LEX *current_sel= (SELECT_LEX *) thd->lex->current_select;
  Name_resolution_context *outer_context= 0;
  SELECT_LEX *select= 0;
  /* Currently derived tables cannot be correlated */
  if (current_sel->master_unit()->first_select()->linkage !=
      DERIVED_TABLE_TYPE)
    outer_context= context->outer_context;

  /*
    This assert is to ensure we have an outer contex when *from_field
    is set.
    If this would not be the case, we would assert in mark_as_dependent
    as last_checked_countex == context
  */
  DBUG_ASSERT(outer_context || !*from_field ||
              *from_field == not_found_field);
  for (;
       outer_context;
       outer_context= outer_context->outer_context)
  {
    select= outer_context->select_lex;
    Item_subselect *prev_subselect_item=
      last_checked_context->select_lex->master_unit()->item;
    last_checked_context= outer_context;
    upward_lookup= TRUE;

    place= prev_subselect_item->parsing_place;
    /*
      If outer_field is set, field was already found by first call
      to find_field_in_tables(). Only need to find appropriate context.
    */
    if (field_found && outer_context->select_lex !=
        table_list->select_lex)
      continue;
    /*
      In case of a view, find_field_in_tables() writes the pointer to
      the found view field into '*reference', in other words, it
      substitutes this Item_field with the found expression.
    */
    if (field_found || (*from_field= find_field_in_tables(thd, this,
                                          outer_context->
                                            first_name_resolution_table,
                                          outer_context->
                                            last_name_resolution_table,
                                          reference,
                                          IGNORE_EXCEPT_NON_UNIQUE,
                                          TRUE, TRUE)) !=
        not_found_field)
    {
      if (*from_field)
      {
        if (thd->variables.sql_mode & MODE_ONLY_FULL_GROUP_BY &&
            select->cur_pos_in_select_list != UNDEF_POS)
        {
          /*
            As this is an outer field it should be added to the list of
            non aggregated fields of the outer select.
          */
          if (select->join)
          {
            marker= select->cur_pos_in_select_list;
            select->join->non_agg_fields.push_back(this);
          }
          else
          {
            /*
              join is absent if it is upper SELECT_LEX of non-select
              command
            */
            DBUG_ASSERT(select->master_unit()->outer_select() == NULL &&
                        (thd->lex->sql_command != SQLCOM_SELECT &&
                         thd->lex->sql_command != SQLCOM_UPDATE_MULTI &&
                         thd->lex->sql_command != SQLCOM_DELETE_MULTI &&
                         thd->lex->sql_command != SQLCOM_INSERT_SELECT &&
                         thd->lex->sql_command != SQLCOM_REPLACE_SELECT));
          }
        }
        if (*from_field != view_ref_found)
        {
          prev_subselect_item->used_tables_cache|= (*from_field)->table->map;
          prev_subselect_item->const_item_cache= 0;
          set_field(*from_field);
          if (!last_checked_context->select_lex->having_fix_field &&
              select->group_list.elements &&
              (place == SELECT_LIST || place == IN_HAVING))
          {
            Item_outer_ref *rf;
            /*
              If an outer field is resolved in a grouping select then it
              is replaced for an Item_outer_ref object. Otherwise an
              Item_field object is used.
              The new Item_outer_ref object is saved in the inner_refs_list of
              the outer select. Here it is only created. It can be fixed only
              after the original field has been fixed and this is done in the
              fix_inner_refs() function.
            */
            ;
            if (!(rf= new Item_outer_ref(context, this)))
              return -1;
            thd->change_item_tree(reference, rf);
            select->inner_refs_list.push_back(rf);
            rf->in_sum_func= thd->lex->in_sum_func;
          }
          /*
            A reference is resolved to a nest level that's outer or the same as
            the nest level of the enclosing set function : adjust the value of
            max_arg_level for the function if it's needed.
          */
          if (thd->lex->in_sum_func &&
              thd->lex->in_sum_func->nest_level >= select->nest_level)
          {
            Item::Type ref_type= (*reference)->type();
            set_if_bigger(thd->lex->in_sum_func->max_arg_level,
                          select->nest_level);
            set_field(*from_field);
            fixed= 1;
            mark_as_dependent(thd, last_checked_context->select_lex,
                              context->select_lex, this,
                              ((ref_type == REF_ITEM ||
                                ref_type == FIELD_ITEM) ?
                               (Item_ident*) (*reference) : 0));
            return 0;
          }
        }
        else
        {
          Item::Type ref_type= (*reference)->type();
          prev_subselect_item->used_tables_cache|=
            (*reference)->used_tables();
          prev_subselect_item->const_item_cache&=
            (*reference)->const_item();
          mark_as_dependent(thd, last_checked_context->select_lex,
                            context->select_lex, this,
                            ((ref_type == REF_ITEM || ref_type == FIELD_ITEM) ?
                             (Item_ident*) (*reference) :
                             0));
          if (thd->lex->in_sum_func &&
              thd->lex->in_sum_func->nest_level >= select->nest_level)
          {
            set_if_bigger(thd->lex->in_sum_func->max_arg_level,
                          select->nest_level);
          }
          /*
            A reference to a view field had been found and we
            substituted it instead of this Item (find_field_in_tables
            does it by assigning the new value to *reference), so now
            we can return from this function.
          */
          return 0;
        }
      }
      break;
    }

    /* Search in SELECT and GROUP lists of the outer select. */
    if (place != IN_WHERE && place != IN_ON)
    {
      if (!(ref= resolve_ref_in_select_and_group(thd, this, select)))
        return -1; /* Some error occurred (e.g. ambiguous names). */
      if (ref != not_found_item)
      {
        DBUG_ASSERT(*ref && (*ref)->fixed);
        prev_subselect_item->used_tables_cache|= (*ref)->used_tables();
        prev_subselect_item->const_item_cache&= (*ref)->const_item();
        break;
      }
    }

    /*
      Reference is not found in this select => this subquery depend on
      outer select (or we just trying to find wrong identifier, in this
      case it does not matter which used tables bits we set)
    */
    prev_subselect_item->used_tables_cache|= OUTER_REF_TABLE_BIT;
    prev_subselect_item->const_item_cache= 0;
  }

  DBUG_ASSERT(ref != 0);
  if (!*from_field)
    return -1;
  if (ref == not_found_item && *from_field == not_found_field)
  {
    if (upward_lookup)
    {
      // We can't say exactly what absent table or field
      my_error(ER_BAD_FIELD_ERROR, MYF(0), full_name(), thd->where);
    }
    else
    {
      /* Call find_field_in_tables only to report the error */
      find_field_in_tables(thd, this,
                           context->first_name_resolution_table,
                           context->last_name_resolution_table,
                           reference, REPORT_ALL_ERRORS,
                           !any_privileges, TRUE);
    }
    return -1;
  }
  else if (ref != not_found_item)
  {
    Item *save;
    Item_ref *rf;

    /* Should have been checked in resolve_ref_in_select_and_group(). */
    DBUG_ASSERT(*ref && (*ref)->fixed);
    /*
      Here, a subset of actions performed by Item_ref::set_properties
      is not enough. So we pass ptr to NULL into Item_[direct]_ref
      constructor, so no initialization is performed, and call 
      fix_fields() below.
    */
    save= *ref;
    *ref= NULL;                             // Don't call set_properties()
    rf= (place == IN_HAVING ?
         new Item_ref(context, ref, (char*) table_name,
                      (char*) field_name, alias_name_used) :
         (!select->group_list.elements ?
         new Item_direct_ref(context, ref, (char*) table_name,
                             (char*) field_name, alias_name_used) :
         new Item_outer_ref(context, ref, (char*) table_name,
                            (char*) field_name, alias_name_used)));
    *ref= save;
    if (!rf)
      return -1;

    if (place != IN_HAVING && select->group_list.elements)
    {
      outer_context->select_lex->inner_refs_list.push_back((Item_outer_ref*)rf);
      ((Item_outer_ref*)rf)->in_sum_func= thd->lex->in_sum_func;
    }
    thd->change_item_tree(reference, rf);
    /*
      rf is Item_ref => never substitute other items (in this case)
      during fix_fields() => we can use rf after fix_fields()
    */
    DBUG_ASSERT(!rf->fixed);                // Assured by Item_ref()
    if (rf->fix_fields(thd, reference) || rf->check_cols(1))
      return -1;

    mark_as_dependent(thd, last_checked_context->select_lex,
                      context->select_lex, rf,
                      rf);

    return 0;
  }
  else
  {
    mark_as_dependent(thd, last_checked_context->select_lex,
                      context->select_lex,
                      this, (Item_ident*)*reference);
    if (last_checked_context->select_lex->having_fix_field)
    {
      Item_ref *rf;
      rf= new Item_ref(context, (*from_field)->table->s->db.str,
                       (*from_field)->table->alias.c_ptr(),
                       (char*) field_name);
      if (!rf)
        return -1;
      thd->change_item_tree(reference, rf);
      /*
        rf is Item_ref => never substitute other items (in this case)
        during fix_fields() => we can use rf after fix_fields()
      */
      DBUG_ASSERT(!rf->fixed);                // Assured by Item_ref()
      if (rf->fix_fields(thd, reference) || rf->check_cols(1))
        return -1;
      return 0;
    }
  }
  return 1;
}