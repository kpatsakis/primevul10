static int push(json_parser *json, Mode mode) {
  if (json->top + 1 >= json->depth) {
    return false;
  }
  json->top += 1;
  json->stack[json->top].mode = mode;
  if (json->top > json->mark) {
    json->mark = json->top;
  }
  return true;
}