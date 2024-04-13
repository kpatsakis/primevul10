best_extension_by_limited_search(JOIN      *join,
                                 table_map remaining_tables,
                                 uint      idx,
                                 double    record_count,
                                 double    read_time,
                                 uint      search_depth,
                                 uint      prune_level,
                                 uint      use_cond_selectivity)
{
  DBUG_ENTER("best_extension_by_limited_search");

  THD *thd= join->thd;

  DBUG_EXECUTE_IF("show_explain_probe_best_ext_lim_search", 
                  if (dbug_user_var_equals_int(thd, 
                                               "show_explain_probe_select_id", 
                                               join->select_lex->select_number))
                        dbug_serve_apcs(thd, 1);
                 );

  if (unlikely(thd->check_killed()))  // Abort
    DBUG_RETURN(TRUE);

  DBUG_EXECUTE("opt", print_plan(join, idx, read_time, record_count, idx,
                                 "SOFAR:"););

  /* 
     'join' is a partial plan with lower cost than the best plan so far,
     so continue expanding it further with the tables in 'remaining_tables'.
  */
  JOIN_TAB *s;
  double best_record_count= DBL_MAX;
  double best_read_time=    DBL_MAX;
  bool disable_jbuf= join->thd->variables.join_cache_level == 0;

  DBUG_EXECUTE("opt", print_plan(join, idx, record_count, read_time, read_time,
                                "part_plan"););

  /* 
    If we are searching for the execution plan of a materialized semi-join nest
    then allowed_tables contains bits only for the tables from this nest.
  */
  table_map allowed_tables= ~(table_map)0;
  if (join->emb_sjm_nest)
    allowed_tables= join->emb_sjm_nest->sj_inner_tables & ~join->const_table_map;

  for (JOIN_TAB **pos= join->best_ref + idx ; (s= *pos) ; pos++)
  {
    table_map real_table_bit= s->table->map;
    if ((remaining_tables & real_table_bit) && 
        (allowed_tables & real_table_bit) &&
        !(remaining_tables & s->dependent) && 
        (!idx || !check_interleaving_with_nj(s)))
    {
      double current_record_count, current_read_time;
      POSITION *position= join->positions + idx;

      Json_writer_object trace_one_table(thd);
      if (unlikely(thd->trace_started()))
      {
        trace_plan_prefix(join, idx, remaining_tables);
        trace_one_table.add_table_name(s);
      }

      /* Find the best access method from 's' to the current partial plan */
      POSITION loose_scan_pos;
      best_access_path(join, s, remaining_tables, join->positions, idx,
                       disable_jbuf, record_count, position, &loose_scan_pos);

      /* Compute the cost of extending the plan with 's' */
      current_record_count= COST_MULT(record_count, position->records_read);
      const double filter_cmp_gain= position->range_rowid_filter_info
        ? position->range_rowid_filter_info->get_cmp_gain(current_record_count)
        : 0;
      current_read_time=COST_ADD(read_time,
                                 COST_ADD(position->read_time -
                                          filter_cmp_gain,
                                          current_record_count /
                                          (double) TIME_FOR_COMPARE));

      if (unlikely(thd->trace_started()))
      {
        trace_one_table.add("rows_for_plan", current_record_count);
        trace_one_table.add("cost_for_plan", current_read_time);
      }
      advance_sj_state(join, remaining_tables, idx, &current_record_count,
                       &current_read_time, &loose_scan_pos);

      /* Expand only partial plans with lower cost than the best QEP so far */
      if (current_read_time >= join->best_read)
      {
        DBUG_EXECUTE("opt", print_plan(join, idx+1,
                                       current_record_count,
                                       read_time,
                                       current_read_time,
                                       "prune_by_cost"););
        trace_one_table.add("pruned_by_cost", true);
        restore_prev_nj_state(s);
        restore_prev_sj_state(remaining_tables, s, idx);
        continue;
      }

      /*
        Prune some less promising partial plans. This heuristic may miss
        the optimal QEPs, thus it results in a non-exhaustive search.
      */
      if (prune_level == 1)
      {
        if (best_record_count > current_record_count ||
            best_read_time > current_read_time ||
            (idx == join->const_tables &&  // 's' is the first table in the QEP
            s->table == join->sort_by_table))
        {
          if (best_record_count >= current_record_count &&
              best_read_time >= current_read_time &&
              /* TODO: What is the reasoning behind this condition? */
              (!(s->key_dependent & allowed_tables & remaining_tables) ||
               join->positions[idx].records_read < 2.0))
          {
            best_record_count= current_record_count;
            best_read_time=    current_read_time;
          }
        }
        else
        {
          DBUG_EXECUTE("opt", print_plan(join, idx+1,
                                         current_record_count,
                                         read_time,
                                         current_read_time,
                                         "pruned_by_heuristic"););
          trace_one_table.add("pruned_by_heuristic", true);
          restore_prev_nj_state(s);
          restore_prev_sj_state(remaining_tables, s, idx);
          continue;
        }
      }

      double pushdown_cond_selectivity= 1.0;
      if (use_cond_selectivity > 1)
        pushdown_cond_selectivity= table_cond_selectivity(join, idx, s,
				                          remaining_tables &
                                                          ~real_table_bit);
      join->positions[idx].cond_selectivity= pushdown_cond_selectivity;

      if (unlikely(thd->trace_started()) && pushdown_cond_selectivity < 1.0)
        trace_one_table.add("selectivity", pushdown_cond_selectivity);

      double partial_join_cardinality= current_record_count *
                                        pushdown_cond_selectivity;
      if ( (search_depth > 1) && (remaining_tables & ~real_table_bit) & allowed_tables )
      { /* Recursively expand the current partial plan */
        swap_variables(JOIN_TAB*, join->best_ref[idx], *pos);
        Json_writer_array trace_rest(thd, "rest_of_plan");
        if (best_extension_by_limited_search(join,
                                             remaining_tables & ~real_table_bit,
                                             idx + 1,
                                             partial_join_cardinality,
                                             current_read_time,
                                             search_depth - 1,
                                             prune_level,
                                             use_cond_selectivity))
          DBUG_RETURN(TRUE);
        swap_variables(JOIN_TAB*, join->best_ref[idx], *pos);
      }
      else
      { /*
          'join' is either the best partial QEP with 'search_depth' relations,
          or the best complete QEP so far, whichever is smaller.
        */
        if (join->sort_by_table &&
            join->sort_by_table !=
            join->positions[join->const_tables].table->table)
        {
          /*
             We may have to make a temp table, note that this is only a
             heuristic since we cannot know for sure at this point.
             Hence it may be wrong.
          */
          trace_one_table.add("cost_for_sorting", current_record_count);
          current_read_time= COST_ADD(current_read_time, current_record_count);
        }
        trace_one_table.add("estimated_join_cardinality",
                            partial_join_cardinality);
        if (current_read_time < join->best_read)
        {
          memcpy((uchar*) join->best_positions, (uchar*) join->positions,
                 sizeof(POSITION) * (idx + 1));
          join->join_record_count= partial_join_cardinality;
          join->best_read= current_read_time - 0.001;
        }
        DBUG_EXECUTE("opt", print_plan(join, idx+1,
                                       current_record_count,
                                       read_time,
                                       current_read_time,
                                       "full_plan"););
      }
      restore_prev_nj_state(s);
      restore_prev_sj_state(remaining_tables, s, idx);
    }
  }
  DBUG_RETURN(FALSE);
}