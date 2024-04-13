commit_checkpoint_notify_ha(handlerton *hton, void *cookie)
{
  tc_log->commit_checkpoint_notify(cookie);
}