optimize_cond(JOIN *join, COND *conds,
              List<TABLE_LIST> *join_list, bool ignore_on_conds,
              Item::cond_result *cond_value, COND_EQUAL **cond_equal,
              int flags)
{
  THD *thd= join->thd;
  DBUG_ENTER("optimize_cond");

  if (!conds)
  {
    *cond_value= Item::COND_TRUE;
    if (!ignore_on_conds)
      build_equal_items(join, NULL, NULL, join_list, ignore_on_conds,
                        cond_equal);
  }  
  else
  {
    /* 
      Build all multiple equality predicates and eliminate equality
      predicates that can be inferred from these multiple equalities.
      For each reference of a field included into a multiple equality
      that occurs in a function set a pointer to the multiple equality
      predicate. Substitute a constant instead of this field if the
      multiple equality contains a constant.
    */

    Json_writer_object trace_wrapper(thd);
    Json_writer_object trace_cond(thd, "condition_processing");
    trace_cond.add("condition", join->conds == conds ? "WHERE" : "HAVING")
              .add("original_condition", conds);

    Json_writer_array trace_steps(thd, "steps");
    DBUG_EXECUTE("where", print_where(conds, "original", QT_ORDINARY););
    conds= build_equal_items(join, conds, NULL, join_list, 
                             ignore_on_conds, cond_equal,
                             MY_TEST(flags & OPT_LINK_EQUAL_FIELDS));
    DBUG_EXECUTE("where",print_where(conds,"after equal_items", QT_ORDINARY););
    {
      Json_writer_object equal_prop_wrapper(thd);
      equal_prop_wrapper.add("transformation", "equality_propagation")
                        .add("resulting_condition", conds);
    }

    /* change field = field to field = const for each found field = const */
    propagate_cond_constants(thd, (I_List<COND_CMP> *) 0, conds, conds);
    /*
      Remove all instances of item == item
      Remove all and-levels where CONST item != CONST item
    */
    DBUG_EXECUTE("where",print_where(conds,"after const change", QT_ORDINARY););
    {
      Json_writer_object const_prop_wrapper(thd);
      const_prop_wrapper.add("transformation", "constant_propagation")
                        .add("resulting_condition", conds);
    }
    conds= conds->remove_eq_conds(thd, cond_value, true);
    if (conds && conds->type() == Item::COND_ITEM &&
        ((Item_cond*) conds)->functype() == Item_func::COND_AND_FUNC)
      *cond_equal= &((Item_cond_and*) conds)->m_cond_equal;

    {
      Json_writer_object cond_removal_wrapper(thd);
      cond_removal_wrapper.add("transformation", "trivial_condition_removal")
                          .add("resulting_condition", conds);
    }
    DBUG_EXECUTE("info",print_where(conds,"after remove", QT_ORDINARY););
  }
  DBUG_RETURN(conds);
}