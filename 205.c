void QuotaManager::SetTemporaryGlobalQuota(int64 new_quota,
                                           QuotaCallback* callback) {
  LazyInitialize();
  if (new_quota < 0) {
    callback->Run(kQuotaErrorInvalidModification,
                  kStorageTypeTemporary, -1);
    delete callback;
    return;
  }

  if (!db_disabled_) {
    scoped_refptr<UpdateTemporaryGlobalQuotaTask> task(
        new UpdateTemporaryGlobalQuotaTask(this, new_quota, callback));
    task->Start();
  } else {
    callback->Run(kQuotaErrorInvalidAccess,
                  kStorageTypeTemporary, -1);
    delete callback;
  }
}
