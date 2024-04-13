  UpdatePersistentHostQuotaTask(
      QuotaManager* manager,
      const std::string& host,
      int new_quota,
      HostQuotaCallback* callback)
      : DatabaseTaskBase(manager),
        host_(host),
        new_quota_(new_quota),
        callback_(callback) {
    DCHECK_GE(new_quota_, 0);
  }
