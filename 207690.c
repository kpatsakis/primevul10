bool JOIN::save_explain_data(Explain_query *output, bool can_overwrite,
                             bool need_tmp_table, bool need_order, 
                             bool distinct)
{
  DBUG_ENTER("JOIN::save_explain_data");
  DBUG_PRINT("enter", ("Save explain Select_lex: %u (%p)  parent lex: %p  stmt_lex: %p  present select: %u (%p)",
                        select_lex->select_number, select_lex,
                        select_lex->parent_lex, thd->lex->stmt_lex,
                        (output->get_select(select_lex->select_number) ?
                         select_lex->select_number : 0),
                        (output->get_select(select_lex->select_number) ?
                         output->get_select(select_lex->select_number)
                         ->select_lex : NULL)));
  /*
    If there is SELECT in this statement with the same number it must be the
    same SELECT
  */
  DBUG_SLOW_ASSERT(select_lex->select_number == UINT_MAX ||
              select_lex->select_number == INT_MAX ||
              !output ||
              !output->get_select(select_lex->select_number) ||
              output->get_select(select_lex->select_number)->select_lex ==
                select_lex);

  if (select_lex->select_number != UINT_MAX && 
      select_lex->select_number != INT_MAX /* this is not a UNION's "fake select */ && 
      have_query_plan != JOIN::QEP_NOT_PRESENT_YET && 
      have_query_plan != JOIN::QEP_DELETED &&  // this happens when there was 
                                               // no QEP ever, but then
                                               //cleanup() is called multiple times
      output && // for "SET" command in SPs.
      (can_overwrite? true: !output->get_select(select_lex->select_number)))
  {
    const char *message= NULL;
    if (!table_count || !tables_list || zero_result_cause)
    {
      /* It's a degenerate join */
      message= zero_result_cause ? zero_result_cause : "No tables used";
    }
    bool rc= save_explain_data_intern(thd->lex->explain, need_tmp_table,
                                      need_order, distinct, message);
    DBUG_RETURN(rc);
  }
  
  /*
    Can have join_tab==NULL for degenerate cases (e.g. SELECT .. UNION ... SELECT LIMIT 0)
  */
  if (select_lex == select_lex->master_unit()->fake_select_lex && join_tab)
  {
    /* 
      This is fake_select_lex. It has no query plan, but we need to set up a
      tracker for ANALYZE 
    */
    uint nr= select_lex->master_unit()->first_select()->select_number;
    Explain_union *eu= output->get_union(nr);
    explain= &eu->fake_select_lex_explain;
    join_tab[0].tracker= eu->get_fake_select_lex_tracker();
    for (uint i=0 ; i < exec_join_tab_cnt() + aggr_tables; i++)
    {
      if (join_tab[i].filesort)
      {
        if (!(join_tab[i].filesort->tracker=
              new Filesort_tracker(thd->lex->analyze_stmt)))
          DBUG_RETURN(1);
      }
    }
  }
  DBUG_RETURN(0);
}