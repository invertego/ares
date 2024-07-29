#define jp(id, name)      case id: return decoder##name(instruction)
#define op(id, name, ...) case id: { OpInfo info = {}; __VA_ARGS__; return info; }
#define br(id, name, ...) case id: { OpInfo info = {}; __VA_ARGS__; return info; }

#define RD     (instruction >> 11 & 31)
#define RT     (instruction >> 16 & 31)
#define RS     (instruction >> 21 & 31)
#define FD     (instruction >>  6 & 31)
#define FS     (instruction >> 11 & 31)
#define FT     (instruction >> 16 & 31)

#define RUse(n)       info.r.use |= 1 << n
#define RDef(n)       info.r.def |= 1 << n
#define FUse(n)       info.f.use |= 1 << n
#define FDef(n)       info.f.def |= 1 << n
#define HiUse         (void)0
#define HiDef         (void)0
#define LoUse         (void)0
#define LoDef         (void)0
#define HiLoDef       HiDef, LoDef
#define CFUse         (void)0
#define CFDef         (void)0
#define Load          info.flags |= OpInfo::Load
#define Store         info.flags |= OpInfo::Store
#define Branch        info.flags |= OpInfo::Branch
#define Likely        info.flags |= OpInfo::Likely
#define Overflow      info.flags |= OpInfo::Overflow
#define Breakpoint    info.flags |= OpInfo::Breakpoint
#define Double        info.flags |= OpInfo::Double
#define Syscall       info.flags |= OpInfo::Syscall
#define Cop0          info.flags |= OpInfo::Cop0
#define Cop1          info.flags |= OpInfo::Cop1
#define Cop2          info.flags |= OpInfo::Cop2
#define Trap          info.flags |= OpInfo::Trap
#define Unimplemented info.flags |= OpInfo::Unimplemented
#define Reserved      info.flags |= OpInfo::Reserved

auto CPU::decoderEXECUTE(u32 instruction) const -> OpInfo {
  switch(instruction >> 26) {
  jp(0x00, SPECIAL);
  jp(0x01, REGIMM);
  br(0x02, J, Branch);
  br(0x03, JAL, RDef(31), Branch);
  br(0x04, BEQ, RUse(RS), RUse(RT), Branch);
  br(0x05, BNE, RUse(RS), RUse(RT), Branch);
  br(0x06, BLEZ, RUse(RS), Branch);
  br(0x07, BGTZ, RUse(RS), Branch);
  op(0x08, ADDI, RDef(RT), RUse(RS), Overflow);
  op(0x09, ADDIU, RDef(RT), RUse(RS));
  op(0x0a, SLTI, RDef(RT), RUse(RS));
  op(0x0b, SLTIU, RDef(RT), RUse(RS));
  op(0x0c, ANDI, RDef(RT), RUse(RS));
  op(0x0d, ORI, RDef(RT), RUse(RS));
  op(0x0e, XORI, RDef(RT), RUse(RS));
  op(0x0f, LUI, RDef(RT));
  jp(0x10, SCC);
  jp(0x11, FPU);
  jp(0x12, COP2);
  br(0x13, COP3, Reserved);
  br(0x14, BEQL, RUse(RS), RUse(RT), Branch, Likely);
  br(0x15, BNEL, RUse(RS), RUse(RT), Branch, Likely);
  br(0x16, BLEZL, RUse(RS), Branch, Likely);
  br(0x17, BGTZL, RUse(RS), Branch, Likely);
  op(0x18, DADDI, RDef(RT), RUse(RS), Double, Overflow);
  op(0x19, DADDIU, RDef(RT), RUse(RS), Double);
  op(0x1a, LDL, RDef(RT), RUse(RT), RUse(RS), Load, Double);
  op(0x1b, LDR, RDef(RT), RUse(RT), RUse(RS), Load, Double);
  br(0x1c, INVALID, Reserved);
  br(0x1d, INVALID, Reserved);
  br(0x1e, INVALID, Reserved);
  br(0x1f, INVALID, Reserved);
  op(0x20, LB, RDef(RT), RUse(RS), Load);
  op(0x21, LH, RDef(RT), RUse(RS), Load);
  op(0x22, LWL, RDef(RT), RUse(RT), RUse(RS), Load);
  op(0x23, LW, RDef(RT), RUse(RS), Load);
  op(0x24, LBU, RDef(RT), RUse(RS), Load);
  op(0x25, LHU, RDef(RT), RUse(RS), Load);
  op(0x26, LWR, RDef(RT), RUse(RT), RUse(RS), Load);
  op(0x27, LWU, RDef(RT), RUse(RS), Load, Double);
  op(0x28, SB, RUse(RT), RUse(RS), Store);
  op(0x29, SH, RUse(RT), RUse(RS), Store);
  op(0x2a, SWL, RUse(RT), RUse(RS), Store);
  op(0x2b, SW, RUse(RT), RUse(RS), Store);
  op(0x2c, SDL, RUse(RT), RUse(RS), Store, Double);
  op(0x2d, SDR, RUse(RT), RUse(RS), Store, Double);
  op(0x2e, SWR, RUse(RT), RUse(RS), Store);
  op(0x2f, CACHE, RUse(RS), Cop0);
  op(0x30, LL, RDef(RT), RUse(RS), Load);
  op(0x31, LWC1, FDef(FT), RUse(RS), Cop1, Load);
  br(0x32, LWC2, RUse(RS), Cop2, Load);
  br(0x33, LWC3, Reserved);
  op(0x34, LLD, RDef(RT), RUse(RS), Load, Double);
  op(0x35, LDC1, FDef(FT), RUse(RS), Cop1, Load);
  br(0x36, LDC2, RUse(RS), Cop2, Load);
  op(0x37, LD, RDef(RT), RUse(RS), Load, Double);
  op(0x38, SC, RDef(RT), RUse(RT), RUse(RS), Store);
  op(0x39, SWC1, FUse(FT), RUse(RS), Cop1, Store);
  br(0x3a, SWC2, RUse(RS), Cop2, Store);
  br(0x3b, SWC3, Reserved);
  op(0x3c, SCD, RDef(RT), RUse(RT), RUse(RS), Store, Double);
  op(0x3d, SDC1, FUse(FT), RUse(RS), Cop1, Store);
  br(0x3e, SDC2, RUse(RS), Cop2, Store);
  op(0x3f, SD, RUse(RT), RUse(RS), Store, Double);
  }

  return {};
}

