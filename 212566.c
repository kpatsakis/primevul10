static int pop(json_parser *json, Mode mode) {
  if (json->top < 0 || json->stack[json->top].mode != mode) {
    return false;
  }
  json->stack[json->top].mode = Mode::INVALID;
  json->top -= 1;
  return true;
}