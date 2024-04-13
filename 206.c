void QuotaManager::StartEviction() {
  DCHECK(!temporary_storage_evictor_.get());
  temporary_storage_evictor_.reset(new QuotaTemporaryStorageEvictor(this,
      kEvictionIntervalInMilliSeconds));
  temporary_storage_evictor_->Start();
}