auto CPU::decoderSPECIAL(u32 instruction) const -> OpInfo {
  switch(instruction & 0x3f) {
  op(0x00, SLL, RDef(RD), RUse(RT));
  br(0x01, INVALID, Reserved);
  op(0x02, SRL, RDef(RD), RUse(RT));
  op(0x03, SRA, RDef(RD), RUse(RT));
  op(0x04, SLLV, RDef(RD), RUse(RT), RUse(RS));
  br(0x05, INVALID, Reserved);
  op(0x06, SRLV, RDef(RD), RUse(RT), RUse(RS));
  op(0x07, SRAV, RDef(RD), RUse(RT), RUse(RS));
  br(0x08, JR, RUse(RS), Branch);
  br(0x09, JALR, RDef(RD), RUse(RS), Branch);
  br(0x0a, INVALID, Reserved);
  br(0x0b, INVALID, Reserved);
  br(0x0c, SYSCALL, Syscall);
  br(0x0d, BREAK, Breakpoint);
  br(0x0e, INVALID, Reserved);
  op(0x0f, SYNC);
  op(0x10, MFHI, RDef(RD), HiUse);
  op(0x11, MTHI, HiDef, RUse(RS));
  op(0x12, MFLO, RDef(RD), LoUse);
  op(0x13, MTLO, LoDef, RUse(RS));
  op(0x14, DSLLV, RDef(RD), RUse(RT), RUse(RS), Double);
  br(0x15, INVALID, Reserved);
  op(0x16, DSRLV, RDef(RD), RUse(RT), RUse(RS), Double);
  op(0x17, DSRAV, RDef(RD), RUse(RT), RUse(RS), Double);
  op(0x18, MULT, HiLoDef, RUse(RS), RUse(RT));
  op(0x19, MULTU, HiLoDef, RUse(RS), RUse(RT));
  op(0x1a, DIV, HiLoDef, RUse(RS), RUse(RT));
  op(0x1b, DIVU, HiLoDef, RUse(RS), RUse(RT));
  op(0x1c, DMULT, HiLoDef, RUse(RS), RUse(RT), Double);
  op(0x1d, DMULTU, HiLoDef, RUse(RS), RUse(RT), Double);
  op(0x1e, DDIV, HiLoDef, RUse(RS), RUse(RT), Double);
  op(0x1f, DDIVU, HiLoDef, RUse(RS), RUse(RT), Double);
  op(0x20, ADD, RDef(RD), RUse(RS), RUse(RT), Overflow);
  op(0x21, ADDU, RDef(RD), RUse(RS), RUse(RT));
  op(0x22, SUB, RDef(RD), RUse(RS), RUse(RT), Overflow);
  op(0x23, SUBU, RDef(RD), RUse(RS), RUse(RT));
  op(0x24, AND, RDef(RD), RUse(RS), RUse(RT));
  op(0x25, OR, RDef(RD), RUse(RS), RUse(RT));
  op(0x26, XOR, RDef(RD), RUse(RS), RUse(RT));
  op(0x27, NOR, RDef(RD), RUse(RS), RUse(RT));
  br(0x28, INVALID, Reserved);
  br(0x29, INVALID, Reserved);
  op(0x2a, SLT, RDef(RD), RUse(RS), RUse(RT));
  op(0x2b, SLTU, RDef(RD), RUse(RS), RUse(RT));
  op(0x2c, DADD, RDef(RD), RUse(RS), RUse(RT), Double, Overflow);
  op(0x2d, DADDU, RDef(RD), RUse(RS), RUse(RT), Double);
  op(0x2e, DSUB, RDef(RD), RUse(RS), RUse(RT), Double, Overflow);
  op(0x2f, DSUBU, RDef(RD), RUse(RS), RUse(RT), Double);
  op(0x30, TGE, RUse(RS), RUse(RT), Trap);
  op(0x31, TGEU, RUse(RS), RUse(RT), Trap);
  op(0x32, TLT, RUse(RS), RUse(RT), Trap);
  op(0x33, TLTU, RUse(RS), RUse(RT), Trap);
  op(0x34, TEQ, RUse(RS), RUse(RT), Trap);
  br(0x35, INVALID, Reserved);
  op(0x36, TNE, RUse(RS), RUse(RT), Trap);
  br(0x37, INVALID, Reserved);
  op(0x38, DSLL, RDef(RD), RUse(RT), Double);
  br(0x39, INVALID, Reserved);
  op(0x3a, DSRL, RDef(RD), RUse(RT), Double);
  op(0x3b, DSRA, RDef(RD), RUse(RT), Double);
  op(0x3c, DSLL32, RDef(RD), RUse(RT), Double);
  br(0x3d, INVALID, Reserved);
  op(0x3e, DSRL32, RDef(RD), RUse(RT), Double);
  op(0x3f, DSRA32, RDef(RD), RUse(RT), Double);
  }

  return {};
}

