  UsageAndQuotaDispatcherTask(
      QuotaManager* manager,
      const std::string& host,
      StorageType type)
      : QuotaTask(manager),
        host_(host),
        type_(type),
        quota_(-1),
        global_usage_(-1),
        global_unlimited_usage_(-1),
        host_usage_(-1),
        quota_status_(kQuotaStatusUnknown),
        waiting_callbacks_(1),
        callback_factory_(ALLOW_THIS_IN_INITIALIZER_LIST(this)) {}
