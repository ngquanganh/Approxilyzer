#include "instruction.h"
#include "common_functions.h"
#define DEBUG_PARSER 0
#define DEBUG_PRUNING 0
#define DEBUG 0

using namespace std;

/* Functions of instruction class */

instruction_t::instruction_t() {
	is_function_name= false;
	function_name = "";
	c_line_frequency = 0;
	pruned_c_line_frequency = 0;
	equivalent_pc = "";
	c_line_number = 0;
	asm_line_number = 0;
	program_counter = "";
	opcode = "";
	def = "";
	dep_type = -1;
	leading_instruction_pc = "";
	leading_index = -1;
	program_name = "";
	app_name = "";
	bb = NULL;
	for(int i=0; i<NUM_OPS; i++) 
		op[i] = "";
}

//constructor for normal instruction 
instruction_t::instruction_t(string program_name, string app_name, int c_line_frequency, int c_line_number, int asm_line_number, string program_counter, string opcode, string *op, vector<string> p_list, vector<string> r_list) {
	this->is_function_name = false;
	this->c_line_frequency = c_line_frequency;
	this->pruned_c_line_frequency = c_line_frequency;
	this->equivalent_pc = "";
	this->c_line_number = c_line_number;
	this->asm_line_number = asm_line_number;
	this->function_name = "";
	this->program_counter = program_counter;
	this->opcode = opcode;
	this->leading_instruction_pc = "";
	this->leading_index= -1;
	this->dep_type = -1;

	this->program_name = program_name; 
	this->app_name = app_name ;
	this->bb = NULL;

	for(int i=0; i<NUM_OPS; i++) 
		this->op[i] = op[i];

	for(int i=0; i<p_list.size(); i++)
		params_list.push_back(p_list[i]);

	for(int i=0; i<r_list.size(); i++)
		ret_val_list.push_back(r_list[i]);

	for(int i=NUM_OPS-1; i>=0; i--) {
		if(op[i].compare("BLANK") != 0) {
            // add an is_fp_op(), include it here
			if(is_arithmetic_op() || is_logical_op() || is_load_op() || is_sethi_op() || is_convert_fp_op() || is_mov_op() || is_clr_reg()) {
				this->def = op[i];
			} else {
				this->def = "";
			}
			break;
		}
	}

	 //if(program_counter.compare("0x10000ba2c") == 0) {
	 //	cout << program_counter << ":" << get_def() << endl;
	 //}
}

//constructor for label
instruction_t::instruction_t(string program_name, string app_name, int c_line_frequency, int c_line_number, int asm_line_number, string func_name) {
	this->is_function_name = true;
	this->c_line_frequency = c_line_frequency;
	this->pruned_c_line_frequency = c_line_frequency;
	this->equivalent_pc = "";
	this->c_line_number = c_line_number;
	this->asm_line_number = asm_line_number;
	this->function_name = func_name;
	leading_instruction_pc = "";
	leading_index = -1;
	this->dep_type = -1;
	opcode = "";
	program_counter = "";
	this->program_name = program_name; 
	this->app_name = app_name ;
	this->bb = NULL;

	for(int i=0; i<NUM_OPS; i++) {
		op[i] = "";
	}
}

void instruction_t::print_instruction() {
	if(is_function_name) { 
		cout << c_line_frequency << ":" << c_line_number << ":" << asm_line_number << ":" << function_name << endl; 
	} else {
		cout << program_counter << ":" << c_line_frequency << ":" << c_line_number << ":" << asm_line_number << ":" << opcode; 
		for(int i=0; i<NUM_OPS; i++) {
			cout << " " << op[i];
		}
		cout << endl;
	}
}

void instruction_t::make_nop() {
	is_function_name = false;
	opcode = "nop";
	dep_type = -1;
	for(int i=0; i<NUM_OPS; i++) 
		op[i] = "BLANK";
	def = "";
}

/***********************************
 * THUMB MEMORY ACCESS INSTRUCTION *
 ***********************************/
bool instruction_t::is_load_op() {
	if(is_function_name) 
		return false;
	if(opcode.substr(0,3).compare("LDR") == 0) 
        return true;
    if(opcode.substr(0,4).compare("LDRH") == 0) 
        return true;
    if(opcode.substr(0,5).compare("LDRSH") == 0) 
        return true;
    if(opcode.substr(0,4).compare("LDRB") == 0) 
        return true;
	if(opcode.substr(0,5).compare("LDRSB") == 0) 
        return true;
    if(opcode.substr(0,5).compare("LDMIA") == 0) 
        return true;
	return false;
}

bool instruction_t::is_store_op() {
	if(is_function_name) 
		return false;
	if(opcode.substr(0,3).compare("STR") == 0) 
		return true;
	if(opcode.substr(0,4).compare("STRH") == 0) 
        return true;
	if(opcode.substr(0,5).compare("STRB") == 0) 
        return true;
    if(opcode.substr(0,5).compare("STMIA") == 0) 
        return true;
	return false;
}

bool instruction_t::is_push_op() {
	if(is_function_name) 
		return false;
	if(opcode.substr(0,4).compare("PUSH") == 0) 
		return true;
	return false;
}

bool instruction_t::is_pop_op() {
	if(is_function_name) 
		return false;
	if(opcode.substr(0,3).compare("POP") == 0) 
		return true;
	return false;
}

bool instruction_t::is_memory_op() {
	if(is_load_op() || is_store_op() || is_push_op() || is_pop_op()) 
		return true;
	return false;
}

/********************************
 * THUMB ARITHMETIC INSTRUCTION *
 ********************************/



bool instruction_t::is_shift_op() {
	if(is_function_name) 
		return false;
	// Arithmetic Shift Right
	if(opcode.substr(0,3).compare("ASR") == 0) 
		return true;
	// Logical Shift Left
	if(opcode.substr(0,3).compare("LSL") == 0) 
		return true;
	// Logical Shift Right
	if(opcode.substr(0,3).compare("LSR") == 0) 
		return true;
	// Rotate Right 
	if(opcode.substr(0,3).compare("ROR") == 0) 
		return true;
	return false;
}

