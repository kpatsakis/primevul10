String DataObjectItem::GetAsString() const {
  DCHECK_EQ(kind_, kStringKind);

  if (source_ == kInternalSource)
    return data_;

  DCHECK_EQ(source_, kClipboardSource);

  String data;
  if (type_ == kMimeTypeTextPlain) {
    data = SystemClipboard::GetInstance().ReadPlainText();
  } else if (type_ == kMimeTypeTextRTF) {
    data = SystemClipboard::GetInstance().ReadRTF();
  } else if (type_ == kMimeTypeTextHTML) {
    KURL ignored_source_url;
    unsigned ignored;
    data = SystemClipboard::GetInstance().ReadHTML(ignored_source_url, ignored,
                                                   ignored);
  } else {
    data = SystemClipboard::GetInstance().ReadCustomData(type_);
  }

  return SystemClipboard::GetInstance().SequenceNumber() == sequence_number_
             ? data
             : String();
}
