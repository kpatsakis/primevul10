void copy_winopt(winopt_T *from, winopt_T *to)
{
  to->wo_arab = from->wo_arab;
  to->wo_list = from->wo_list;
  to->wo_nu = from->wo_nu;
  to->wo_rnu = from->wo_rnu;
  to->wo_nuw = from->wo_nuw;
  to->wo_rl  = from->wo_rl;
  to->wo_rlc = vim_strsave(from->wo_rlc);
  to->wo_stl = vim_strsave(from->wo_stl);
  to->wo_wrap = from->wo_wrap;
  to->wo_wrap_save = from->wo_wrap_save;
  to->wo_lbr = from->wo_lbr;
  to->wo_bri = from->wo_bri;
  to->wo_briopt = vim_strsave(from->wo_briopt);
  to->wo_scb = from->wo_scb;
  to->wo_scb_save = from->wo_scb_save;
  to->wo_crb = from->wo_crb;
  to->wo_crb_save = from->wo_crb_save;
  to->wo_spell = from->wo_spell;
  to->wo_cuc = from->wo_cuc;
  to->wo_cul = from->wo_cul;
  to->wo_cc = vim_strsave(from->wo_cc);
  to->wo_diff = from->wo_diff;
  to->wo_diff_saved = from->wo_diff_saved;
  to->wo_cocu = vim_strsave(from->wo_cocu);
  to->wo_cole = from->wo_cole;
  to->wo_fdc = from->wo_fdc;
  to->wo_fdc_save = from->wo_fdc_save;
  to->wo_fen = from->wo_fen;
  to->wo_fen_save = from->wo_fen_save;
  to->wo_fdi = vim_strsave(from->wo_fdi);
  to->wo_fml = from->wo_fml;
  to->wo_fdl = from->wo_fdl;
  to->wo_fdl_save = from->wo_fdl_save;
  to->wo_fdm = vim_strsave(from->wo_fdm);
  to->wo_fdm_save = from->wo_diff_saved
                    ? vim_strsave(from->wo_fdm_save) : empty_option;
  to->wo_fdn = from->wo_fdn;
  to->wo_fde = vim_strsave(from->wo_fde);
  to->wo_fdt = vim_strsave(from->wo_fdt);
  to->wo_fmr = vim_strsave(from->wo_fmr);
  check_winopt(to);             /* don't want NULL pointers */
}