bool instruction_t::is_test_op() 
{
	if (is_function_name)
		return false;
	if (opcode.substr(0,3).compare("TST") == 0)
		return true;
	return false;
}


//do bic (logical bit clear) count as branch?? 
bool instruction_t::is_branch_op() {
	if(is_function_name) 
		return false;
	if(opcode.substr(0,1).compare("B") == 0) 
		return true;
	if(opcode.substr(0,2).compare("BL") == 0) 
		return true;
	if(opcode.substr(0,2).compare("BX") == 0) 
		return true;
	if(opcode.substr(0,3).compare("BLX") == 0) 
		return true;
	if(opcode.substr(0,3).compare("BXJ") == 0)
		return true;
	if(opcode.substr(0,3).compare("BIC") == 0) // put this here for now because the sparc version has it 
		return true;

	return false;
}

bool instruction_t::is_software_interrupt_op() {
	if (is_function_name)
		return false;
	if (opcode.substr(0,3).compare("SWI") == 0)
		return true;
	return false;
}

bool instruction_t::is_breakpoint_op() {
	if (is_function_name)
		return false;
	if (opcode.substr(0,4).compare("BKPT") == 0)
		return true;
	return false;
}

bool instruction_t::is_jump(){
	if(is_function_name) 
		return false;
	if(opcode.compare("jmp") == 0) 
		return true;
	if(opcode.compare("jmpl") == 0) 
		return true;
	return false;
}
bool instruction_t::is_jmpl(){
	if(is_function_name) 
		return false;
	if(opcode.compare("jmpl") == 0) 
		return true;
	return false;
}

bool instruction_t::is_branch_with_annul_bit() {
	if(is_branch_op()) {
		int found = opcode.find(",a") ;
		if(found != string::npos) { 
			return true;
		}
	}
	return false;
}

bool instruction_t::is_branch_always() {
	if(is_function_name) 
		return false;

	size_t fi = opcode.find_first_of(",");
	if(fi == string::npos) {
		// return true for branch always or branch never
		if(opcode.compare("B") == 0) 
			return true;
		if(opcode.compare("BL") == 0) 
			return true;
		if(opcode.compare("BLX") == 0) 
			return true;
		//if(opcode.compare("fbn") == 0) 
		//	return true;
	} else {
		if(opcode.substr(0,fi+1).compare("ba") == 0) 
			return true;
		if(opcode.substr(0,fi+1).compare("bn") == 0) 
			return true;
		if(opcode.substr(0,fi+1).compare("fba") == 0) 
			return true;
		if(opcode.substr(0,fi+1).compare("fbn") == 0) 
			return true;
	}
	return false;
}

bool instruction_t::is_branch_conditional() {
	if(is_function_name) 
		return false;
	if(is_branch_op() && !is_branch_always()) 
		return true;
	return false;
}

bool instruction_t::is_clr() {
	if(is_function_name) 
		return false;
	if(opcode.substr(0,3).compare("clr") == 0) 
		return true;
	return false;
}

bool instruction_t::is_clr_reg() {
	if(is_function_name) 
		return false;
	if(opcode.substr(0,3).compare("clr") == 0) 
		if(is_register(get_op(0))) 
			return true;
	return false;
}

bool instruction_t::is_clr_mem() {
	if(is_function_name) 
		return false;
	if(opcode.substr(0,3).compare("clr") == 0) 
		if(!is_register(get_op(0)))
			return true;
	return false;
}

bool instruction_t::is_conditional_op() {
	if(is_function_name) 
		return false;
	if(opcode.substr(0,4).compare("movr") == 0) 
		return true;
	if(is_movcc_op()) 
		return true;
	return false;
}

bool instruction_t::is_orcc() {
	if(is_function_name) 
		return false;
	if(opcode.compare("orcc") == 0) 
		return true;
	return false;
}
bool instruction_t::is_add() {
	if(is_function_name) 
		return false;
	if(opcode.compare("add") == 0) 
		return true;
	return false;
}
bool instruction_t::is_addc() {
	if (is_function_name)
		return false;
	if (opcode.compare("addc") == 0)
		return true;
	return false;
}
	
bool instruction_t::is_compare_op() {
	if(is_function_name) 
		return false;
	if(opcode.substr(0,3).compare("CMP") == 0) 
		return true;
	if(opcode.substr(0,3).compare("CMN") == 0) 
		return true;
	if(opcode.compare("teq") == 0) 
		return true;
	if(opcode.compare("tst") == 0) 
		return true;
	// if(is_orcc()) 
	// 	return true;
	// addcc %r, imm, %g0 is used just to set the conidtion code. We treat it as a compare instruction
	// if(is_addcc()) 
	// 	if(def.compare("%g0") == 0) 
	// 		return true;
	return false;
}

bool instruction_t::is_fcompare_op() {
	if (is_function_name)
		return false;
	if (opcode.substr(0,4).compare("fcmpd") == 0) // this case includes both fcmpd (double precision) and fcmps (single precision)
		return true;
	return false;
}

bool instruction_t::is_neg_compare_op() {
	if(is_function_name) 
		return false;
	if(opcode.substr(0,3).compare("CMN") == 0) 
		return true;
	return false;
}3


bool instruction_t::is_func_name() {
	return is_function_name;
}

string instruction_t::get_function_name() {
	return function_name;
}


bool instruction_t::is_logical_op() {
	if(is_function_name) 
		return false;

	if (is_shift_op())
		return true;
	//Update CPSR flags on Rn AND Operand2
	if (opcode.substr(0,3).compare("TST") == 0)
		return true;
	//Update CPSR flags on Rn EOR Operand2
	if (opcode.substr(0,3).compare("TEQ") == 0)
		return true;
	// Rd := Rn AND Operand
	if (opcode.substr(0,3).compare("AND") == 0) 
		return true;
	//logical or Rd = Rn OR Operand2
	if (opcode.substr(0,3).compare("ORR") == 0) 
		return true;
	//exclusive (xor) Rd = Rn EOR Operand2
	if (opcode.substr(0,3).compare("EOR") == 0) 
		return true;
	// Bit clear Rd = Rn AND (!operand2)
	if (opcode.substr(0,3).compare("BIC") == 0) 
		return true;
	
	return false;
}