auto CPU::decoderREGIMM(u32 instruction) const -> OpInfo {
  switch(instruction >> 16 & 0x1f) {
  br(0x00, BLTZ, RUse(RS), Branch);
  br(0x01, BGEZ, RUse(RS), Branch);
  br(0x02, BLTZL, RUse(RS), Branch, Likely);
  br(0x03, BGEZL, RUse(RS), Branch, Likely);
  br(0x04, INVALID, Reserved);
  br(0x05, INVALID, Reserved);
  br(0x06, INVALID, Reserved);
  br(0x07, INVALID, Reserved);
  op(0x08, TGEI, RUse(RS), Trap);
  op(0x09, TGEIU, RUse(RS), Trap);
  op(0x0a, TLTI, RUse(RS), Trap);
  op(0x0b, TLTIU, RUse(RS), Trap);
  op(0x0c, TEQI, RUse(RS), Trap);
  br(0x0d, INVALID, Reserved);
  op(0x0e, TNEI, RUse(RS), Trap);
  br(0x0f, INVALID, Reserved);
  br(0x10, BLTZAL, RDef(31), RUse(RS), Branch);
  br(0x11, BGEZAL, RDef(31), RUse(RS), Branch);
  br(0x12, BLTZALL, RDef(31), RUse(RS), Branch, Likely);
  br(0x13, BGEZALL, RDef(31), RUse(RS), Branch, Likely);
  br(0x14, INVALID, Reserved);
  br(0x15, INVALID, Reserved);
  br(0x16, INVALID, Reserved);
  br(0x17, INVALID, Reserved);
  br(0x18, INVALID, Reserved);
  br(0x19, INVALID, Reserved);
  br(0x1a, INVALID, Reserved);
  br(0x1b, INVALID, Reserved);
  br(0x1c, INVALID, Reserved);
  br(0x1d, INVALID, Reserved);
  br(0x1e, INVALID, Reserved);
  br(0x1f, INVALID, Reserved);
  }

  return {};
}

