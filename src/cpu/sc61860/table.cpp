
static void sc61860_instruction(void)
{
  int oper=READ_OP();
  if ((oper&0xc0)==0x80)
  {
    sc61860_load_imm_p(oper&0x3f);
    sc61860_icount-=2;
  }
  else if ((oper&0xe0)==0xe0)
  {
    sc61860_call(READ_OP()|((oper&0x1f)<<8));
    sc61860_icount-=7;
  }
  else
  {
    switch(oper)
    {
      case 0:
        sc61860_load_imm(I, READ_OP());
        sc61860_icount-=4;
        break;
      case 1:
        sc61860_load_imm(J, READ_OP());
        sc61860_icount-=4;
        break;
      case 2:
        sc61860_load_imm(A, READ_OP());
        sc61860_icount-=4;
        break;
      case 3:
        sc61860_load_imm(B, READ_OP());
        sc61860_icount-=4;
        break;
      case 4:
        sc61860_inc_load_dp(XL);
        sc61860_icount-=6;
        break;
      case 5:
        sc61860_dec_load_dp(XL);
        sc61860_icount-=6;
        break;
      case 6:
        sc61860_inc_load_dp(YL);
        sc61860_icount-=6;
        break;
      case 7:
        sc61860_dec_load_dp(YL);
        sc61860_icount-=6;
        break;
      case 8:
        sc61860_copy(sc61860.ram[I]);
        break;
      case 9:
        sc61860_exchange(sc61860.ram[I]);
        break;
      case 10:
        sc61860_copy(sc61860.ram[J]);
        break;
      case 11:
        sc61860_exchange(sc61860.ram[J]);
        break;
      case 12:
        sc61860_add_bcd_a();
        sc61860_icount-=7;
        break;
      case 13:
        sc61860_sub_bcd_a();
        sc61860_icount-=7;
        break;
      case 14:
        sc61860_add_bcd();
        sc61860_icount-=7;
        break;
      case 15:
        sc61860_sub_bcd();
        sc61860_icount-=7;
        break;
      case 16:
        sc61860_load_dp();
        sc61860_icount-=8;
        break;
      case 17:
        sc61860_load_dl();
        sc61860_icount-=5;
        break;
      case 18:
        sc61860_load_imm_p(READ_OP());
        sc61860_icount-=4;
        break;
      case 19:
        sc61860_load_imm_q(READ_OP());
        sc61860_icount-=4;
        break;
      case 20:
        sc61860_add_word();
        sc61860_icount-=5;
        break;
      case 21:
        sc61860_sub_word();
        sc61860_icount-=5;
        break;
      case 24:
        sc61860_copy_ext(sc61860.ram[I]);
        break;
      case 25:
        sc61860_exchange_ext(sc61860.ram[I]);
        break;
      case 26:
        sc61860_copy_ext(sc61860.ram[J]);
        break;
      case 27:
        sc61860_exchange_ext(sc61860.ram[J]);
        break;
      case 28:
        sc61860_shift_right_nibble();
        sc61860_icount-=5;
        break;
      case 29:
        sc61860_shift_left_nibble();
        sc61860_icount-=5;
        break;
      case 30:
        sc61860_fill();
        sc61860_icount-=5;
        break;
      case 31:
        sc61860_fill_ext();
        sc61860_icount-=4;
        break;
      case 32:
        sc61860_store_p();
        sc61860_icount-=2;
        break;
      case 33:
        sc61860_store_q();
        sc61860_icount-=2;
        break;
      case 34:
        sc61860_store_r();
        sc61860_icount-=2;
        break;
      case 36:
        sc61860_inc_load_dp_load();
        sc61860_icount-=7;
        break;
      case 37:
        sc61860_dec_load_dp_load();
        sc61860_icount-=7;
        break;
      case 38:
        sc61860_inc_load_dp_store();
        sc61860_icount-=7;
        break;
      case 39:
        sc61860_dec_load_dp_store();
        sc61860_icount-=7;
        break;
      case 40:
        sc61860_jump_rel_plus(!sc61860.zero);
        sc61860_icount-=4;
        break;
      case 41:
        sc61860_jump_rel_minus(!sc61860.zero);
        sc61860_icount-=4;
        break;
      case 42:
        sc61860_jump_rel_plus(!sc61860.carry);
        sc61860_icount-=4;
        break;
      case 43:
        sc61860_jump_rel_minus(!sc61860.carry);
        sc61860_icount-=4;
        break;
      case 44:
        sc61860_jump_rel_plus(1);
        sc61860_icount-=4;
        break;
      case 45:
        sc61860_jump_rel_minus(1);
        sc61860_icount-=4;
        break;
      case 47:
        sc61860_loop();
        sc61860_icount-=7;
        break;
      case 48:
        sc61860_load_imm_p(sc61860.ram[A]);
        sc61860_icount-=2;
        break;
      case 49:
        sc61860_load_imm_q(sc61860.ram[A]);
        sc61860_icount-=2;
        break;
      case 50:
        sc61860_load_r();
        sc61860_icount-=2;
        break;
      case 52:
        sc61860_push();
        sc61860_icount-=3;
        break;
      case 53:
        sc61860_copy_int(sc61860.ram[I]);
        break;
      case 55:
        sc61860_return();
        sc61860_icount-=4;
        break;
      case 56:
        sc61860_jump_rel_plus(sc61860.zero);
        sc61860_icount-=4;
        break;
      case 57:
        sc61860_jump_rel_minus(sc61860.zero);
        sc61860_icount-=4;
        break;
      case 58:
        sc61860_jump_rel_plus(sc61860.carry);
        sc61860_icount-=4;
        break;
      case 59:
        sc61860_jump_rel_minus(sc61860.carry);
        sc61860_icount-=4;
        break;
      case 64:
        sc61860_inc(I);
        sc61860_icount-=4;
        break;
      case 65:
        sc61860_dec(I);
        sc61860_icount-=4;
        break;
      case 66:
        sc61860_inc(A);
        sc61860_icount-=4;
        break;
      case 67:
        sc61860_dec(A);
        sc61860_icount-=4;
        break;
      case 68:
        sc61860_add(sc61860.p,sc61860.ram[A]);
        sc61860_icount-=3;
        break;
      case 69:
        sc61860_sub(sc61860.p,sc61860.ram[A]);
        sc61860_icount-=3;
        break;
      case 70:
        sc61860_and(sc61860.p,sc61860.ram[A]);
        sc61860_icount-=3;
        break;
      case 71:
        sc61860_or(sc61860.p,sc61860.ram[A]);
        sc61860_icount-=3;
        break;
      case 72:
        sc61860_inc(K);
        sc61860_icount-=4;
        break;
      case 73:
        sc61860_dec(K);
        sc61860_icount-=4;
        break;
      case 74:
        sc61860_inc(V);
        sc61860_icount-=4;
        break;
      case 75:
        sc61860_dec(V);
        sc61860_icount-=4;
        break;
      case 76:
        sc61860_in_a();
        sc61860_icount-=2;
        break;
      case 77: /*nopw*/
        ;
        sc61860_icount-=2;
        break;
      case 78:
        sc61860_wait();
        sc61860_icount-=6;
        break;
      case 80:
        sc61860_inc_p();
        sc61860_icount-=2;
        break;
      case 81:
        sc61860_dec_p();
        sc61860_icount-=2;
        break;
      case 82:
        sc61860_store_ext(A);
        sc61860_icount-=2;
        break;
      case 83:
        sc61860_store_ext(sc61860.p);
        sc61860_icount-=2;
        break;
      case 85:
        sc61860_load_ext(sc61860.p);
        sc61860_icount-=3;
        break;
      case 87:
        sc61860_load_ext(A);
        sc61860_icount-=3;
        break;
      case 88:
        sc61860_swap();
        sc61860_icount-=2;
        break;
      case 89:
        sc61860_load();
        sc61860_icount-=2;
        break;
      case 90:
        sc61860_rotate_left();
        sc61860_icount-=2;
        break;
      case 91:
        sc61860_pop();
        sc61860_icount-=2;
        break;
      case 93:
        sc61860_out_a();
        sc61860_icount-=3;
        break;
      case 95:
        sc61860_out_f();
        sc61860_icount-=3;
        break;
      case 96:
        sc61860_and(sc61860.p,READ_OP());
        sc61860_icount-=4;
        break;
      case 97:
        sc61860_or(sc61860.p,READ_OP());
        sc61860_icount-=4;
        break;
      case 98:
        sc61860_test(sc61860.p,READ_OP());
        sc61860_icount-=4;
        break;
      case 99:
        sc61860_cmp(sc61860.p,READ_OP());
        sc61860_icount-=4;
        break;
      case 100:
        sc61860_and(A,READ_OP());
        sc61860_icount-=4;
        break;
      case 101:
        sc61860_or(A,READ_OP());
        sc61860_icount-=4;
        break;
      case 102:
        sc61860_test(A,READ_OP());
        sc61860_icount-=4;
        break;
      case 103:
        sc61860_cmp(A,READ_OP());
        sc61860_icount-=4;
        break;
      case 105:
        sc61860_execute_table_call();
        sc61860_icount-=3;
        break;
      case 107:
        sc61860_test_special();
        sc61860_icount-=4;
        break;
      case 112:
        sc61860_add(sc61860.p,READ_OP());
        sc61860_icount-=4;
        break;
      case 113:
        sc61860_sub(sc61860.p,READ_OP());
        sc61860_icount-=4;
        break;
      case 116:
        sc61860_add(A,READ_OP());
        sc61860_icount-=4;
        break;
      case 117:
        sc61860_sub(A,READ_OP());
        sc61860_icount-=4;
        break;
      case 120:
        sc61860_call(READ_OP_ARG_WORD());
        sc61860_icount-=8;
        break;
      case 121:
        sc61860_jump(1);
        sc61860_icount-=6;
        break;
      case 122:
        sc61860_prepare_table_call();
        sc61860_icount-=9;
        break;
      case 124:
        sc61860_jump(!sc61860.zero);
        sc61860_icount-=6;
        break;
      case 125:
        sc61860_jump(!sc61860.carry);
        sc61860_icount-=6;
        break;
      case 126:
        sc61860_jump(sc61860.zero);
        sc61860_icount-=6;
        break;
      case 127:
        sc61860_jump(sc61860.carry);
        sc61860_icount-=6;
        break;
      case 192:
        sc61860_inc(J);
        sc61860_icount-=4;
        break;
      case 193:
        sc61860_dec(J);
        sc61860_icount-=4;
        break;
      case 194:
        sc61860_inc(B);
        sc61860_icount-=4;
        break;
      case 195:
        sc61860_dec(B);
        sc61860_icount-=4;
        break;
      case 196:
        sc61860_add_carry();
        sc61860_icount-=3;
        break;
      case 197:
        sc61860_sub_carry();
        sc61860_icount-=3;
        break;
      case 199:
        sc61860_cmp(sc61860.p,sc61860.ram[A]);
        sc61860_icount-=3;
        break;
      case 200:
        sc61860_inc(L);
        sc61860_icount-=4;
        break;
      case 201:
        sc61860_dec(L);
        sc61860_icount-=4;
        break;
      case 202:
        sc61860_inc(W);
        sc61860_icount-=4;
        break;
      case 203:
        sc61860_dec(W);
        sc61860_icount-=4;
        break;
      case 204:
        sc61860_in_b();
        sc61860_icount-=2;
        break;
      case 206: /*nopt*/
        ;
        sc61860_icount-=3;
        break;
      case 208:
        sc61860_set_carry();
        sc61860_icount-=2;
        break;
      case 209:
        sc61860_reset_carry();
        sc61860_icount-=4;
        break;
      case 210:
        sc61860_rotate_right();
        sc61860_icount-=2;
        break;
      case 212:
        sc61860_and_ext();
        sc61860_icount-=6;
        break;
      case 213:
        sc61860_or_ext();
        sc61860_icount-=6;
        break;
      case 214:
        sc61860_test_ext();
        sc61860_icount-=6;
        break;
      case 216:
        sc61860_leave();
        sc61860_icount-=4/*?*/;
        break;
      case 218:
        sc61860_exam(A, B);
        sc61860_icount-=3;
        break;
      case 219:
        sc61860_exam(A, sc61860.p);
        sc61860_icount-=3;
        break;
      case 221:
        sc61860_out_b();
        sc61860_icount-=2;
        break;
      case 223:
        sc61860_out_c();
        sc61860_icount-=2;
        break;
    }
  }
}