/// \brief returns true if the current instruction is an arithmetic ("add," "sub," "mul," "smul," "sdiv," "udiv," 
/// "fmul," "fadd," "fdiv," "fsub," "fsqrtd," or "fsqrts") operation
/// \details Note: If and only if a spill or ﬁll trap is not generated, SAVE and RESTORE behave like
/// normal ADD instructions, except that the source operands r[rs1] and/or r[rs2] are read from the
/// old window (that is, the window addressed by the original CWP) and the sum is written into r[rd]
/// of the new window (that is, the window addressed by the new CWP).
/// see A.46 in sparc-v9 manual (page 254)
bool instruction_t::is_arithmetic_op() {
	if(is_function_name) 
		return false;
	if(opcode.substr(0,3).compare("add") == 0) 
		return true;
	if(opcode.substr(0,3).compare("sub") == 0) 
		return true;
	if(opcode.substr(0,3).compare("mul") == 0 || opcode.substr(0,4).compare("smul") == 0 || opcode.substr(0,4).compare("umul") == 0) 
		return true;
	if(opcode.substr(0,4).compare("sdiv") == 0 || opcode.substr(0,4).compare("udiv") == 0) 
		return true;

	if(opcode.compare("restore") == 0) 
		return true;
	if(is_save())
		return true;

	//floating point
	if(opcode.substr(0,4).compare("fmul") == 0 
            || opcode.substr(0,4).compare("fadd") == 0 
            || opcode.substr(0,4).compare("fdiv") == 0 
            || opcode.substr(0,4).compare("fsub") == 0 
            || opcode.substr(0,5).compare("fsqrt") == 0 
            || opcode.substr(0,5).compare("fsmul") == 0)
		return true;


	return false;
}

bool instruction_t::is_mov_op() {
	if(is_function_name) 
		return false;
	if (opcode.substr(0,3).compare("MOV") == 0)
		return true;
	if (opcode.substr(0,3).compare("MVN") == 0)
		return true;
	if (opcode.substr(0,3).compare("NEG") == 0)
		return true;
	if (opcode.substr(0,4).compare("FMOV") == 0)
		return true;
	return false;
}


bool instruction_t::is_movcc_op() {
	if(is_function_name) 
		return false;
	//floating point
	if(opcode.compare("move") == 0 || opcode.compare("movrgez") == 0 || opcode.compare("movleu") == 0 || opcode.compare("movle") == 0 || opcode.compare("movl") == 0 || opcode.compare("movne") == 0 || opcode.compare("movg") == 0 || opcode.compare("movge") == 0 || opcode.compare("movule") == 0 || opcode.compare("movcs") == 0 || opcode.compare("movgu") == 0 || opcode.compare("movcc") == 0)
		return true;
	return false;
}
bool instruction_t::is_nop() {
	if(is_function_name) 
		return false;
	if(opcode.compare("nop") == 0)
		return true;
	return false;
}

// bool instruction_t::is_read_state_reg() {
// 	if(is_function_name) 
// 		return false;
// 	if(opcode.compare("rd") == 0)
// 		return true;
// 	return false;
// }

// bool instruction_t::is_write_state_reg() {
// 	if(is_function_name) 
// 		return false;
// 	if(opcode.compare("wr") == 0)
// 		return true;
// 	return false;
// }
bool instruction_t::is_prefetch() {
	if(is_function_name) 
		return false;
	if(opcode.substr(0,4).compare("PRFM") == 0)
		return true;
	return false;
}
bool instruction_t::is_ret() {
	if(is_function_name) 
		return false;
	if(opcode.compare("ret") == 0)
		return true;
	return false;
}
bool instruction_t::is_save() {
	if(is_function_name) 
		return false;
	if(opcode.compare("save") == 0)
		return true;
	return false;
}
bool instruction_t::is_restore() {
	if(is_function_name) 
		return false;
	if(opcode.compare("restore") == 0)
		return true;
	return false;
}
bool instruction_t::is_call_site() {
	if(is_function_name) 
		return false;
	if(opcode.compare("call") == 0)
		return true;
	return false;
}

/// \brief returns true if the current instruction is an illegal instruction trap ("illtrap") operation
bool instruction_t::is_illtrap() {
	if(is_function_name) 
		return false;
	if(opcode.compare("eret") == 0)
		return true;
	return false;
}
bool instruction_t::is_sethi_op() {
	if(is_function_name) 
		return false;
	if(opcode.compare("sethi") == 0) 
		return true;
	return false;
}