auto CPU::decoderSCC(u32 instruction) const -> OpInfo {
  switch(instruction >> 21 & 0x1f) {
  op(0x00, MFC0, RDef(RT), Cop0);
  op(0x01, DMFC0, RDef(RT), Cop0, Double);
  br(0x02, CFC0, Cop0, Reserved);
  br(0x03, INVALID, Cop0, Reserved);
  op(0x04, MTC0, RUse(RT), Cop0);
  op(0x05, DMTC0, RUse(RT), Cop0, Double);
  br(0x06, CTC0, Cop0, Reserved);
  br(0x07, INVALID, Cop0, Reserved);
  br(0x08, BC0, Cop0, Reserved);  //TODO decode further
  br(0x09, INVALID, Cop0, Reserved);
  br(0x0a, INVALID, Cop0, Reserved);
  br(0x0b, INVALID, Cop0, Reserved);
  br(0x0c, INVALID, Cop0, Reserved);
  br(0x0d, INVALID, Cop0, Reserved);
  br(0x0e, INVALID, Cop0, Reserved);
  br(0x0f, INVALID, Cop0, Reserved);
  }

  switch(instruction & 0x3f) {
  op(0x01, TLBR, Cop0);
  op(0x02, TLBWI, Cop0);
  op(0x06, TLBWR, Cop0);
  op(0x08, TLBP, Cop0);
  br(0x10, INVALID, Cop0, Reserved);
  br(0x18, ERET, Cop0, Branch);
  }

  return {};
}

