/**
 * @file      mips_isa.cpp
 * @author    Sandro Rigo
 *            Marcus Bartholomeu
 *            Alexandro Baldassin (acasm information)
 *
 *            The ArchC Team
 *            http://www.archc.org/
 *
 *            Computer Systems Laboratory (LSC)
 *            IC-UNICAMP
 *            http://www.lsc.ic.unicamp.br/
 *
 * @version   1.0
 * @date      Mon, 19 Jun 2006 15:50:52 -0300
 * 
 * @brief     The ArchC i8051 functional model.
 * 
 * @attention Copyright (C) 2002-2006 --- The ArchC Team
 *
 */

#include  "mips_isa.H"
#include  "mips_isa_init.cpp"
#include  "mips_bhv_macros.H"
#include <iostream>
#include <vector>

//If you want debug information for this model, uncomment next line
//#define DEBUG_MODEL
#include "ac_debug_model.H"


//!User defined macros to reference registers.
#define Ra 31
#define Sp 29

// 'using namespace' statement to allow access to all
// mips-specific datatypes
using namespace mips_parms;

static int processors_started = 0;
#define DEFAULT_STACK_SIZE (256*1024)

// Create instructions history (stack)
typedef struct inst_hist_t {
  int type;
  int r1;
  int r2;
  int r3;
} inst_hist_t;
std::vector <inst_hist_t> history;

// Instruction types
enum inst_type {LD, WR, BR, OTHER};

// Branch prediction types
enum branch_pred_type {BTFNT, NOT_TAKEN, NONE};

/*
 * CONFIGURATIONS
 * - pipeline size
 * - branch predictor type
 * - super scalar
 * - generate traces
 */
int pipeline_size = 5;
int superscalar = 1;
branch_pred_type predictor = NONE;
int generate_traces = 0;

/*
 * COUNTERS
 * - data_stalls
 * - branch_stalls
 * - branch_calls
 * - branch_correct
 */
int instructions = 0;
int stalls = 0;
int branch_stalls = 0;
int branch_calls = 0;
int branch_correct = 0;

void saveInstruction(inst_type type, int r1, int r2, int r3) {
  
  instructions++;
	
  inst_hist_t a;
  a.r1 = r1;
  a.r2 = r2;
  a.r3 = r3;
  a.type = type;
  history.push_back(a);

  if(history.size() > pipeline_size) {
    history.erase(history.begin());
  }

  // Check for scalar
  if(!superscalar) {
    if(history[3].type == LD && (a.r2 == history[3].r1 || a.r3 == history[3].r1))
      stalls++;
      
    // Control Hazard
    // Consequecence of early evaluation of the branch decision in ID stage
    if(a.type == BR){
		// write followed by a branch testing the result
		if(history[3].type == WR && ( history[3].r1 == a.r1 || history[3].r1 == a.r2 ))
			stalls++;
		// load followed by a branch testing the result
		if(history[3].type == LD && ( history[3].r1 == a.r1 || history[3].r1 == a.r2 ))
			stalls+=2;
	}
  }
  else {
    // RAW hazard
    // Compare first pair with the first from the 2nd pair
    if(history[1].type == LD && (a.r2 == history[1].r1 || a.r3 == history[1].r1) ||
       history[2].type == LD && (a.r2 == history[2].r1 || a.r3 == history[2].r1))
          stalls++;

    // Compare first pair with the second from the 2nd pair
    else if(history[1].type == LD && (history[3].r2 == history[1].r1 || history[3].r3 == history[1].r1) ||
            history[2].type == LD && (history[3].r2 == history[2].r1 || history[3].r3 == history[2].r1)) 
    {
      stalls++;

      // Check between pairs
      if(history[3].type == LD && (a.r2 == history[3].r1 || a.r3 == history[3].r1))
        stalls++;
    }
    // Check only between pairs
    else if(history[3].type == LD && (a.r2 == history[3].r1 || a.r3 == history[3].r1))
      stalls += 2;

    // WAR hazard
    if(a.type == WR && (a.r1 == history[3].r2 || a.r1 == history[3].r3)) {
      stalls++;
    }

    // WAW hazard
    if(a.type == WR && history[3].type == WR && a.r1 == history[3].r1) {
      stalls++;
    }	
  }
}