bool instruction_t::is_convert_fp_op() {
	if(is_function_name) 
		return false;
	if(opcode.substr(0,4).compare("vcvt") == 0)
		return true;
	return false;
}
void instruction_t::create_fault_list() {
	//only for ops	
	if(is_function_name) 
		return; 

	// TODO: mark whether the fault is microarch-level or arch-level
	for(int i=0; i<NUM_OPS; i++) {
		if(op[i].compare("BLANK") != 0) {
			if( is_const(op[i]) ) {
				if(!CONSTANT_OP_FAULTS) 
					continue;
			}
            
            // if/else chain - if single float
            // then initiate to 32 bits
            // Abdulrahman: fp bug where %f should go only to 32 bits. START FIX
            int num_bits_value = 64;
            if (is_single_fp_register(op[i]))
            {
                num_bits_value = 32;
            }
            // END FIX


            // else if CHANGE NEXT LINE
			if(is_memory_op()) {
				if(op[i].find_first_of("[") != -1) {

					int plus_pos = op[i].find_first_of("+");
					int minus_pos = op[i].find_first_of("-");
					int plus_minus_pos = plus_pos == -1 ? minus_pos : plus_pos;
					if(plus_pos != -1 || minus_pos != -1) {

						int start = op[i].find_first_of("[")+1;
						int end = plus_minus_pos; 
						unit_fault uf1(num_bits_value,op[i].substr(start,end-start), 1, is_def(op[i]));
						fault_list.push_back(uf1);
	
						start = plus_minus_pos + 1;
						end = op[i].find_first_of("]"); 
						bool flag = true;
						if(is_const(op[i].substr(start,end-start)) ) {
							if(CONSTANT_OP_FAULTS)
								flag = true;
							else
								flag = false;
						}
						if (flag) {
							unit_fault uf2(num_bits_value,op[i].substr(start,end-start), 1, is_def(op[i]));
							fault_list.push_back(uf2);
						}
					} else {
						int start = op[i].find_first_of("[")+1;
						int end = op[i].find_first_of("]"); 
						unit_fault uf1(num_bits_value,op[i].substr(start,end-start), 1, is_def(op[i]));
						fault_list.push_back(uf1);
					}

					if(MICRO_ARCH_FAULTS == 1) {
						// unit_fault uf3(64,op[i], 0, is_def(op[i])); // data
						// fault_list.push_back(uf3);

						int start = op[i].find_first_of("[");
						int end = op[i].find_first_of("]"); 
						unit_fault uf4(num_bits_value, op[i].substr(start,end-start+2), 1, is_def(op[i])); // address
						fault_list.push_back(uf4);
					}

				} else {
					unit_fault uf1(num_bits_value,op[i], 0, is_def(op[i]));
					fault_list.push_back(uf1);
				}
			} else if(op[i].find_first_of("(") != -1) { //operand of type %hi(0xABCD1234)

				if(CONSTANT_OP_FAULTS) {
					int start = op[i].find_first_of("(")+1;
					int end = op[i].find_first_of(")"); 
					unit_fault uf1(32,op[i].substr(start,end-start), 0, is_def(op[i])); // 0xABCD1234
					fault_list.push_back(uf1);
				}

			}  else if(op[i].compare("%xcc") == 0 || op[i].compare("%icc") == 0) {
				unit_fault uf1(8, op[i], 0, is_def(op[i]));
				fault_list.push_back(uf1);

			} else {
				bool is_def_bit = false;
				if(i==NUM_OPS-1) {
					is_def_bit = is_def(op[i]);
				} else {
					is_def_bit = (op[i+1].compare("BLANK") == 0) && is_def(op[i]);
				}
				unit_fault uf1(num_bits_value,op[i], 0, is_def_bit);
				fault_list.push_back(uf1);
			}
		} 
	}
}

bool instruction_t::is_instruction_pruned() {
	for(int i=0; i<fault_list.size(); i++)  {
		if(!fault_list[i].is_unit_fault_pruned())
			return false;
	}
	return true;
}

// fm
//	0 - all faults
// 	1 - register faults
// 	2 - integer register faults
void instruction_t::print_fault_list(bool original, fstream & output_stream, int fm) {

	if(fault_list.empty()) 
		return;

	int line_freq = c_line_frequency;
	if(!original) 
		line_freq = pruned_c_line_frequency;

	if(line_freq == 0) 
		return; 

	// compare is a special case
	if(is_instruction_pruned() && !is_compare_op()) 
	//if(!is_compare_op()) // UNCOMMENT AND COMMENT PREVIOUS IF WANT JUST G0 FAULTS
		return;

	output_stream << line_freq << "[" << equivalent_pc << "]:" ;
	output_stream << program_counter << ":" << dep_type << ":{" ;
	output_stream << leading_instruction_pc << ", " << leading_index << "}:" ;
	for(int i=0; i<fault_list.size(); i++) {
		bool print_flag = false;
		if(fm == 0) {
			print_flag = true;
		}
		if(fm == 1) {
			if(is_register(fault_list[i].get_unit())) 
				print_flag = true; 
		}
		if(fm == 2) {
			if(is_int_register(fault_list[i].get_unit())) 
				print_flag = true; 
		}
		if(fm == 3) { // only integer registers and agen
			print_flag = true; 
			if(is_register(fault_list[i].get_unit()) && !is_int_register(fault_list[i].get_unit())) 
				print_flag = false; 
			if(is_condition_code(fault_list[i].get_unit()))
				print_flag = false; 
		}
    	if(fm == 4) {
			if(is_floating_point_register(fault_list[i].get_unit())) 
				print_flag = true; 
		}

		if(print_flag) { 
			if(original) {
				fault_list[i].print_unit_fault(output_stream);
				//fault_list[i].print_pruned_unit_fault_extended(output_stream); // ABDULRAHMAN + RADHA
			} else {
    //            if(is_instruction_pruned()) // UNCOMMENT FOR G0 PRUNED FAULTS
				fault_list[i].print_pruned_unit_fault(output_stream);
			}
		}
	}
	output_stream << "\n" ;
}

// fm = 0 - count all
// fm = 1 - count only integer and fp registers
// fm = 2 - count only integer registers
double instruction_t::count_fault_list( bool original, int fm ) {

	double fault_list_size = 0;
	for(int i=0; i<fault_list.size(); i++) {
		bool count_flag = false;
		if(fm == 0) { // all 
			count_flag = true;
		}
		if(fm == 1) { // all registers
			if(is_register(fault_list[i].get_unit())) 
				count_flag = true; 
		}
		if(fm == 2) { // only integer registers
			if(is_int_register(fault_list[i].get_unit())) 
				count_flag = true; 
		}
		if(fm == 3) { // only integer registers and agen
			count_flag = true; 
			if(is_register(fault_list[i].get_unit()) && !is_int_register(fault_list[i].get_unit())) 
				count_flag = false; 
			if(is_condition_code(fault_list[i].get_unit()))
				count_flag = false; 
		}
		if(fm == 4) { // only floating point registers
			if(is_floating_point_register(fault_list[i].get_unit()))
				count_flag = true;
		}

		if(count_flag) {
			if(original)
				fault_list_size += fault_list[i].get_num_bits();
			else 
				fault_list_size += fault_list[i].get_pruned_bits();
		}
	}

	if(original)
		return fault_list_size*((double)c_line_frequency/1000000);
	else {
		return fault_list_size*((double)pruned_c_line_frequency/1000000);
	}
}

