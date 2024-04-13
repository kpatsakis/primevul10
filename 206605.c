void StreamReq::Dispose() {
  BaseObjectPtr<AsyncWrap> destroy_me{GetAsyncWrap()};
  object()->SetAlignedPointerInInternalField(
      StreamReq::kStreamReqField, nullptr);
  destroy_me->Detach();
}