QuotaManager::QuotaManager(bool is_incognito,
                           const FilePath& profile_path,
                           base::MessageLoopProxy* io_thread,
                           base::MessageLoopProxy* db_thread,
                           SpecialStoragePolicy* special_storage_policy)
  : is_incognito_(is_incognito),
    profile_path_(profile_path),
    proxy_(new QuotaManagerProxy(
        ALLOW_THIS_IN_INITIALIZER_LIST(this), io_thread)),
    db_disabled_(false),
    eviction_disabled_(false),
    io_thread_(io_thread),
    db_thread_(db_thread),
    need_initialize_origins_(false),
    temporary_global_quota_(-1),
    special_storage_policy_(special_storage_policy),
    callback_factory_(ALLOW_THIS_IN_INITIALIZER_LIST(this)) {
}