auto CPU::decoderFPU(u32 instruction) const -> OpInfo {
  switch(instruction >> 21 & 0x1f) {
  op(0x00, MFC1, RDef(RT), FUse(FS), Cop1);
  op(0x01, DMFC1, RDef(RT), FUse(FS), Cop1);  //Double?
  op(0x02, CFC1, RDef(RT), Cop1);
  br(0x03, DCFC1, RDef(RT), Cop1, Unimplemented);  //Double?
  op(0x04, MTC1, RUse(RT), FDef(FS), Cop1);
  op(0x05, DMTC1, RUse(RT), FDef(FS), Cop1);  //Double?
  op(0x06, CTC1, RUse(RT), Cop1);
  br(0x07, DCTC1, RUse(RT), Cop1, Unimplemented);  //Double?
  br(0x08, BC1, CFUse, Cop1, Branch, instruction >> 17 & 1 ? Likely : 0); // TODO decode more
  br(0x09, INVALID, Cop1, Reserved);  //Reserved or Unimplemented?
  br(0x0a, INVALID, Cop1, Reserved);
  br(0x0b, INVALID, Cop1, Reserved);
  br(0x0c, INVALID, Cop1, Reserved);
  br(0x0d, INVALID, Cop1, Reserved);
  br(0x0e, INVALID, Cop1, Reserved);
  br(0x0f, INVALID, Cop1, Reserved);
  }

  if((instruction >> 21 & 31) == 16)
  switch(instruction & 0x3f) {
  op(0x00, FADD_S, FDef(FD), FUse(FS), FUse(FT), Cop1);
  op(0x01, FSUB_S, FDef(FD), FUse(FS), FUse(FT), Cop1);
  op(0x02, FMUL_S, FDef(FD), FUse(FS), FUse(FT), Cop1);
  op(0x03, FDIV_S, FDef(FD), FUse(FS), FUse(FT), Cop1);
  op(0x04, FSQRT_S, FDef(FD), FUse(FS), Cop1);
  op(0x05, FABS_S, FDef(FD), FUse(FS), Cop1);
  op(0x06, FMOV_S, FDef(FD), FUse(FS), Cop1);
  op(0x07, FNEG_S, FDef(FD), FUse(FS), Cop1);
  op(0x08, FROUND_L_S, FDef(FD), FUse(FS), Cop1);  //Double?
  op(0x09, FTRUNC_L_S, FDef(FD), FUse(FS), Cop1);  //Double?
  op(0x0a, FCEIL_L_S, FDef(FD), FUse(FS), Cop1);  //Double?
  op(0x0b, FFLOOR_L_S, FDef(FD), FUse(FS), Cop1);  //Double?
  op(0x0c, FROUND_W_S, FDef(FD), FUse(FS), Cop1);
  op(0x0d, FTRUNC_W_S, FDef(FD), FUse(FS), Cop1);
  op(0x0e, FCEIL_W_S, FDef(FD), FUse(FS), Cop1);
  op(0x0f, FFLOOR_W_S, FDef(FD), FUse(FS), Cop1);
  br(0x20, FCVT_S_S, FDef(FD), FUse(FS), Cop1, Unimplemented);
  op(0x21, FCVT_D_S, FDef(FD), FUse(FS), Cop1);
  op(0x24, FCVT_W_S, FDef(FD), FUse(FS), Cop1);
  op(0x25, FCVT_L_S, FDef(FD), FUse(FS), Cop1);  //Double?
  op(0x30, FC_F_S, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x31, FC_UN_S, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x32, FC_EQ_S, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x33, FC_UEQ_S, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x34, FC_OLT_S, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x35, FC_ULT_S, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x36, FC_OLE_S, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x37, FC_ULE_S, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x38, FC_SF_S, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x39, FC_NGLE_S, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x3a, FC_SEQ_S, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x3b, FC_NGL_S, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x3c, FC_LT_S, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x3d, FC_NGE_S, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x3e, FC_LE_S, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x3f, FC_NGT_S, CFDef, FUse(FS), FUse(FT), Cop1);
  }

  if((instruction >> 21 & 31) == 17)
  switch(instruction & 0x3f) {
  op(0x00, FADD_D, FDef(FD), FUse(FS), FUse(FT), Cop1);
  op(0x01, FSUB_D, FDef(FD), FUse(FS), FUse(FT), Cop1);
  op(0x02, FMUL_D, FDef(FD), FUse(FS), FUse(FT), Cop1);
  op(0x03, FDIV_D, FDef(FD), FUse(FS), FUse(FT), Cop1);
  op(0x04, FSQRT_D, FDef(FD), FUse(FS), Cop1);
  op(0x05, FABS_D, FDef(FD), FUse(FS), Cop1);
  op(0x06, FMOV_D, FDef(FD), FUse(FS), Cop1);
  op(0x07, FNEG_D, FDef(FD), FUse(FS), Cop1);
  op(0x08, FROUND_L_D, FDef(FD), FUse(FS), Cop1);  //Double?
  op(0x09, FTRUNC_L_D, FDef(FD), FUse(FS), Cop1);  //Double?
  op(0x0a, FCEIL_L_D, FDef(FD), FUse(FS), Cop1);  //Double?
  op(0x0b, FFLOOR_L_D, FDef(FD), FUse(FS), Cop1);  //Double?
  op(0x0c, FROUND_W_D, FDef(FD), FUse(FS), Cop1);
  op(0x0d, FTRUNC_W_D, FDef(FD), FUse(FS), Cop1);
  op(0x0e, FCEIL_W_D, FDef(FD), FUse(FS), Cop1);
  op(0x0f, FFLOOR_W_D, FDef(FD), FUse(FS), Cop1);
  op(0x20, FCVT_S_D, FDef(FD), FUse(FS), Cop1);
  br(0x21, FCVT_D_D, FDef(FD), FUse(FS), Cop1, Unimplemented);
  op(0x24, FCVT_W_D, FDef(FD), FUse(FS), Cop1);
  op(0x25, FCVT_L_D, FDef(FD), FUse(FS), Cop1);  //Double?
  op(0x30, FC_F_D, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x31, FC_UN_D, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x32, FC_EQ_D, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x33, FC_UEQ_D, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x34, FC_OLT_D, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x35, FC_ULT_D, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x36, FC_OLE_D, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x37, FC_ULE_D, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x38, FC_SF_D, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x39, FC_NGLE_D, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x3a, FC_SEQ_D, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x3b, FC_NGL_D, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x3c, FC_LT_D, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x3d, FC_NGE_D, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x3e, FC_LE_D, CFDef, FUse(FS), FUse(FT), Cop1);
  op(0x3f, FC_NGT_D, CFDef, FUse(FS), FUse(FT), Cop1);
  }

  if((instruction >> 21 & 31) == 20)
  switch(instruction & 0x3f) {
  br(0x08, FROUND_L_W, FDef(FD), FUse(FS), Cop1, Unimplemented);  //Double?
  br(0x09, FTRUNC_L_W, FDef(FD), FUse(FS), Cop1, Unimplemented);  //Double?
  br(0x0a, FCEIL_L_W, FDef(FD), FUse(FS), Cop1, Unimplemented);  //Double?
  br(0x0b, FFLOOR_L_W, FDef(FD), FUse(FS), Cop1, Unimplemented);  //Double?
  br(0x0c, FROUND_W_W, FDef(FD), FUse(FS), Cop1, Unimplemented);
  br(0x0d, FTRUNC_W_W, FDef(FD), FUse(FS), Cop1, Unimplemented);
  br(0x0e, FCEIL_W_W, FDef(FD), FUse(FS), Cop1, Unimplemented);
  br(0x0f, FFLOOR_W_W, FDef(FD), FUse(FS), Cop1, Unimplemented);
  op(0x20, FCVT_S_W, FDef(FD), FUse(FS), Cop1);
  op(0x21, FCVT_D_W, FDef(FD), FUse(FS), Cop1);
  br(0x24, FCVT_W_W, FDef(FD), FUse(FS), Cop1, Unimplemented);
  br(0x25, FCVT_L_W, FDef(FD), FUse(FS), Cop1, Unimplemented);  //Double?
  }

  if((instruction >> 21 & 31) == 21)
  switch(instruction & 0x3f) {
  br(0x08, FROUND_L_L, FDef(FD), FUse(FS), Cop1, Unimplemented);
  br(0x09, FTRUNC_L_L, FDef(FD), FUse(FS), Cop1, Unimplemented);
  br(0x0a, FCEIL_L_L, FDef(FD), FUse(FS), Cop1, Unimplemented);
  br(0x0b, FFLOOR_L_L, FDef(FD), FUse(FS), Cop1, Unimplemented);
  br(0x0c, FROUND_W_L, FDef(FD), FUse(FS), Cop1, Unimplemented);
  br(0x0d, FTRUNC_W_L, FDef(FD), FUse(FS), Cop1, Unimplemented);
  br(0x0e, FCEIL_W_L, FDef(FD), FUse(FS), Cop1, Unimplemented);
  br(0x0f, FFLOOR_W_L, FDef(FD), FUse(FS), Cop1, Unimplemented);
  op(0x20, FCVT_S_L, FDef(FD), FUse(FS), Cop1);  //Double?
  op(0x21, FCVT_D_L, FDef(FD), FUse(FS), Cop1);  //Double?
  br(0x24, FCVT_W_L, FDef(FD), FUse(FS), Cop1, Unimplemented);
  br(0x25, FCVT_L_L, FDef(FD), FUse(FS), Cop1, Unimplemented);
  }

  //Unimplemented?

  return {};
}