int instruction_t::get_asm_line_number() {
	return asm_line_number;
}

int instruction_t::get_c_line_number() {
	return c_line_number;
}

int instruction_t::get_c_line_frequency() {
	return c_line_frequency;
}

void instruction_t::set_c_line_frequency(int num) {
	c_line_frequency = num;
}

void instruction_t::set_pruned_c_line_frequency(int num) {
	pruned_c_line_frequency = num;
}

int instruction_t::get_pruned_c_line_frequency() {
	return pruned_c_line_frequency;
}

bool instruction_t::is_stack_address() {
	if(!is_memory_op())
		return false;
	
	// If address has %fp its is a stack address
	// Addresses without %fp can also be stack addresses. I do not know how to identify them.
	for(int i=0; i<fault_list.size(); i++) 
		if(fault_list[i].is_address()) 
			if(fault_list[i].get_unit().compare("%fp") == 0) 
				return true;
	return false;
}	
bool instruction_t::is_global_address() {
	if(!is_memory_op())
		return false;

	return false;
}	

void instruction_t::prune_addresses() {
	if(!is_memory_op())
		return;

	for(int i=0; i<fault_list.size(); i++) {
		if(fault_list[i].is_address()) {
			// How to distinguish between heap and stack pointers?
			// Answer: Dynamic profiling - get this info from simics runs
			
			if(is_stack_address()) {
				fault_list[i].set_pruned_bits(get_stack_limit(program_name), DETECTED);
				//cout << " its in stack\n";
			} else if(is_global_address()) {
				fault_list[i].set_pruned_bits(get_global_limit(program_name), DETECTED);
				//cout << " its in global\n";
			} else { // heap address
				fault_list[i].set_pruned_bits(get_heap_limit(program_name), DETECTED);
				//cout << " its in heap \n";
			}
		}
	}
}

void instruction_t::prune_branch_targets() {
	if(!is_branch_op())
		return;

	// branch instr will have either 1 or two operands
	if(fault_list.size() == 1) 
		fault_list[0].set_pruned_bits(get_text_size(program_name), DETECTED);
	else 
		fault_list[1].set_pruned_bits(get_text_size(program_name), DETECTED);

}

/// \brief gets the instruction definition
/// \returns definition as a string
string instruction_t::get_def() {
	return def;
}

/// \brief returns true if the local op is useful
bool instruction_t::find_use(string local_op) {

	if(is_memory_op())  {
		for(int i=0; i<fault_list.size(); i++)  {

			if(fault_list[i].get_unit().compare(local_op) == 0) 
				return true;
		}
	}

	for(int i=0; i<NUM_OPS; i++) 
		if(op[i].compare("BLANK") != 0) 
			if(op[i].compare(local_op) == 0) 
				if(op[i].compare(def) != 0)
					return true;
		
	return false;
}

/// \brief gets the extra count from the fault list
/// \returns extra count as an int, -1 for error
int instruction_t::get_extra_count(string reg, bool is_def) {
	for(int i=0; i<fault_list.size(); i++)  {
		if(fault_list[i].get_unit().compare(reg) == 0 ) {
			if( is_def && fault_list[i].get_is_def() ) {
				return fault_list[i].get_extra_count();
			} else if( !is_def && !fault_list[i].get_is_def() ) {
				return fault_list[i].get_extra_count();
			}
		}
	}
	return -1;
}

/// \brief marks the current definition for etra counting
void instruction_t::mark_def_for_extra_counting() {
	for(int i=0; i<fault_list.size(); i++)  {
		if(fault_list[i].get_is_def()) {
			fault_list[i].mark_for_extra_counting();
			break;
		}
	}
}

/// \brief marks the current instruction for extra counting
void instruction_t::mark_for_extra_counting(string reg, int count, string pc) {
	for(int i=0; i<fault_list.size(); i++)  {
		if(fault_list[i].get_unit().compare(reg) == 0 && !fault_list[i].get_is_def()) {
			fault_list[i].mark_for_extra_counting(count+1, pc);
			break;
		}
	}
}

/// \brief removes instruction from the fault list
/// \returns the pruned bits if successful, -1 if error
int instruction_t::remove_from_fault_list(string local_op, bool is_def) {
	int return_val= -1;
	for(int i=0; i<fault_list.size(); i++)  {
		// only if its a def
		if(fault_list[i].get_unit().compare(local_op) == 0 && fault_list[i].get_is_def() && is_def) {
			if(DEBUG_PRUNING)
				cout << asm_line_number << ": removing fault: " << local_op << "\n";
			return_val = fault_list[i].get_pruned_bits();
			fault_list[i].set_pruned_bits(0, EQUIVALENT);
			break;
		}
		// only if its a use
		if(fault_list[i].get_unit().compare(local_op) == 0 && !fault_list[i].get_is_def() && !is_def) {
			if(DEBUG_PRUNING)
				cout << asm_line_number << ": removing fault: " << local_op << "\n";
			return_val = fault_list[i].get_pruned_bits();
			fault_list[i].set_pruned_bits(0, EQUIVALENT);
			break;
		}

	}
	return return_val;
}

/// \brief prunes the instruction that contains the %%g0 register
void instruction_t::prune_g0_faults() {
	int return_val= -1;
	for(int i=0; i<fault_list.size(); i++)  {
		if(fault_list[i].get_unit().compare("\%g0") == 0) {
			return_val = fault_list[i].get_pruned_bits();
			fault_list[i].set_pruned_bits(0, MASKED);
		}
	}
}

/// \brief prunes the instruction that contains a constant shift operation
/// \details This function is a very specific one.
/// It prunes faults in const of shift ops
/// \n sllx    %%o4,12,%%o5
/// \n In this case, we know that its the second operand.
void instruction_t::prune_shift_const() {
	if(fault_list.size() == 3)  {
		fault_list[1].set_pruned_bits(6, MASKED);
	}
}

