static void set_runtimepath_default(void)
{
  size_t rtp_size = 0;
  char *const data_home = stdpaths_get_xdg_var(kXDGDataHome);
  char *const config_home = stdpaths_get_xdg_var(kXDGConfigHome);
  char *const vimruntime = vim_getenv("VIMRUNTIME");
  char *const data_dirs = stdpaths_get_xdg_var(kXDGDataDirs);
  char *const config_dirs = stdpaths_get_xdg_var(kXDGConfigDirs);
#define SITE_SIZE (sizeof("site") - 1)
#define AFTER_SIZE (sizeof("after") - 1)
  size_t data_len = 0;
  size_t config_len = 0;
  size_t vimruntime_len = 0;
  if (data_home != NULL) {
    data_len = strlen(data_home);
    if (data_len != 0) {
      rtp_size += ((data_len + memcnt(data_home, ',', data_len)
                    + NVIM_SIZE + 1 + SITE_SIZE + 1
                    + !after_pathsep(data_home, data_home + data_len)) * 2
                   + AFTER_SIZE + 1);
    }
  }
  if (config_home != NULL) {
    config_len = strlen(config_home);
    if (config_len != 0) {
      rtp_size += ((config_len + memcnt(config_home, ',', config_len)
                    + NVIM_SIZE + 1
                    + !after_pathsep(config_home, config_home + config_len)) * 2
                   + AFTER_SIZE + 1);
    }
  }
  if (vimruntime != NULL) {
    vimruntime_len = strlen(vimruntime);
    if (vimruntime_len != 0) {
      rtp_size += vimruntime_len + memcnt(vimruntime, ',', vimruntime_len) + 1;
    }
  }
  rtp_size += compute_double_colon_len(data_dirs, NVIM_SIZE + 1 + SITE_SIZE + 1,
                                       AFTER_SIZE + 1);
  rtp_size += compute_double_colon_len(config_dirs, NVIM_SIZE + 1,
                                       AFTER_SIZE + 1);
  if (rtp_size == 0) {
    return;
  }
  char *const rtp = xmalloc(rtp_size);
  char *rtp_cur = rtp;
  rtp_cur = add_dir(rtp_cur, config_home, config_len, true, NULL, 0, NULL, 0);
  rtp_cur = add_colon_dirs(rtp_cur, config_dirs, NULL, 0, NULL, 0, true);
  rtp_cur = add_dir(rtp_cur, data_home, data_len, true, "site", SITE_SIZE,
                    NULL, 0);
  rtp_cur = add_colon_dirs(rtp_cur, data_dirs, "site", SITE_SIZE, NULL, 0,
                           true);
  rtp_cur = add_dir(rtp_cur, vimruntime, vimruntime_len, false, NULL, 0,
                    NULL, 0);
  rtp_cur = add_colon_dirs(rtp_cur, data_dirs, "site", SITE_SIZE,
                           "after", AFTER_SIZE, false);
  rtp_cur = add_dir(rtp_cur, data_home, data_len, true, "site", SITE_SIZE,
                    "after", AFTER_SIZE);
  rtp_cur = add_colon_dirs(rtp_cur, config_dirs, "after", AFTER_SIZE, NULL, 0,
                           false);
  rtp_cur = add_dir(rtp_cur, config_home, config_len, true,
                    "after", AFTER_SIZE, NULL, 0);
  // Strip trailing comma.
  rtp_cur[-1] = NUL;
  assert((size_t) (rtp_cur - rtp) == rtp_size);
#undef SITE_SIZE
#undef AFTER_SIZE
  set_string_default("runtimepath", rtp, true);
  // Make a copy of 'rtp' for 'packpath'
  set_string_default("packpath", rtp, false);
  xfree(data_dirs);
  xfree(config_dirs);
  xfree(data_home);
  xfree(config_home);
  xfree(vimruntime);
}