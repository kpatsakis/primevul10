void QuotaManager::RegisterClient(QuotaClient* client) {
  DCHECK(io_thread_->BelongsToCurrentThread());
  DCHECK(!database_.get());
  clients_.push_back(client);
}
