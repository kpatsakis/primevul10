set_sub_anchor(regex_t* reg, OptAnc* anc)
{
  reg->sub_anchor |= anc->left  & ANCHOR_BEGIN_LINE;
  reg->sub_anchor |= anc->right & ANCHOR_END_LINE;
}