auto CPU::decoderCOP2(u32 instruction) const -> OpInfo {
  switch(instruction >> 21 & 0x1f) {
  op(0x00, MFC2, RDef(RT), Cop2);
  op(0x01, DMFC2, RDef(RT), Cop2);  //Double?
  op(0x02, CFC2, RDef(RT), Cop2);
  br(0x03, INVALID, Cop2, Reserved);
  op(0x04, MTC2, RUse(RT), Cop2);
  op(0x05, DMTC2, RUse(RT), Cop2);  //Double?
  op(0x06, CTC2, RUse(RT), Cop2);
  br(0x07, INVALID, Cop2, Reserved);
  br(0x08, INVALID, Cop2, Reserved);
  br(0x09, INVALID, Cop2, Reserved);
  br(0x0a, INVALID, Cop2, Reserved);
  br(0x0b, INVALID, Cop2, Reserved);
  br(0x0c, INVALID, Cop2, Reserved);
  br(0x0d, INVALID, Cop2, Reserved);
  br(0x0e, INVALID, Cop2, Reserved);
  br(0x0f, INVALID, Cop2, Reserved);
  }

  return {};
}

#undef RUse
#undef RDef
#undef FUse
#undef FDef
#undef HiUse
#undef HiDef
#undef LoUse
#undef LoDef
#undef HiLoDef
#undef CFUse
#undef CFDef
#undef Load
#undef Store
#undef Branch
#undef Likely
#undef Overflow
#undef Breakpoint
#undef Double
#undef Syscall
#undef Cop0
#undef Cop1
#undef Cop2
#undef Trap
#undef Unimplemented
#undef Reserved

#undef RD
#undef RT
#undef RS
#undef FD
#undef FS
#undef FT

#undef jp
#undef op
#undef br