/// \brief prunes a set number of bits from the instruction
/// \param s specified starting point of prune
/// \param e specified ending point of prune
void instruction_t::prune_bits(string op, bool is_def, int s, int e) {
	for(int i=0; i<fault_list.size(); i++) {
		if(fault_list[i].get_unit().compare(op) == 0 && (!is_def && !fault_list[i].get_is_def()) ) {
			fault_list[1].set_pruned_bits(e-s, MASKED);
		}
	}
}

/// \brief returns true if the operand is a constant value
bool instruction_t::has_const_op() {
	for(int i=0; i<NUM_OPS; i++) 
		if(is_number(op[i]))
			return true;
	return false;
}

/// \brief returns true if the first operand was removed
bool instruction_t::is_op0_removed() {
	if(fault_list.size() == 3)  
		if(fault_list[0].get_pruned_bits() == 0) {
			return true;
		}
	return false;
}

/// \brief returns true if the definition was removed
bool instruction_t::is_def_removed() {
	for(int i=0; i<fault_list.size(); i++) {
		if(fault_list[i].get_unit().compare(get_def()) == 0) {
			if(fault_list[0].get_pruned_bits() == 0) {
				return true;
			}
		}
	}
	return false;
}

/// \brief returns the desired operand of the instruction
/// \param op_num ranges from 0-2, used to get the first, second, or third operand respectively
/// \returns the specified operand in string format, NULL if error
string instruction_t::get_op(int op_num) {
	if(op_num >= 3) 
		return "";
	return op[op_num];
}

/// \brief gets the instruction's opcode
/// \returns opcode in string format
string instruction_t::get_opcode() {
	return opcode;
}


/// \brief gets the pruned bits from the instruction based on the operand
/// \returns the pruned bits as an integer, -1 if error
int instruction_t::get_fault_bits(string op) {
	for(int i=0; i<fault_list.size(); i++) {
		if(fault_list[i].get_unit().compare(op) == 0) {
			return fault_list[i].get_pruned_bits();
		}
	}
	return -1;
}

/// \brief sets the pruned bits for the instruction
/// \param op desired operand
/// \param bits number of bits to be pruned
/// \param is_val used only load/store instructions
void instruction_t::set_fault_bits(string op, int bits, int is_val) {
	for(int i=0; i<fault_list.size(); i++) {
		if(fault_list[i].get_unit().compare(op) == 0) {
			if(!is_val) {
				fault_list[i].set_pruned_bits(bits, EQUIVALENT);
				break;
			} else {
				if(!fault_list[i].is_address()) {
					fault_list[i].set_pruned_bits(bits, EQUIVALENT);
					break;
				}
			}
		}
	}
}

/// \brief accumulates information used for statistical analysis
/// \param aggr_stats a _statistics struct that holds the statistical data
void instruction_t::accumulate_fault_stats(statistics & aggr_stats) {
	for(int i=0; i<fault_list.size(); i++) {
		fault_list[i].accumulate(aggr_stats, c_line_frequency, pruned_c_line_frequency);
	}
}

/// \brief gets the program counter of the instruction
/// \returns program counter in string format
string instruction_t::get_program_counter() {
	return program_counter;
}

/// \brief sets the equivalent program counter of the instruction
/// \param pc the desired value for the equivalent pc
void instruction_t::set_equivalent_pc(string pc) {
	equivalent_pc = pc;
}

/// \brief sets the leading instruction program counter
/// \param pc program counter to set if dep_type parameter not equal to -1
/// \param d_type if equal to -1, then set the leading store program counter to the pc parameter
/// \param l_index gap of the instruction for the instruction at pc
void instruction_t::set_leading_store_pc(string pc, dep_type_t d_type, int l_index) {
	if(dep_type == -1) {
		if(d_type == CONTROL || d_type == COMPARE) {
			leading_instruction_pc = program_counter;
			dep_type = d_type;
			leading_index = 0;
		} else {
			leading_instruction_pc = pc;
			dep_type = d_type;
			leading_index = l_index;
		}
	}
}

/// \brief forces the leading instruction program counter to be the desired program counter
/// \param pc desired program counter
/// \param d_type dependency type (refer to dep_type_t for full list)
/// \param l_index gap of the instruction for the instruction at pc
void instruction_t::force_leading_store_pc(string pc, dep_type_t d_type, int l_index) {
	leading_instruction_pc = pc;
	dep_type = d_type;
	leading_index = l_index;
}

/// \brief gets the leading store program counter
/// \returns leading instruction program counter in string format 
string instruction_t::get_leading_store_pc() {
	return leading_instruction_pc;
}

/// \brief returns true if the operand is a register
/// \param op the operand string being tested
/// \details not a great function to use.
bool instruction_t::is_register(string op) {
    // do not include fp as floating register. only an int register. may have already been implemented, but just check
	//    fprintf(stderr, "Is %s a register?....", op.c_str());
	if(op.size() > 4 || op.size() < 2)  
		return false;
	if(op[0] != 'r') {
		if (op[0] != 's' || op[0] != 'l' || op[0] != 'p')
			return false;
		if (op[0] == 's' && op[1] != 'p')
			return false;
		if (op[0] == 'l' && op[1] != 'r')
			return false;
		if (op[0] == 'p' && op[1] != 'c')
			return false;
		return false;

	}
 // fprintf(stderr, "YES! It is %s\n", op.c_str());
	return true;
}

/// \brief returns true if the operand is a condition code register
/// \param op the operand being tested
bool instruction_t::is_condition_code(string op) 
{
	if( op.compare("%icc") == 0 ) // condition codes are not included as registers for now
		return true;
	if( op.substr(0,4).compare("%fcc") == 0 ) // condition codes are not included as registers for now
		return true;
	if( op.substr(0,4).compare("%fsr") == 0 ) // condition codes are not included as registers for now
        return false;

    // abdul: might want to also exclude FSR
	return false;
}

/// \brief Returns true if the operand is a program status register 
/// \param op the operand being checked
bool instruction_t::is_program_status_register_name (string op) 
{
	if (op.size() > 4 || op.size() < 4)
		return false;
	if ((op.substr(0,4).compare("cpsr") == 0) || (op.substr(0,4).compare("spsr") == 0))
		return true;
	return false;
}

