DataObjectItem::DataObjectItem(ItemKind kind,
                               const String& type,
                               uint64_t sequence_number)
    : source_(kClipboardSource),
      kind_(kind),
      type_(type),
      sequence_number_(sequence_number) {}
