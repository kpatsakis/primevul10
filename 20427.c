setup_anchor(Node* node, regex_t* reg, int state, ScanEnv* env)
{
/* allowed node types in look-behind */
#define ALLOWED_TYPE_IN_LB \
  ( NODE_BIT_LIST | NODE_BIT_ALT | NODE_BIT_STRING | NODE_BIT_CCLASS \
  | NODE_BIT_CTYPE | NODE_BIT_ANCHOR | NODE_BIT_ENCLOSURE | NODE_BIT_QUANT \
  | NODE_BIT_CALL | NODE_BIT_GIMMICK)

#define ALLOWED_ENCLOSURE_IN_LB       ( 1<<ENCLOSURE_MEMORY | 1<<ENCLOSURE_OPTION )
#define ALLOWED_ENCLOSURE_IN_LB_NOT   (1<<ENCLOSURE_OPTION)

#define ALLOWED_ANCHOR_IN_LB \
  ( ANCHOR_LOOK_BEHIND | ANCHOR_BEGIN_LINE | ANCHOR_END_LINE | ANCHOR_BEGIN_BUF \
  | ANCHOR_BEGIN_POSITION | ANCHOR_WORD_BOUNDARY | ANCHOR_NO_WORD_BOUNDARY \
  | ANCHOR_WORD_BEGIN | ANCHOR_WORD_END \
  | ANCHOR_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY \
  | ANCHOR_NO_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY )

#define ALLOWED_ANCHOR_IN_LB_NOT \
  ( ANCHOR_LOOK_BEHIND | ANCHOR_LOOK_BEHIND_NOT | ANCHOR_BEGIN_LINE \
  | ANCHOR_END_LINE | ANCHOR_BEGIN_BUF | ANCHOR_BEGIN_POSITION | ANCHOR_WORD_BOUNDARY \
  | ANCHOR_NO_WORD_BOUNDARY | ANCHOR_WORD_BEGIN | ANCHOR_WORD_END \
  | ANCHOR_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY \
  | ANCHOR_NO_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY )

  int r;
  AnchorNode* an = ANCHOR_(node);

  switch (an->type) {
  case ANCHOR_PREC_READ:
    r = setup_tree(NODE_ANCHOR_BODY(an), reg, state, env);
    break;
  case ANCHOR_PREC_READ_NOT:
    r = setup_tree(NODE_ANCHOR_BODY(an), reg, (state | IN_NOT), env);
    break;

  case ANCHOR_LOOK_BEHIND:
    {
      r = check_type_tree(NODE_ANCHOR_BODY(an), ALLOWED_TYPE_IN_LB,
                          ALLOWED_ENCLOSURE_IN_LB, ALLOWED_ANCHOR_IN_LB);
      if (r < 0) return r;
      if (r > 0) return ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
      r = setup_tree(NODE_ANCHOR_BODY(an), reg, state, env);
      if (r != 0) return r;
      r = setup_look_behind(node, reg, env);
    }
    break;

  case ANCHOR_LOOK_BEHIND_NOT:
    {
      r = check_type_tree(NODE_ANCHOR_BODY(an), ALLOWED_TYPE_IN_LB,
                          ALLOWED_ENCLOSURE_IN_LB_NOT, ALLOWED_ANCHOR_IN_LB_NOT);
      if (r < 0) return r;
      if (r > 0) return ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
      r = setup_tree(NODE_ANCHOR_BODY(an), reg, (state | IN_NOT), env);
      if (r != 0) return r;
      r = setup_look_behind(node, reg, env);
    }
    break;

  default:
    r = 0;
    break;
  }

  return r;
}