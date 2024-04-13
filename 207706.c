static bool get_range_limit_read_cost(const JOIN_TAB *tab, 
                                      const TABLE *table, 
                                      ha_rows table_records,
                                      uint keynr,
                                      ha_rows rows_limit,
                                      double *read_time)
{
  bool res= false;
  /* 
    We need to adjust the estimates if we had a quick select (or ref(const)) on
    index keynr.
  */
  if (table->quick_keys.is_set(keynr))
  {
    /*
      Start from quick select's rows and cost. These are always cheaper than
      full index scan/cost.
    */
    double best_rows= (double)table->quick_rows[keynr];
    double best_cost= (double)table->quick_costs[keynr];
    
    /*
      Check if ref(const) access was possible on this index. 
    */
    if (tab)
    {
      key_part_map map= 1;
      uint kp;
      /* Find how many key parts would be used by ref(const) */
      for (kp=0; kp < MAX_REF_PARTS; map=map << 1, kp++)
      {
        if (!(table->const_key_parts[keynr] & map))
          break;
      }
      
      if (kp > 0)
      {
        ha_rows ref_rows;
        /*
          Two possible cases:
          1. ref(const) uses the same #key parts as range access. 
          2. ref(const) uses fewer key parts, becasue there is a
            range_cond(key_part+1).
        */
        if (kp == table->quick_key_parts[keynr])
          ref_rows= table->quick_rows[keynr];
        else
          ref_rows= (ha_rows) table->key_info[keynr].actual_rec_per_key(kp-1);

        if (ref_rows > 0)
        {
          double tmp= (double)ref_rows;
          /* Reuse the cost formula from best_access_path: */
          set_if_smaller(tmp, (double) tab->join->thd->variables.max_seeks_for_key);
          if (table->covering_keys.is_set(keynr))
            tmp= table->file->keyread_time(keynr, 1, (ha_rows) tmp);
          else
            tmp= table->file->read_time(keynr, 1,
                                        (ha_rows) MY_MIN(tmp,tab->worst_seeks));
          if (tmp < best_cost)
          {
            best_cost= tmp;
            best_rows= (double)ref_rows;
          }
        }
      }
    }

    /*
      Consider an example:

        SELECT *
        FROM t1
        WHERE key1 BETWEEN 10 AND 20 AND col2='foo'
        ORDER BY key1 LIMIT 10

      If we were using a full index scan on key1, we would need to read this
      many rows to get 10 matches:

        10 / selectivity(key1 BETWEEN 10 AND 20 AND col2='foo')

      This is the number we get in rows_limit.
      But we intend to use range access on key1. The rows returned by quick
      select will satisfy the range part of the condition,
      "key1 BETWEEN 10 and 20". We will still need to filter them with
      the remainder condition, (col2='foo').

      The selectivity of the range access is (best_rows/table_records). We need
      to discount it from the rows_limit:
    */
    double rows_limit_for_quick= rows_limit * (best_rows / table_records);

    if (best_rows > rows_limit_for_quick)
    {
      /*
        LIMIT clause specifies that we will need to read fewer records than
        quick select will return. Assume that quick select's cost is
        proportional to the number of records we need to return (e.g. if we 
        only need 1/3rd of records, it will cost us 1/3rd of quick select's
        read time)
      */
      best_cost *= rows_limit_for_quick / best_rows;
    }
    *read_time= best_cost;
    res= true;
  }
  return res;
}