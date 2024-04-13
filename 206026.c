bool Item_ident::collect_outer_ref_processor(uchar *param)
{
  Collect_deps_prm *prm= (Collect_deps_prm *)param;
  if (depended_from && 
      depended_from->nest_level_base == prm->nest_level_base &&
      depended_from->nest_level < prm->nest_level)
    prm->parameters->add_unique(this, &cmp_items);
  return FALSE;
}