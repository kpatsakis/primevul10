concat_left_node_opt_info(OnigEncoding enc, NodeOpt* to, NodeOpt* add)
{
  int exb_reach, exm_reach;
  OptAnc tanc;

  concat_opt_anc_info(&tanc, &to->anc, &add->anc, to->len.max, add->len.max);
  copy_opt_anc_info(&to->anc, &tanc);

  if (add->exb.len > 0 && to->len.max == 0) {
    concat_opt_anc_info(&tanc, &to->anc, &add->exb.anc, to->len.max, add->len.max);
    copy_opt_anc_info(&add->exb.anc, &tanc);
  }

  if (add->map.value > 0 && to->len.max == 0) {
    if (add->map.mmd.max == 0)
      add->map.anc.left |= to->anc.left;
  }

  exb_reach = to->exb.reach_end;
  exm_reach = to->exm.reach_end;

  if (add->len.max != 0)
    to->exb.reach_end = to->exm.reach_end = 0;

  if (add->exb.len > 0) {
    if (exb_reach) {
      concat_opt_exact(&to->exb, &add->exb, enc);
      clear_opt_exact(&add->exb);
    }
    else if (exm_reach) {
      concat_opt_exact(&to->exm, &add->exb, enc);
      clear_opt_exact(&add->exb);
    }
  }
  select_opt_exact(enc, &to->exm, &add->exb);
  select_opt_exact(enc, &to->exm, &add->exm);

  if (to->expr.len > 0) {
    if (add->len.max > 0) {
      if (to->expr.len > (int )add->len.max)
        to->expr.len = add->len.max;

      if (to->expr.mmd.max == 0)
        select_opt_exact(enc, &to->exb, &to->expr);
      else
        select_opt_exact(enc, &to->exm, &to->expr);
    }
  }
  else if (add->expr.len > 0) {
    copy_opt_exact(&to->expr, &add->expr);
  }

  select_opt_map(&to->map, &add->map);
  add_mml(&to->len, &add->len);
}