/**
 * resultBranch:
 *     true: branch taken
 *     false: branch not taken
 * npc: new pc
 * cpc: current pc
  */
void countBranchStalls(bool resultBranch, int npc, int cpc){
	
	branch_calls++;
	if(resultBranch)
		branch_correct++;
	
	if(predictor == NOT_TAKEN && resultBranch == true){
		branch_stalls++; // with fowarding and more hardware changes
	}else if(predictor == NONE){
		branch_stalls++; // with fowarding and more hardware changes
	}else if(predictor == BTFNT){
		// backward branch and not taken
		if(npc < cpc && resultBranch == false){
			branch_stalls++;
		// foward branch and taken
		}else if(npc > cpc && resultBranch == true){
			branch_stalls++;
		}
	}
}

std::ofstream dineroTraceOutputFile;
enum DINERO_TYPE { READ, WRITE, INSTRUCTION_FETCH };

void writeTofile(DINERO_TYPE type, int address){
  dineroTraceOutputFile << type << " " << std::hex << address << endl;
}

void init(){
	if(generate_traces)
		dineroTraceOutputFile.open("/tmp/dineroTraceOutputFile.trace", std::ofstream::out | std::ofstream::trunc);
		
	instructions = 0;
	stalls = 0;
	branch_stalls = 0;
	branch_calls = 0;
	branch_correct = 0;
}

void print_config(){
  
  printf("\n\n----- Configurations -----\n");
  
  printf("Pipeline size: %d\n", pipeline_size);  
  
  printf("Superscalar: ");  
  if(superscalar) printf("true\n");  
  else printf("false\n");  
  
  printf("Branch predictor: ");  
  if(predictor == BTFNT)
	printf("BTFNT\n");
  else if(predictor == NOT_TAKEN)
    printf("Always not taken\n");
  else
    printf("None\n");

  printf("\nGenerate Traces: %d\n", generate_traces);
}

void print_result(){
  printf("\n\n----- Results -----\n");
  
  int cycles = instructions;

  if(superscalar)
    cycles = cycles/2;
  
  int total_stalls = stalls + branch_stalls;
  int total_cycles = cycles + total_stalls;
  float cpi = total_cycles / (float) instructions;
  
  printf("Cycles: %d\n", total_cycles);
  printf("Instructions: %d\n", instructions);
  printf("Stalls: %d\n", total_stalls);
  printf("-- Data: %d\n", stalls);
  printf("-- Branch: %d\n", branch_stalls);
  printf("CPI: %.2f\n", cpi);
  printf("Branch calls: %d\n", branch_calls);
  printf("Branch correct: %d\n", branch_correct);
   
  printf("\n\n\n");
}

//!Generic instruction behavior method.
void ac_behavior( instruction )
{ 
  dbg_printf("----- PC=%#x ----- %lld\n", (int) ac_pc, ac_instr_counter);
  //  dbg_printf("----- PC=%#x NPC=%#x ----- %lld\n", (int) ac_pc, (int)npc, ac_instr_counter);
#ifndef NO_NEED_PC_UPDATE
  ac_pc = npc;
  npc = ac_pc + 4;
#endif 
};
 
//! Instruction Format behavior methods.
void ac_behavior( Type_R ){}
void ac_behavior( Type_I ){}
void ac_behavior( Type_J ){}
 
//!Behavior called before starting simulation
void ac_behavior(begin)
{
  init();
	
  dbg_printf("@@@ begin behavior @@@\n");
  RB[0] = 0;
  npc = ac_pc + 4;

  // Is is not required by the architecture, but makes debug really easier
  for (int regNum = 0; regNum < 32; regNum ++)
    RB[regNum] = 0;
  hi = 0;
  lo = 0;

  RB[29] =  AC_RAM_END - 1024 - processors_started++ * DEFAULT_STACK_SIZE;


}

//!Behavior called after finishing simulation
void ac_behavior(end)
{
  dbg_printf("@@@ end behavior @@@\n");
  
  if(generate_traces)
	dineroTraceOutputFile.close();
  
  print_config();
  print_result();
}

