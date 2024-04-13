subselect_partial_match_engine::subselect_partial_match_engine(
  subselect_uniquesubquery_engine *engine_arg,
  TABLE *tmp_table_arg, Item_subselect *item_arg,
  select_result_interceptor *result_arg,
  List<Item> *equi_join_conds_arg,
  bool has_covering_null_row_arg,
  bool has_covering_null_columns_arg,
  uint count_columns_with_nulls_arg)
  :subselect_engine(item_arg, result_arg),
   tmp_table(tmp_table_arg), lookup_engine(engine_arg),
   equi_join_conds(equi_join_conds_arg),
   has_covering_null_row(has_covering_null_row_arg),
   has_covering_null_columns(has_covering_null_columns_arg),
   count_columns_with_nulls(count_columns_with_nulls_arg)
{}