/// \brief Returns true if the operand is a Coprocessor register - C0-C15 or P0 - P15
/// \param op the operand being checked 
bool instruction_t::is_coprocessor_names (string op)
{
	if (op.size() > 3 || op.size() < 2)
		return false;
	if (!((op[0] == 'p') || op[0] == 'c'))
		return false;
	return true;
}

/// \brief returns true if the operand is a floating point register
/// \param op the operand being tested
bool instruction_t::is_floating_point_register(string op) 
{
 // fprintf(stderr, "Is %s a floating point reg?....\n", op.c_str());
    if ((op.size() > 4 || op.size() < 2))
		return false;
	if ((!(op[0] != 'f') && (op[0] != 's') && (op[0] != 'd')))
		return false;
	// stack pointer is not counted as a floating point register.
	if (op.substr(0,2).compare("sp") == 0)
		return false;
	// Frame pointer is not counted as a floating point register.
	if (op.substr(0,2).compare("fp") == 0)
		return false;
 // fprintf(stderr, "YES! It is %s\n", op.c_str());
	return true;
}



/// \brief returns true if the operand is an integer register usually start with R0-R15; A1-4 (r0 - r3); v1-8(r4-r11)
/// \param op the operand being tested
bool instruction_t::is_int_register(string op) 
{
 // fprintf(stderr, "Is %s an integer reg?....", op.c_str());
	if(op.size() > 4 || op.size() < 2) 
		return false;
	if(op[0] != 'r'){
		if (!(op[0] == 'a' || op[0] == 'v'))
			return false;
		else 
			return true;
		// Stack base register, R9
		if (op.substr(0,2).compare("sb") == 0)
			return true;
		// Stack limit, R10
		if (op.substr(0,2).compare("sl") == 0)
			return true;
		// Frame Pointer, R11
		if (op.substr(0,2).compare("fp") == 0)
			return true;
		// Intra Procedure Call Scratch Register, R12
		if (op.substr(0,2).compare("ip") == 0)
			return true;
		// Stack Pointer, R13
		if (op.substr(0,2).compare("sp") == 0)
			return true;
		// Link Register, R14
		if (op.substr(0,2).compare("lr") == 0)
			return true;
		// Program Counter, R15
		if (op.substr(0,2).compare("pc") == 0)
			return true;

		return false;
	}

	// conditional code register in ARM?
 // fprintf(stderr, "YES! It is %s\n", op.c_str());
	return true;
}

/// \brief gets the number of call parameters
/// \returns the number of call parameters in int format
int instruction_t::get_num_call_parameters() {
	return params_list.size();
}

/// \brief gets the call parameter at a specified index
/// \param i the index being accessed
/// \returns call parameter in string format, NULL if error
string instruction_t::get_call_parameter(int i) {
	if(params_list.size() > i) 
		return params_list[i];
	else 
		return "";
}

/// \brief gets the number of return parameters
/// \returns the number of return parameters in int format
int instruction_t::get_num_ret_parameters() {
	return ret_val_list.size();
}

/// \brief gets the return parameter at a specified index
/// \param i the index being accessed
/// \returns return parameter in string format, NULL if error
string instruction_t::get_ret_parameter(int i) {
	if(ret_val_list.size() > i) 
		return ret_val_list[i];
	else 
		return "";
}

/// \brief returns true if the operand is a definition
/// \param op operand being checked
bool instruction_t::is_def(string op) {
	if(op.compare(def) == 0) 
		return true;
	else 
		return false;
}

/// \brief gets the target for the branch instruction
/// \returns branch target in string format, NULL if error
string instruction_t::get_branch_target() {
	if(is_branch_op()) {
		if(get_opcode().find(",p") != -1)
			return get_op(1);
		else 
			return get_op(0);
	}
	return "";
}

/// \brief gets the target for the call instruction
/// \returns target of call in string format, NULL if error
string instruction_t::get_call_target() {
	if(is_call_site()) {
		return get_op(0);
	}
	return "";
}

/// \brief sets the call control flow graph for the instruction
/// \param c cfg_t control flow graph specified by user
void instruction_t::set_call_cfg(cfg_t * c) {
	call_cfg = c;
}

/// \brief gets the call control graph for the instruction
/// \returns call control graph
cfg_t * instruction_t::get_call_cfg() {
	return call_cfg;
}

/// \brief gets the basic block for the instruction
/// \returns the basic block
bb_t * instruction_t::get_bb() {
	return bb;
}

/// \brief sets the basic block for the instruction
/// \param bb_t basic block specified by user
void instruction_t::set_bb(bb_t * b) {
	bb = b;
}

/// \brief gets the duplicate uses for the instruction
/// \returns the duplicate uses as a set of strings
set<string> instruction_t::get_duplicate_uses() {
	set<string> uses;
    set<string> dup_uses;
	int j;
	// skip BLANKS
	for(j=NUM_OPS-1; j>=0; j--) 
		if(op[j].compare("BLANK") != 0) 
			break;
	// skip def
	if(op[j].compare(def) == 0) 
		j--;
    
	for(; j >= 0; j--) {
		string local_op = get_op(j);
        //cout << "LOCAL OP: " << local_op << endl;
		if( is_register(local_op) ) { // only if the operand is a register
            //cout << "    local op is reg: " << local_op << endl;
			//if(uses.find(local_op) != uses.end()) 
                //cout << "GOT INSIDE 1" << endl;
				if(!uses.insert(local_op).second ) {
                   dup_uses.insert(local_op); // can make this cleaner code TODO 
                   //printf("JUST INDENTIFIED A DUPLICATE 1: %s\n", local_op.c_str());
                }
            
		} else {
			// find if this op has a reg in it
			int found = local_op.find_first_of("%");
			while (found != string::npos) {
				string local_reg = get_reg(local_op, found);
                //cout << "    local op has a reg in it: " << local_reg << endl;
				//if(local_reg.compare("") != 0) 
					//if(uses.find(local_reg) != uses.end()) 
                        //cout << "GOT INSIDE 2" << endl;
						if(!uses.insert(local_reg).second ){
                            //printf("JUST INDENTIFIED A DUPLICATE2: %s\n", local_reg.c_str());
                            dup_uses.insert(local_reg); // can make this cleaner code TODO
                        }
                    
				found = local_op.find_first_of("%", found+1);
			}
		}
	}

	return dup_uses;
}

