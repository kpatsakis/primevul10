POSITION::POSITION()
{
  table= 0;
  records_read= cond_selectivity= read_time= 0.0;
  prefix_record_count= 0.0;
  key= 0;
  use_join_buffer= 0;
  sj_strategy= SJ_OPT_NONE;
  n_sj_tables= 0;
  spl_plan= 0;
  range_rowid_filter_info= 0;
  ref_depend_map= dups_producing_tables= 0;
  inner_tables_handled_with_other_sjs= 0;
  dups_weedout_picker.set_empty();
  firstmatch_picker.set_empty();
  loosescan_picker.set_empty();
  sjmat_picker.set_empty();
}