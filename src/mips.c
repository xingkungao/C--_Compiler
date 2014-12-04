#include "interCode.h"
#include "mips.h"
#include "var.h"
#include <stdio.h>
#include <malloc.h>


#define ASSEM_HDR ".data\n"\
"_prompt: .asciiz \"Enter an integer:\"\n"\
"_ret: .asciiz \"\\n\"\n"\
".globl main\n"\
"\n"\
".text\n"\
"read:\n"\
"\tli $v0, 4\n"\
"\tla $a0, _prompt\n"\
"\tsyscall\n"\
"\tli $v0, 5\n"\
"\tsyscall\n"\
"\tjr $ra\n"\
"\n"\
"write:\n"\
"\tli $v0, 1\n"\
"\tsyscall\n"\
"\tli $v0, 4\n"\
"\tla $a0, _ret\n"\
"\tsyscall\n"\
"\tmove $v0, $0\n"\
"\tjr $ra\n"\
"\n"

char *reg_names[22] = {"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9", "$a0", "$a1", "$a2", "$a3"};

char *print_reg(int reg_id) {
    return reg_names[reg_id];
}

int isRelop(InterCode* code) {
	return ((code)->kind == Eq || (code)->kind == Ne || (code)->kind == Lt \
			|| (code)->kind == Gt || (code)->kind == Ge || (code)->kind == Le);
}
void spill_reg(FILE *file, int reg_id) {
    if (reg_id >= NR_REGS)
        return;
    var_node *p_var = get_var_node(regs[reg_id].var_id);
    if (p_var->is_param_reg)
        return;
    fprintf(file, "\tsw %s, %d($fp)\n", print_reg(reg_id), -p_var->offset - 4);
}

void free_reg(FILE *file, int reg_id) {
    if (reg_id >= NR_REGS)
        return;
    if (regs[reg_id].var_id < 0)
        return;
    if (regs[reg_id].is_modified) {
        spill_reg(file, reg_id);
    }
    regs[reg_id].var_id = -1;
}

void set_reg_modified(int reg_id) {
    if (reg_id >= NR_REGS)
        return;
    regs[reg_id].is_modified = 1;
}

int allocate_reg(FILE *file, int var_id) {
    var_node *p_var = get_var_node(var_id);

    if (p_var && p_var->is_param_reg)
        return NR_REGS + p_var->offset;

    int i;
    for (i = 0; i < NR_REGS; ++ i)
        if (regs[i].var_id < 0) {
            regs[i].var_id = var_id;
            regs[i].is_modified = 0;
            return i;
        }

    spill_reg(file, 0);
    regs[0].var_id = var_id;
    regs[0].is_modified = 0;

    return 0;
}

int store_const_into_reg(FILE *file, int val) {
    int reg = allocate_reg(file, CONST_TEMP_REG);
    fprintf(file, "\tli %s, %d\n", print_reg(reg), val);

    return reg;
}

int ensure_reg(FILE *file, int var_id) {
    var_node *p_var = get_var_node(var_id);

    if (p_var == NULL)
        return -1;

    if (p_var->is_param_reg)
        return NR_REGS + p_var->offset;
    
    int i;
    for (i = 0; i < NR_REGS; ++ i)
        if (regs[i].var_id == var_id)
            return i;

    int reg = allocate_reg(file, var_id);
    fprintf(file, "\tlw %s, %d($fp)\n", print_reg(reg), -p_var->offset - 4);

    return reg;
}

