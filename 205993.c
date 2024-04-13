  virtual void visit_field(Item_field *item)
  {
    // Find which select the field is in. This is achieved by walking up 
    // the select tree and looking for the table of interest.
    st_select_lex *sel;
    for (sel= current_select;
         sel ;
         sel= (sel->context.outer_context ?
               sel->context.outer_context->select_lex:
               NULL))
    {
      List_iterator<TABLE_LIST> li(sel->leaf_tables);
      TABLE_LIST *tbl;
      while ((tbl= li++))
      {
        if (tbl->table == item->field->table)
        {
          if (sel != current_select)
            mark_as_dependent(thd, sel, current_select, item, item);
          return;
        }
      }
    }
  }