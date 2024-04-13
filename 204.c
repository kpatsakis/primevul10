void QuotaManager::SetPersistentHostQuota(const std::string& host,
                                          int64 new_quota,
                                          HostQuotaCallback* callback_ptr) {
  scoped_ptr<HostQuotaCallback> callback(callback_ptr);
  LazyInitialize();
  if (host.empty()) {
    callback->Run(kQuotaErrorNotSupported, host, kStorageTypePersistent, 0);
    return;
  }
  if (new_quota < 0) {
    callback->Run(kQuotaErrorInvalidModification,
                  host, kStorageTypePersistent, -1);
    return;
  }

  if (!db_disabled_) {
    scoped_refptr<UpdatePersistentHostQuotaTask> task(
        new UpdatePersistentHostQuotaTask(
            this, host, new_quota, callback.release()));
    task->Start();
  } else {
    callback->Run(kQuotaErrorInvalidAccess,
                  host, kStorageTypePersistent, -1);
  }
}