void print_mips_code(FILE *file, InterCodes *codes) {
    InterCode *code = codes->code;

    if (code->kind == Label)
        fprintf(file, "label%d:\n", code->u.binop.result->u.var_no);
    else if (code->kind == Assign) {
        if (code->u.assign.left->kind == Variable && code->u.assign.right->kind == Constant) {
            int regx = allocate_reg(file, code->u.assign.left->u.var_no);
            fprintf(file, "\tli %s, %d\n", print_reg(regx), code->u.assign.right->u.val);
            set_reg_modified(regx);
            free_reg(file, regx);
        }
        else if (code->u.assign.left->kind == Variable && code->u.assign.right->kind == Variable) {
            int regy = ensure_reg(file, code->u.assign.right->u.var_no);
            int regx = allocate_reg(file, code->u.assign.left->u.var_no);
            fprintf(file, "\tmove %s, %s\n", print_reg(regx), print_reg(regy));
            set_reg_modified(regx);
            free_reg(file, regx);
            free_reg(file, regy);
        }
        else {
            fprintf(file, "\tError!!!\n");
        }

    }
    else if (code->kind == Add || code->kind == Sub || code->kind  == Mul || code->kind == Div) {
            int regy, regz;

            if (code->u.binop.op1->kind == Constant)
                regy = store_const_into_reg(file, code->u.binop.op1->u.val);
            else if (code->u.binop.op1->kind == Variable)
                regy = ensure_reg(file, code->u.binop.op1->u.var_no);

            if (code->u.binop.op2->kind == Constant)
                regz = store_const_into_reg(file, code->u.binop.op2->u.val);
            else if (code->u.binop.op2->kind == Variable)
                regz = ensure_reg(file, code->u.binop.op2->u.var_no);

            free_reg(file, regy);free_reg(file, regz);

            int regx = allocate_reg(file, code->u.binop.result->u.var_no);

            if (code->kind == Add)
                fprintf(file, "\tadd %s, %s, %s\n", print_reg(regx), print_reg(regy), print_reg(regz));
            else if (code->kind == Sub)
                fprintf(file, "\tsub %s, %s, %s\n", print_reg(regx), print_reg(regy), print_reg(regz));
            else if (code->kind == Mul)
                fprintf(file, "\tmul %s, %s, %s\n", print_reg(regx), print_reg(regy), print_reg(regz));
            else if (code->kind == Div) {
                fprintf(file, "\tdiv %s, %s\n", print_reg(regy), print_reg(regz));
                fprintf(file, "\tmflo %s\n", print_reg(regx));
            }

            set_reg_modified(regx);
            free_reg(file, regx);
    }
    else if (isRelop(code)) {
        int regx, regy;

        if (code->u.binop.op1->kind == Constant)
            regx = store_const_into_reg(file, code->u.binop.op1->u.val);
        else if (code->u.binop.op1->kind == Variable)
            regx = ensure_reg(file, code->u.binop.op1->u.var_no);

        if (code->u.binop.op2->kind == Constant)
            regy = store_const_into_reg(file, code->u.binop.op2->u.val);
        else if (code->u.binop.op2->kind == Variable)
            regy = ensure_reg(file, code->u.binop.op2->u.var_no);

        free_reg(file, regx);free_reg(file, regy);

        fprintf(file, "\t");

        if (code->kind == Eq)
            fprintf(file, "beq");
        else if (code->kind == Ne)
            fprintf(file, "bne");
        else if (code->kind == Gt)
            fprintf(file, "bgt");
        else if (code->kind == Lt)
            fprintf(file, "blt");
        else if (code->kind == Ge)
            fprintf(file, "bge");
        else if (code->kind == Le)
            fprintf(file, "ble");


        fprintf(file, " %s, %s, label%d\n", print_reg(regx), print_reg(regy), code->u.binop.result->u.var_no);

    }
    else if (code->kind == Return) {
        int regx;

        if (code->u.uniop.op->kind == Constant)
            regx = store_const_into_reg(file, code->u.uniop.op->u.val);
        else if (code->u.uniop.op->kind == Variable)
            regx = ensure_reg(file, code->u.uniop.op->u.var_no);

        free_reg(file, regx);
        
        fprintf(file, "\tmove $v0, %s\n", print_reg(regx));

        fprintf(file, "\taddi $sp, $fp, 8\n");
        fprintf(file, "\tlw $ra, -4($sp)\n");
        fprintf(file, "\tlw $fp, -8($sp)\n");

        fprintf(file, "\tjr $ra\n");
    }
    else if (code->kind == Goto) {
        fprintf(file, "\tj label%d\n", code->u.uniop.op->u.var_no);
    }
    else if (code->kind == Arg) {

        int i;
        for (i = 0; i < 4; ++ i)
            fprintf(file, "\tsw $a%d, %d($sp)\n", i, -(i + 1) * 4);
        fprintf(file, "\tsubu $sp, $sp, %d\n", 16);

        args_num = 0;

        InterCodes *tmp = codes;

        while (tmp && tmp->code->kind == Arg) {
            ++ args_num;
            tmp = tmp->next;
        }

        fprintf(file, "\tsubu $sp, $sp, %d\n", args_num >= 4 ? 4 * (args_num - 4) : 0);

        tmp = codes;
        i = args_num;

        while (i > 0) {
            int regx;

            if (tmp->code->u.uniop.op->kind == Constant)
                regx = store_const_into_reg(file, tmp->code->u.uniop.op->u.val);
            else if (tmp->code->u.uniop.op->kind == Variable)
                regx = ensure_reg(file, tmp->code->u.uniop.op->u.var_no);
            -- i;
            if (i < 4) {
                fprintf(file, "\tmove $a%d, %s\n", i, print_reg(regx));
            }
            else {
                fprintf(file, "\tsw %s, %d($sp)\n", print_reg(regx), 4 * (i - 4));
            }
            free_reg(file, regx);
            tmp = tmp->next;
        }
    } 
    else if (code->kind == Call) {
        int regx = allocate_reg(file, code->u.call.result->u.var_no);

        fprintf(file, "\tjal %s\n", code->u.call.name);
        fprintf(file, "\taddi $sp, $sp, %d\n", args_num >= 4 ?  4 * (args_num - 4) : 0);
        fprintf(file, "\tmove %s, $v0\n", print_reg(regx));

        fprintf(file, "\taddi $sp, $sp, %d\n", 16);

        int i;
        for (i = 0; i < 4; ++ i)
            fprintf(file, "\tlw $a%d, %d($sp)\n", i, -(i + 1) * 4);

        set_reg_modified(regx);
        free_reg(file, regx);
    }
    else if (code->kind == Read) {
        int regx = allocate_reg(file, code->u.uniop.op->u.var_no);
        fprintf(file, "\tsubu $sp, $sp, 4\n");
        fprintf(file, "\tsw $a0, 0($sp)\n");
        fprintf(file, "\tjal read\n");
        fprintf(file, "\taddi $sp, $sp, %d\n", args_num >= 4 ?  4 * (args_num - 4) : 0);
        fprintf(file, "\tmove %s, $v0\n", print_reg(regx));
        fprintf(file, "\tlw $a0, 0($sp)\n");
        fprintf(file, "\taddi $sp, $sp, 4\n");
        set_reg_modified(regx);
        free_reg(file, regx);
    }
    else if (code->kind == Write) {
        int regx;
        if (code->u.uniop.op->kind == Variable)
            regx = ensure_reg(file, code->u.uniop.op->u.var_no);
        else if (code->u.uniop.op->kind == Constant)
            regx = store_const_into_reg(file, code->u.uniop.op->u.val);
        free_reg(file, regx);
        fprintf(file, "\tsubu $sp, $sp, 4\n");
        fprintf(file, "\tsw $a0, 0($sp)\n");
        fprintf(file, "\tmove $a0, %s\n", print_reg(regx));
        fprintf(file, "\tjal write\n");
        fprintf(file, "\tlw $a0, 0($sp)\n");
        fprintf(file, "\taddi $sp, $sp, 4\n");
    }

}