//!Instruction lb behavior method.
void ac_behavior( lb )
{
  char byte;
  dbg_printf("lb r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
  byte = DM.read_byte(RB[rs]+ imm);
  RB[rt] = (ac_Sword)byte ;
  dbg_printf("Result = %#x\n", RB[rt]);
  writeTofile(READ, RB[rs]+ imm);
  saveInstruction(LD, rt, 0, 0);
};

//!Instruction lbu behavior method.
void ac_behavior( lbu )
{
  unsigned char byte;
  dbg_printf("lbu r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
  byte = DM.read_byte(RB[rs]+ imm);
  RB[rt] = byte ;
  dbg_printf("Result = %#x\n", RB[rt]);
  writeTofile(READ, RB[rs]+ imm);
  saveInstruction(LD, rt, 0, 0);
};

//!Instruction lh behavior method.
void ac_behavior( lh )
{
  short int half;
  dbg_printf("lh r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
  half = DM.read_half(RB[rs]+ imm);
  RB[rt] = (ac_Sword)half ;
  dbg_printf("Result = %#x\n", RB[rt]);
  writeTofile(READ, RB[rs]+ imm);
  saveInstruction(LD, rt, 0, 0);

};

//!Instruction lhu behavior method.
void ac_behavior( lhu )
{
  unsigned short int  half;
  half = DM.read_half(RB[rs]+ imm);
  RB[rt] = half ;
  dbg_printf("Result = %#x\n", RB[rt]);
  writeTofile(READ, RB[rs]+ imm);
  saveInstruction(LD, rt, 0, 0);
};

//!Instruction lw behavior method.
void ac_behavior( lw )
{
  dbg_printf("lw r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
  RB[rt] = DM.read(RB[rs]+ imm);
  dbg_printf("Result = %#x\n", RB[rt]);
  writeTofile(READ, RB[rs]+ imm);
  saveInstruction(LD, rt, 0, 0);
};

//!Instruction lwl behavior method.
void ac_behavior( lwl )
{
  dbg_printf("lwl r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
  unsigned int addr, offset;
  ac_Uword data;

  addr = RB[rs] + imm;
  offset = (addr & 0x3) * 8;
  data = DM.read(addr & 0xFFFFFFFC);
  data <<= offset;
  data |= RB[rt] & ((1<<offset)-1);
  RB[rt] = data;
  dbg_printf("Result = %#x\n", RB[rt]);
  writeTofile(READ, addr & 0xFFFFFFFC);
  saveInstruction(LD, rt, 0, 0);

};

//!Instruction lwr behavior method.
void ac_behavior( lwr )
{
  dbg_printf("lwr r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
  unsigned int addr, offset;
  ac_Uword data;

  addr = RB[rs] + imm;
  offset = (3 - (addr & 0x3)) * 8;
  data = DM.read(addr & 0xFFFFFFFC);
  data >>= offset;
  data |= RB[rt] & (0xFFFFFFFF << (32-offset));
  RB[rt] = data;
  dbg_printf("Result = %#x\n", RB[rt]);
  writeTofile(READ, addr & 0xFFFFFFFC);
  saveInstruction(LD, rt, 0, 0);
};

//!Instruction sb behavior method.
void ac_behavior( sb )
{
  unsigned char byte;
  dbg_printf("sb r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
  byte = RB[rt] & 0xFF;
  DM.write_byte(RB[rs] + imm, byte);
  dbg_printf("Result = %#x\n", (int) byte);
  writeTofile(WRITE, RB[rs] + imm);
  saveInstruction(WR, 0, rt, 0);
};

//!Instruction sh behavior method.
void ac_behavior( sh )
{
  unsigned short int half;
  dbg_printf("sh r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
  half = RB[rt] & 0xFFFF;
  DM.write_half(RB[rs] + imm, half);
  dbg_printf("Result = %#x\n", (int) half);
  writeTofile(WRITE, RB[rs] + imm);
  saveInstruction(WR, 0, rt, 0);
};

//!Instruction sw behavior method.
void ac_behavior( sw )
{
  dbg_printf("sw r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
  DM.write(RB[rs] + imm, RB[rt]);
  dbg_printf("Result = %#x\n", RB[rt]);
  writeTofile(WRITE, RB[rs] + imm);
  saveInstruction(WR, 0, rt, 0);
};

//!Instruction swl behavior method.
void ac_behavior( swl )
{
  dbg_printf("swl r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
  unsigned int addr, offset;
  ac_Uword data;

  addr = RB[rs] + imm;
  offset = (addr & 0x3) * 8;
  data = RB[rt];
  data >>= offset;
  data |= DM.read(addr & 0xFFFFFFFC) & (0xFFFFFFFF << (32-offset));
  DM.write(addr & 0xFFFFFFFC, data);
  dbg_printf("Result = %#x\n", data);
  writeTofile(WRITE, addr & 0xFFFFFFFC);
  saveInstruction(WR, 0, rt, 0);
};

//!Instruction swr behavior method.
void ac_behavior( swr )
{
  dbg_printf("swr r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
  unsigned int addr, offset;
  ac_Uword data;

  addr = RB[rs] + imm;
  offset = (3 - (addr & 0x3)) * 8;
  data = RB[rt];
  data <<= offset;
  data |= DM.read(addr & 0xFFFFFFFC) & ((1<<offset)-1);
  DM.write(addr & 0xFFFFFFFC, data);
  dbg_printf("Result = %#x\n", data);
  writeTofile(WRITE, addr & 0xFFFFFFFC);
  saveInstruction(WR, 0, rt, 0);
};

//!Instruction addi behavior method.
void ac_behavior( addi )
{
  dbg_printf("addi r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
  RB[rt] = RB[rs] + imm;
  dbg_printf("Result = %#x\n", RB[rt]);
  //Test overflow
  if ( ((RB[rs] & 0x80000000) == (imm & 0x80000000)) &&
       ((imm & 0x80000000) != (RB[rt] & 0x80000000)) ) {
    fprintf(stderr, "EXCEPTION(addi): integer overflow.\n"); exit(EXIT_FAILURE);
  }
  saveInstruction(WR, rt, rs, 0);
};

//!Instruction addiu behavior method.
void ac_behavior( addiu )
{
  dbg_printf("addiu r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
  RB[rt] = RB[rs] + imm;
  dbg_printf("Result = %#x\n", RB[rt]);
  saveInstruction(WR, rt, rs, 0);
};

//!Instruction slti behavior method.
void ac_behavior( slti )
{
  dbg_printf("slti r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
  // Set the RD if RS< IMM
  if( (ac_Sword) RB[rs] < (ac_Sword) imm )
    RB[rt] = 1;
  // Else reset RD
  else
    RB[rt] = 0;
  dbg_printf("Result = %#x\n", RB[rt]);
  saveInstruction(WR, rt, rs, 0);
};

//!Instruction sltiu behavior method.
void ac_behavior( sltiu )
{
  dbg_printf("sltiu r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
  // Set the RD if RS< IMM
  if( (ac_Uword) RB[rs] < (ac_Uword) imm )
    RB[rt] = 1;
  // Else reset RD
  else
    RB[rt] = 0;
  dbg_printf("Result = %#x\n", RB[rt]);
  saveInstruction(WR, rt, rs, 0);
};

//!Instruction andi behavior method.
void ac_behavior( andi )
{ 
  dbg_printf("andi r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
  RB[rt] = RB[rs] & (imm & 0xFFFF) ;
  dbg_printf("Result = %#x\n", RB[rt]);
  saveInstruction(WR, rt, rs, 0);
};

//!Instruction ori behavior method.
void ac_behavior( ori )
{ 
  dbg_printf("ori r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
  RB[rt] = RB[rs] | (imm & 0xFFFF) ;
  dbg_printf("Result = %#x\n", RB[rt]);
  saveInstruction(WR, rt, rs, 0);
};

//!Instruction xori behavior method.
void ac_behavior( xori )
{ 
  dbg_printf("xori r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
  RB[rt] = RB[rs] ^ (imm & 0xFFFF) ;
  dbg_printf("Result = %#x\n", RB[rt]);
  saveInstruction(WR, rt, rs, 0);
};

//!Instruction lui behavior method.
void ac_behavior( lui )
{ 
  dbg_printf("lui r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
  // Load a constant in the upper 16 bits of a register
  // To achieve the desired behaviour, the constant was shifted 16 bits left
  // and moved to the target register ( rt )
  RB[rt] = imm << 16;
  dbg_printf("Result = %#x\n", RB[rt]);
  saveInstruction(LD, rt, 0, 0);
};

//!Instruction add behavior method.
void ac_behavior( add )
{
  dbg_printf("add r%d, r%d, r%d\n", rd, rs, rt);
  RB[rd] = RB[rs] + RB[rt];
  dbg_printf("Result = %#x\n", RB[rd]);
  //Test overflow
  if ( ((RB[rs] & 0x80000000) == (RB[rd] & 0x80000000)) &&
       ((RB[rd] & 0x80000000) != (RB[rt] & 0x80000000)) ) {
    fprintf(stderr, "EXCEPTION(add): integer overflow.\n"); exit(EXIT_FAILURE);
  }
  saveInstruction(WR, rd, rs, rt);
};

//!Instruction addu behavior method.
void ac_behavior( addu )
{
  dbg_printf("addu r%d, r%d, r%d\n", rd, rs, rt);
  RB[rd] = RB[rs] + RB[rt];
  //cout << "  RS: " << (unsigned int)RB[rs] << " RT: " << (unsigned int)RB[rt] << endl;
  //cout << "  Result =  " <<  (unsigned int)RB[rd] <<endl;
  dbg_printf("Result = %#x\n", RB[rd]);
  saveInstruction(WR, rd, rs, rt);
};

//!Instruction sub behavior method.
void ac_behavior( sub )
{
  dbg_printf("sub r%d, r%d, r%d\n", rd, rs, rt);
  RB[rd] = RB[rs] - RB[rt];
  dbg_printf("Result = %#x\n", RB[rd]);
  //TODO: test integer overflow exception for sub
  saveInstruction(WR, rd, rs, rt);
};

//!Instruction subu behavior method.
void ac_behavior( subu )
{
  dbg_printf("subu r%d, r%d, r%d\n", rd, rs, rt);
  RB[rd] = RB[rs] - RB[rt];
  dbg_printf("Result = %#x\n", RB[rd]);
  saveInstruction(WR, rd, rs, rt);
};

//!Instruction slt behavior method.
void ac_behavior( slt )
{ 
  dbg_printf("slt r%d, r%d, r%d\n", rd, rs, rt);
  // Set the RD if RS< RT
  if( (ac_Sword) RB[rs] < (ac_Sword) RB[rt] )
    RB[rd] = 1;
  // Else reset RD
  else
    RB[rd] = 0;
  dbg_printf("Result = %#x\n", RB[rd]);
  saveInstruction(WR, rd, rs, rt);
};

//!Instruction sltu behavior method.
void ac_behavior( sltu )
{
  dbg_printf("sltu r%d, r%d, r%d\n", rd, rs, rt);
  // Set the RD if RS < RT
  if( RB[rs] < RB[rt] )
    RB[rd] = 1;
  // Else reset RD
  else
    RB[rd] = 0;
  dbg_printf("Result = %#x\n", RB[rd]);
  saveInstruction(WR, rd, rs, rt);
};

//!Instruction instr_and behavior method.
void ac_behavior( instr_and )
{
  dbg_printf("instr_and r%d, r%d, r%d\n", rd, rs, rt);
  RB[rd] = RB[rs] & RB[rt];
  dbg_printf("Result = %#x\n", RB[rd]);
  saveInstruction(WR, rd, rs, rt);
};

//!Instruction instr_or behavior method.
void ac_behavior( instr_or )
{
  dbg_printf("instr_or r%d, r%d, r%d\n", rd, rs, rt);
  RB[rd] = RB[rs] | RB[rt];
  dbg_printf("Result = %#x\n", RB[rd]);
  saveInstruction(WR, rd, rs, rt);
};

//!Instruction instr_xor behavior method.
void ac_behavior( instr_xor )
{
  dbg_printf("instr_xor r%d, r%d, r%d\n", rd, rs, rt);
  RB[rd] = RB[rs] ^ RB[rt];
  dbg_printf("Result = %#x\n", RB[rd]);
  saveInstruction(WR, rd, rs, rt);
};

//!Instruction instr_nor behavior method.
void ac_behavior( instr_nor )
{
  dbg_printf("nor r%d, r%d, r%d\n", rd, rs, rt);
  RB[rd] = ~(RB[rs] | RB[rt]);
  dbg_printf("Result = %#x\n", RB[rd]);
  saveInstruction(WR, rd, rs, rt);
};

//!Instruction nop behavior method.
void ac_behavior( nop )
{  
  dbg_printf("nop\n");
};

//!Instruction sll behavior method.
void ac_behavior( sll )
{  
  dbg_printf("sll r%d, r%d, %d\n", rd, rs, shamt);
  RB[rd] = RB[rt] << shamt;
  dbg_printf("Result = %#x\n", RB[rd]);
  saveInstruction(WR, rd, rt, 0);
};

//!Instruction srl behavior method.
void ac_behavior( srl )
{
  dbg_printf("srl r%d, r%d, %d\n", rd, rs, shamt);
  RB[rd] = RB[rt] >> shamt;
  dbg_printf("Result = %#x\n", RB[rd]);
  saveInstruction(WR, rd, rt, 0);
};

//!Instruction sra behavior method.
void ac_behavior( sra )
{
  dbg_printf("sra r%d, r%d, %d\n", rd, rs, shamt);
  RB[rd] = (ac_Sword) RB[rt] >> shamt;
  dbg_printf("Result = %#x\n", RB[rd]);
  saveInstruction(WR, rd, rt, 0);
};

//!Instruction sllv behavior method.
void ac_behavior( sllv )
{
  dbg_printf("sllv r%d, r%d, r%d\n", rd, rt, rs);
  RB[rd] = RB[rt] << (RB[rs] & 0x1F);
  dbg_printf("Result = %#x\n", RB[rd]);
  saveInstruction(WR, rd, rt, rs);
};

//!Instruction srlv behavior method.
void ac_behavior( srlv )
{
  dbg_printf("srlv r%d, r%d, r%d\n", rd, rt, rs);
  RB[rd] = RB[rt] >> (RB[rs] & 0x1F);
  dbg_printf("Result = %#x\n", RB[rd]);
  saveInstruction(WR, rd, rt, rs);
};

//!Instruction srav behavior method.
void ac_behavior( srav )
{
  dbg_printf("srav r%d, r%d, r%d\n", rd, rt, rs);
  RB[rd] = (ac_Sword) RB[rt] >> (RB[rs] & 0x1F);
  dbg_printf("Result = %#x\n", RB[rd]);
  saveInstruction(WR, rd, rt, rs);
};

//!Instruction mult behavior method.
void ac_behavior( mult )
{
  dbg_printf("mult r%d, r%d\n", rs, rt);

  long long result;
  int half_result;

  result = (ac_Sword) RB[rs];
  result *= (ac_Sword) RB[rt];

  half_result = (result & 0xFFFFFFFF);
  // Register LO receives 32 less significant bits
  lo = half_result;

  half_result = ((result >> 32) & 0xFFFFFFFF);
  // Register HI receives 32 most significant bits
  hi = half_result ;

  dbg_printf("Result = %#llx\n", result);
  saveInstruction(WR, 0, rs, rt);
};

//!Instruction multu behavior method.
void ac_behavior( multu )
{
  dbg_printf("multu r%d, r%d\n", rs, rt);

  unsigned long long result;
  unsigned int half_result;

  result  = RB[rs];
  result *= RB[rt];

  half_result = (result & 0xFFFFFFFF);
  // Register LO receives 32 less significant bits
  lo = half_result;

  half_result = ((result>>32) & 0xFFFFFFFF);
  // Register HI receives 32 most significant bits
  hi = half_result ;

  dbg_printf("Result = %#llx\n", result);
  saveInstruction(WR, 0, rs, rt);
};

//!Instruction div behavior method.
void ac_behavior( div )
{
  dbg_printf("div r%d, r%d\n", rs, rt);
  // Register LO receives quotient
  lo = (ac_Sword) RB[rs] / (ac_Sword) RB[rt];
  // Register HI receives remainder
  hi = (ac_Sword) RB[rs] % (ac_Sword) RB[rt];
  saveInstruction(WR, 0, rs, rt);
};

//!Instruction divu behavior method.
void ac_behavior( divu )
{
  dbg_printf("divu r%d, r%d\n", rs, rt);
  // Register LO receives quotient
  lo = RB[rs] / RB[rt];
  // Register HI receives remainder
  hi = RB[rs] % RB[rt];
  saveInstruction(WR, 0, rs, rt);
};

//!Instruction mfhi behavior method.
void ac_behavior( mfhi )
{
  dbg_printf("mfhi r%d\n", rd);
  RB[rd] = hi;
  dbg_printf("Result = %#x\n", RB[rd]);
  saveInstruction(WR, rd, 0, 0);
};

//!Instruction mthi behavior method.
void ac_behavior( mthi )
{
  dbg_printf("mthi r%d\n", rs);
  hi = RB[rs];
  dbg_printf("Result = %#x\n", (unsigned int) hi);
  saveInstruction(WR, 0, rs, 0);
};

//!Instruction mflo behavior method.
void ac_behavior( mflo )
{
  dbg_printf("mflo r%d\n", rd);
  RB[rd] = lo;
  dbg_printf("Result = %#x\n", RB[rd]);
  saveInstruction(WR, rd, 0, 0);
};

//!Instruction mtlo behavior method.
void ac_behavior( mtlo )
{
  dbg_printf("mtlo r%d\n", rs);
  lo = RB[rs];
  dbg_printf("Result = %#x\n", (unsigned int) lo);
  saveInstruction(WR, 0, rs, 0);
};

//!Instruction j behavior method.
void ac_behavior( j )
{
  dbg_printf("j %d\n", addr);
  addr = addr << 2;
#ifndef NO_NEED_PC_UPDATE
  npc =  (ac_pc & 0xF0000000) | addr;
#endif 
  dbg_printf("Target = %#x\n", (ac_pc & 0xF0000000) | addr );
  saveInstruction(OTHER, 0, 0, 0);
};

//!Instruction jal behavior method.
void ac_behavior( jal )
{
  dbg_printf("jal %d\n", addr);
  // Save the value of PC + 8 (return address) in $ra ($31) and
  // jump to the address given by PC(31...28)||(addr<<2)
  // It must also flush the instructions that were loaded into the pipeline
  RB[Ra] = ac_pc+4; //ac_pc is pc+4, we need pc+8
  
  addr = addr << 2;
#ifndef NO_NEED_PC_UPDATE
  npc = (ac_pc & 0xF0000000) | addr;
#endif 
  
  dbg_printf("Target = %#x\n", (ac_pc & 0xF0000000) | addr );
  dbg_printf("Return = %#x\n", ac_pc+4);
  saveInstruction(OTHER, 0, 0, 0);
};

//!Instruction jr behavior method.
void ac_behavior( jr )
{
  dbg_printf("jr r%d\n", rs);
  // Jump to the address stored on the register reg[RS]
  // It must also flush the instructions that were loaded into the pipeline
#ifndef NO_NEED_PC_UPDATE
  npc = RB[rs], 1;
#endif 
  dbg_printf("Target = %#x\n", RB[rs]);
  saveInstruction(OTHER, 0, 0, 0);
};

//!Instruction jalr behavior method.
void ac_behavior( jalr )
{
  dbg_printf("jalr r%d, r%d\n", rd, rs);
  // Save the value of PC + 8(return address) in rd and
  // jump to the address given by [rs]

#ifndef NO_NEED_PC_UPDATE
  npc = RB[rs], 1;
#endif 
  dbg_printf("Target = %#x\n", RB[rs]);

  if( rd == 0 )  //If rd is not defined use default
    rd = Ra;
  RB[rd] = ac_pc+4;
  dbg_printf("Return = %#x\n", ac_pc+4);
  saveInstruction(OTHER, 0, 0, 0);
}; 

//!Instruction beq behavior method.
void ac_behavior( beq )
{
  dbg_printf("beq r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
  if( RB[rs] == RB[rt] ){
#ifndef NO_NEED_PC_UPDATE
    npc = ac_pc + (imm<<2);
#endif 
    dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
    countBranchStalls(true, npc, ac_pc);
  }else{
	  countBranchStalls(false, npc, ac_pc);
  }
  
  saveInstruction(BR, 0, 0, 0);
};

//!Instruction bne behavior method.
void ac_behavior( bne )
{ 
  dbg_printf("bne r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
  if( RB[rs] != RB[rt] ){
#ifndef NO_NEED_PC_UPDATE
    npc = ac_pc + (imm<<2);
#endif 
    dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
    countBranchStalls(true, npc, ac_pc);
  }else{
	  countBranchStalls(false, npc, ac_pc);
  }
  saveInstruction(BR, 0, 0, 0);
};

//!Instruction blez behavior method.
void ac_behavior( blez )
{
  dbg_printf("blez r%d, %d\n", rs, imm & 0xFFFF);
  if( (RB[rs] == 0 ) || (RB[rs]&0x80000000 ) ){
#ifndef NO_NEED_PC_UPDATE
    npc = ac_pc + (imm<<2), 1;
#endif 
    dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
    countBranchStalls(true, npc, ac_pc);
  }else{
	  countBranchStalls(false, npc, ac_pc);
  }
  saveInstruction(BR, 0, 0, 0);
};

//!Instruction bgtz behavior method.
void ac_behavior( bgtz )
{
  dbg_printf("bgtz r%d, %d\n", rs, imm & 0xFFFF);
  if( !(RB[rs] & 0x80000000) && (RB[rs]!=0) ){
#ifndef NO_NEED_PC_UPDATE
    npc = ac_pc + (imm<<2);
#endif 
    dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
    countBranchStalls(true, npc, ac_pc);
  }else{
	  countBranchStalls(false, npc, ac_pc);
  }
  saveInstruction(BR, 0, 0, 0);
};

//!Instruction bltz behavior method.
void ac_behavior( bltz )
{
  dbg_printf("bltz r%d, %d\n", rs, imm & 0xFFFF);
  if( RB[rs] & 0x80000000 ){
#ifndef NO_NEED_PC_UPDATE
    npc = ac_pc + (imm<<2);
#endif 
    dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
    countBranchStalls(true, npc, ac_pc);
  }else{
	  countBranchStalls(false, npc, ac_pc);
  }
  saveInstruction(BR, 0, 0, 0);
};

//!Instruction bgez behavior method.
void ac_behavior( bgez )
{
  dbg_printf("bgez r%d, %d\n", rs, imm & 0xFFFF);
  if( !(RB[rs] & 0x80000000) ){
#ifndef NO_NEED_PC_UPDATE
    npc = ac_pc + (imm<<2);
#endif 
    dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
    countBranchStalls(true, npc, ac_pc);
  }else{
	  countBranchStalls(false, npc, ac_pc);
  }
  saveInstruction(BR, 0, 0, 0);
};

//!Instruction bltzal behavior method.
void ac_behavior( bltzal )
{
  dbg_printf("bltzal r%d, %d\n", rs, imm & 0xFFFF);
  RB[Ra] = ac_pc+4; //ac_pc is pc+4, we need pc+8
  if( RB[rs] & 0x80000000 ){
#ifndef NO_NEED_PC_UPDATE
    npc = ac_pc + (imm<<2);
#endif 
    dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
    countBranchStalls(true, npc, ac_pc);
  }else{
	  countBranchStalls(false, npc, ac_pc);
  }
  dbg_printf("Return = %#x\n", ac_pc+4);
  saveInstruction(BR, 0, 0, 0);
};

//!Instruction bgezal behavior method.
void ac_behavior( bgezal )
{
  dbg_printf("bgezal r%d, %d\n", rs, imm & 0xFFFF);
  RB[Ra] = ac_pc+4; //ac_pc is pc+4, we need pc+8
  if( !(RB[rs] & 0x80000000) ){
#ifndef NO_NEED_PC_UPDATE
    npc = ac_pc + (imm<<2);
#endif 
    dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
    countBranchStalls(true, npc, ac_pc);
  }else{
	  countBranchStalls(false, npc, ac_pc);
  }
  dbg_printf("Return = %#x\n", ac_pc+4);
  saveInstruction(BR, 0, 0, 0);
};

//!Instruction sys_call behavior method.
void ac_behavior( sys_call )
{
  dbg_printf("syscall\n");
  stop();
}

//!Instruction instr_break behavior method.
void ac_behavior( instr_break )
{
  fprintf(stderr, "instr_break behavior not implemented.\n"); 
  exit(EXIT_FAILURE);
}
