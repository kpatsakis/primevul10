  Http::FilterHeadersStatus decodeHeaders(Http::RequestHeaderMap& headers, bool) override {
    const auto entry = headers.get(header_);
    if (!entry.empty()) {
      decoder_callbacks_->streamInfo().filterState()->setData(
          state_, std::make_unique<Router::StringAccessorImpl>(entry[0]->value().getStringView()),
          StreamInfo::FilterState::StateType::ReadOnly,
          StreamInfo::FilterState::LifeSpan::FilterChain);
    }
    return Http::FilterHeadersStatus::Continue;
  }