int is_first_instr(InterCodes *codes) {
    return codes->code->kind == Func || codes->code->kind == Call || codes->code->kind == Label || isRelop(codes->prev->code) || codes->prev->code->kind == Goto || codes->prev->code->kind == Call;
}

void init_basic_block(InterCodes *codes) {

}

void init_regs() {
    int i;
    for (i = 0; i < NR_REGS; ++ i)
        regs[i].var_id = -1;
}

void ensure_var(int var_id, int size) {
    var_node *p_var = get_var_node(var_id);

    if (p_var == NULL) {
        insert_var_node(var_id, 0, offset_now);
        offset_now += size;
    }
}

int is_in_func = 0;

void init_func_var(FILE *file, InterCodes *codes) {
    offset_now = 0;
    args_num = 0;
    is_in_func = 1;
    init_var_table();
    init_regs();

    if (codes && codes->code->kind == Func) {
        fprintf(file, "%s:\n", codes->code->u.funcdec.name);

        codes = codes->next;
        
        int param_num = 0;

        while (codes && codes->code->kind != Func) {
            InterCode *code = codes->code;
            if (code->kind == Param) {
                if (param_num < 4) {
                    insert_var_node(code->u.uniop.op->u.var_no, 1, param_num);
                }
                else {
                    insert_var_node(code->u.uniop.op->u.var_no, 0, -(param_num - 4) * 4 - 12);
                }
                ++ param_num;
            }
            else if (code->kind == Add || code->kind == Sub || code->kind == Mul || code->kind == Div) {
                if (code->u.binop.result->kind == Variable) {
                    ensure_var(code->u.binop.result->u.var_no, 4);
                }
            }
            else if (code->kind == Call) {
                if (code->u.call.result->kind == Variable) {
                    ensure_var(code->u.call.result->u.var_no, 4);
                }
            }
            else if (code->kind == Assign) {
                if (code->u.assign.left->kind == Variable) {
                    ensure_var(code->u.assign.left->u.var_no, 4);
                }
            }
            else if (code->kind == Read) {
                if (code->u.uniop.op->kind == Variable) {
                    ensure_var(code->u.uniop.op->u.var_no, 4);
                }
            }
            else if (code->kind == Decl) {
                ensure_var(code->u.dec.addr->u.var_no, code->u.dec.size);
            }

            codes = codes->next;
        }
    }

    fprintf(file, "\tsw $ra, -4($sp)\n");
    fprintf(file, "\tsw $fp, -8($sp)\n");
    fprintf(file, "\taddi $fp, $sp, -8\n");
    fprintf(file, "\taddi $sp, $sp, %d\n", -8 - offset_now);
}

InterCodes *codes_next(FILE *file, InterCodes *codes) {
    InterCodes *ret;
    if (codes->code->kind == Arg) {
        while (codes->code->kind == Arg)
            codes = codes->next;
        ret = codes;
    }
    else
        ret = codes->next;

    if (ret == NULL)
        is_in_func = 0;

    if (ret && ret->code->kind == Func) {
        if (is_in_func) {
            is_in_func = 0;
            fprintf(file, "\n");
        }

        init_func_var(file, ret);
    }

    if (ret && is_first_instr(ret)) {
        init_basic_block(ret);
    }

    return ret;
}

void gen_code(FILE *file, InterCodes *codes) {
	printf("in genecode**************\n");
    if (codes) { //make it not loop linked list
        InterCodes *tail = codes->prev;
        codes->prev = NULL;
        tail->next = NULL;
    }

    fprintf(file, ASSEM_HDR);

    init_func_var(file, codes);
    init_basic_block(codes);

    while (codes) {
        print_mips_code(file, codes);
        codes = codes_next(file, codes);
    }
}
