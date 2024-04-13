set_optimize_info_from_tree(Node* node, regex_t* reg, ScanEnv* scan_env)
{
  int r;
  NodeOpt opt;
  OptEnv env;

  env.enc            = reg->enc;
  env.options        = reg->options;
  env.case_fold_flag = reg->case_fold_flag;
  env.scan_env       = scan_env;
  clear_mml(&env.mmd);

  r = optimize_nodes(node, &opt, &env);
  if (r != 0) return r;

  reg->anchor = opt.anc.left & (ANCHOR_BEGIN_BUF |
        ANCHOR_BEGIN_POSITION | ANCHOR_ANYCHAR_INF | ANCHOR_ANYCHAR_INF_ML |
        ANCHOR_LOOK_BEHIND);

  if ((opt.anc.left & (ANCHOR_LOOK_BEHIND | ANCHOR_PREC_READ_NOT)) != 0)
    reg->anchor &= ~ANCHOR_ANYCHAR_INF_ML;

  reg->anchor |= opt.anc.right & (ANCHOR_END_BUF | ANCHOR_SEMI_END_BUF |
       ANCHOR_PREC_READ_NOT);

  if (reg->anchor & (ANCHOR_END_BUF | ANCHOR_SEMI_END_BUF)) {
    reg->anchor_dmin = opt.len.min;
    reg->anchor_dmax = opt.len.max;
  }

  if (opt.exb.len > 0 || opt.exm.len > 0) {
    select_opt_exact(reg->enc, &opt.exb, &opt.exm);
    if (opt.map.value > 0 && comp_opt_exact_or_map(&opt.exb, &opt.map) > 0) {
      goto set_map;
    }
    else {
      r = set_optimize_exact(reg, &opt.exb);
      set_sub_anchor(reg, &opt.exb.anc);
    }
  }
  else if (opt.map.value > 0) {
  set_map:
    set_optimize_map(reg, &opt.map);
    set_sub_anchor(reg, &opt.map.anc);
  }
  else {
    reg->sub_anchor |= opt.anc.left & ANCHOR_BEGIN_LINE;
    if (opt.len.max == 0)
      reg->sub_anchor |= opt.anc.right & ANCHOR_END_LINE;
  }

#if defined(ONIG_DEBUG_COMPILE) || defined(ONIG_DEBUG_MATCH)
  print_optimize_info(stderr, reg);
#endif
  return r;
}