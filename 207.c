  UpdateAccessTimeTask(
      QuotaManager* manager,
      const GURL& origin,
      StorageType type,
      base::Time accessed_time)
      : DatabaseTaskBase(manager),
        origin_(origin),
        type_(type),
        accessed_time_(accessed_time) {}