/// \brief gets the total uses for the instruction
/// \returns the total uses as a set of strings
set<string> instruction_t::get_uses() {
	set<string> uses;
	int j;
	// skip BLANKS
	for(j=NUM_OPS-1; j>=0; j--) 
		if(op[j].compare("BLANK") != 0) 
			break;
	// skip def
	if(op[j].compare(def) == 0) 
		j--;

	for(; j >= 0; j--) {
		string local_op = get_op(j);
		if( is_register(local_op) ) { // only if the operand is a register
      	    if(uses.find(local_op) == uses.end())
				uses.insert(local_op);
		} else {
			// find if this op has a reg in it
			int found = local_op.find_first_of("%");
			while (found != string::npos) {
				string local_reg = get_reg(local_op, found);
				if(local_reg.compare("") != 0) 
                    if(uses.find(local_reg) == uses.end())
						uses.insert(local_reg);
				found = local_op.find_first_of("%", found+1);
			}
		}
	}
	// special case of store double/store extended
	if(DEBUG) {
		set<string>::iterator it;
		cout << get_program_counter();
		cout << ":uses:";
		int something = 0;
		for (it=uses.begin(); it != uses.end(); it++)  {
			if((*it).compare("") != 0)  {
				cout << " " << (*it);
				something = 1;
			}
		}
		cout << endl;
	}

	return uses;
}

/// \brief gets the non stack and floating point registers
/// \details checks for registers of this format  %gx %lx %lo %ix %dxx %fxx
/// \returns register in string format
// for ARM ISA, there are only r0 - r12, and sp - r13, lr - r14 and pc r15
string instruction_t::get_reg(string str, int start) {
	string local_reg = "";
	string register_number = "  ";

	// for integer registers which usually start with R.
	int found_r = str.find_first_of("r", start);

	if (str.size() - found_r >= 2) {
		if (found_r != string::npos) {
			if (isdigit(str[found_r + 1])) {
				local_reg = str.substr(found_r, 2);
				register_number[0] = str[found_r + 1];
				if (str.size() - found_r >= 3)
					if (isdigit(str[found_r + 2]))
						if (strtoul(register_number, &register_number, 10) < 15)
							local_reg = str.substr(found_r, 3);
						else 
							local_reg = "";
			}
		}
	}

	// for floating point registers starting with f, d, s
	// F0-F7
	int found_f = str.find_first_of("f", start);
	if (str.size() - found_f >= 2) {
		if (found_f != string::npos) {
			if (isdigit(str[found_f + 1])) {
				register_number[0] = str[found_r + 1];
				if (strtoul(register_number, & register_number, 10) < 8)
					local_reg = str.substr(found_f, 2);
				else 
					local_reg = "";
			}

		}
	}

	// D0-D15
	int found_d = str.find_first_of("d", start);
	if (str.size() - found_d >= 2) {
		if (found_d != string::npos) {
			if (isdigit(str[found_d + 1])) {
				local_reg = str.substr(found_d, 2);
				if (str.size() - found_d >= 3)
					if (isdigit (str[found_d + 2]))
						if (strtoul(register_number, &register_number, 10) < 16)
							local_reg = str.substr(found_d, 3);
						else 
							local_reg = "";
			}

		}
	}

	// S0-S31
	int found_s = str.find_first_of("s", start);
	if (str.size() - found_s >= 2) {
		if (found_s != string::npos) {
			if (isdigit(str[found_s + 1])) {
				local_reg = str.substr(found_s, 2);
				if (str.size() - found_s >= 3)
					if (isdigit (str[found_s + 2]))
						if (strtoul(register_number, &register_number, 10) < 32)
							local_reg = str.substr(found_s, 3);
						else 
							local_reg = "";
			}

		}
	}

	return local_reg;
}

/// \brief sets the leading store program counter TODO: elaborate?
void instruction_t::mark_instruction(string pc, dep_type_t d_type, int distance) {
	set_leading_store_pc(pc, d_type, distance);
}

/// \brief sets the leading store program counter TODO: difference between other mark_instruction function?
void instruction_t::mark_instruction_force(string pc, dep_type_t d_type, int distance) {
	// force_leading_store_pc(pc, d_type, distance);
	set_leading_store_pc(pc, d_type, distance);
}

/// \brief gets the dependency type
/// \returns dependency type in int format
int instruction_t::get_dep_type() {
	return dep_type;
}

/// \brief prunes all uses in the fault list TODO: is this function still used?
void instruction_t::prune_uses() {
	if(fault_list.empty()) 
		return;
	for(int i=0; i<fault_list.size(); i++) {
		fault_list[i].prune_uses();
	}
}

/// \brief returns true if the operand is a constant
/// \param op user specified operand in string format
bool instruction_t::is_const(string op) {
	int start_dec = -1;
	int start_hex = -1;
	// hex number 
	if( op.substr(0,2).compare("#0x") == 0 )
		start_hex = 2;
	else if( op.substr(0,3).compare("-0x") == 0 )
		start_hex = 3;

	// decimal number 
	if (op.substr(0,1).compare("#") == 0)
		start_dec = 1;
	else if (op.substr(0,2).compare("#-") == 0)
		start_dec = 2

	if (start_hex >= 0 && op.size() > start_hex) {
		for (int i = start_hex; i < op.size(); i++) 
			if (!isxdigit(op[i])) 
				return false;
		return true;
	}

	if (start_dec >= 0 && op.size() > start_dec) {
		for (int i = start_dec; i < op.size(); i++) 
			if (!isdigit(op[i])) 
				return false;
		return true;
	}

	return false;
}

