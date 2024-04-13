ha_commit_checkpoint_request(void *cookie, void (*pre_hook)(void *))
{
  st_commit_checkpoint_request st;
  st.cookie= cookie;
  st.pre_hook= pre_hook;
  plugin_foreach(NULL, commit_checkpoint_request_handlerton,
                 MYSQL_STORAGE_ENGINE_PLUGIN, &st);
}