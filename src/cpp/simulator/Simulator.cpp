#include "HivekSimulator.h"

namespace HivekSimulator {

    uint8_t Simulator::str2byte(std::string& str) {
        uint8_t v = 0;

        for (int i = 7; i >= 0; --i) {
            v |= (str[7 - i] - '0') << i;
        }

        return v;
    }

    void Simulator::open(char* filename) {
        uint8_t v;
        std::string str;
        std::ifstream file;

        file.open(filename);

        file >> str;

        if (str.compare(".text") == 0) {
            while (file.good()) {
                file >> str;

                if (str.compare(".data") == 0) {
                    break;
                }

                v = str2byte(str);
                instruction_memory.push_back(v);
            }
        }

        while (file.good()) {
            file >> str;
            v = str2byte(str);
            data_memory.push_back(v);
        }

        data_memory.resize(200 * 200);
        file.close();
    }

    void Simulator::print_registers() {
        std::cout << "pc: " << pc << std::endl;

        for (int i = 0; i < 32; ++i) {
            std::cout << "regs[" << i << "]: " << regs[i] << std::endl;
        }

        std::cout << "pr_reg[0]: " << pr_regs[0] << std::endl;
        std::cout << "pr_reg[1]: " << pr_regs[1] << std::endl;
        std::cout << "pr_reg[2]: " << pr_regs[2] << std::endl;
        std::cout << "pr_reg[3]: " << pr_regs[3] << std::endl;
    }

    void Simulator::fetch_instructions() {
        instruction1  = instruction_memory[pc] << 24;
        instruction1 |= instruction_memory[pc + 1] << 16;
        instruction1 |= instruction_memory[pc + 2] << 8;
        instruction1 |= instruction_memory[pc + 3];

        instruction2  = instruction_memory[pc + 4] << 24;
        instruction2 |= instruction_memory[pc + 5] << 16;
        instruction2 |= instruction_memory[pc + 6] << 8;
        instruction2 |= instruction_memory[pc + 7];

        sz     = new_sz;
        new_sz = (instruction1 & 0xC0000000) >> 30;

        switch (sz) {
            case 0: 
                pc += 2; break;
            case 1:
            case 2:
                pc += 4; break;
            case 3:
                pc += 8; break;
            default:
                break;
        }
    }

    void Simulator::update_status() {
    }

    void Simulator::decode_instructions() {
        if (sz == 0) {
            instruction1 = expand(instruction1 >> 16);
            instruction2 = NOP;
        } else if (sz == 2) {
            instruction2 = expand(instruction1 & 0x0FFFF);
            instruction1 = expand(instruction1 >> 16);
        } else if (sz == 1) {
            instruction2 = NOP;
        }
    }

    uint32_t Simulator::get_part(uint32_t operation, int end, int begin) {
        uint32_t mask = 0;

        for (int i = begin; i <= end; ++i) {
            mask |= (1 << i);
        }

        return (operation & mask) >> begin;
    }

    uint32_t Simulator::expand(uint32_t operation) {
        uint32_t predict;
        uint32_t itype  ;
        uint32_t opcode ;
        uint32_t immd9  ;
        uint32_t immd5  ;
        uint32_t cond   ;

        uint32_t reg_a3 ;
        uint32_t reg_b3 ;
        uint32_t reg_c3 ;

        uint32_t reg_a3b;
        uint32_t reg_b3b;
        uint32_t reg_c3b;

        uint32_t reg_a3c;
        uint32_t reg_b3c;

        uint32_t reg_b3i;

        uint32_t immd12 ;
        uint32_t immd22 ;

        uint32_t op;
        uint32_t dout;

        itype   = get_part(operation, 13, 13); 
        predict = get_part(operation, 12, 12); 
        opcode  = get_part(operation, 12, 9);
        immd9   = get_part(operation, 8, 0);
        immd5   = get_part(operation, 4, 0);
        cond    = get_part(operation, 11, 9);

        reg_a3 = get_part(operation, 2, 0); 
        reg_b3 = get_part(operation, 5, 3); 
        reg_c3 = get_part(operation, 8, 6); 

        reg_a3b = get_part(operation, 2, 0) + 8; 
        reg_b3b = get_part(operation, 5, 3) + 8; 
        reg_c3b = get_part(operation, 8, 6) + 8; 

        reg_b3i = get_part(operation, 8, 5);
        reg_a3c = get_part(operation, 3, 0); 
        reg_b3c = get_part(operation, 8, 4);

        if (immd5 & (1 << 4)) {
            immd12 = (get_part(~0, 11, 5) << 5) | immd5;
        } else {
            immd12 = immd5;
        }

        if (immd9 & (1 << 8)) {
            immd22 = (get_part(~0, 21, 9) << 9) | immd9;
        } else {
            immd22 = immd9;
        }

        if (itype == 1) {
            dout = (12 << 26) | (predict << 25) | (immd22 << 3) | cond;
            return dout;
        } else {
            op = 4;

            switch (opcode) {
                case 0: //OP_ADD_16
                    op |= (reg_c3 << 13) | (reg_b3 << 8) | (reg_a3 << 3);
                    op |=  (112 << 23) | (0 << 18); //OP_ADD
                    break;

                case 1: //OP_SUB_16
                    op |= (reg_c3 << 13) | (reg_b3 << 8) | (reg_a3 << 3);
                    op |=  (112 << 23) | (1 << 18); //OP_ADD
                    break;

                case 2: //and
                    op |= (reg_c3 << 13) | (reg_b3 << 8) | (reg_a3 << 3);
                    op |=  (112 << 23) | (4 << 18); //OP_ADD
                    break;

                case 3: //or
                    op |= (reg_c3 << 13) | (reg_b3 << 8) | (reg_a3 << 3);
                    op |=  (112 << 23) | (5 << 18); //OP_ADD
                    break;

                case 4: //cmpeq
                    op |= (reg_c3 << 13) | (reg_b3 << 8) | (reg_a3 << 3);
                    op |=  (112 << 23) | (11 << 18); //OP_ADD
                    break;

                case 5: //cmplt
                    op |= (reg_c3 << 13) | (reg_b3 << 8) | (reg_a3 << 3);
                    op |=  (112 << 23) | (12 << 18); //OP_ADD
                    break;

                case 6: //cmpgt
                    op |= (reg_c3 << 13) | (reg_b3 << 8) | (reg_a3 << 3);
                    op |=  (112 << 23) | (14 << 18); //OP_ADD
                    break;

                case 7: //addhi
                    op |= (reg_c3b << 13) | (reg_b3b << 8) | (reg_a3b << 3);
                    op |=  (112 << 23) | (0 << 18); //OP_ADD
                    break;

                case 8: //subhi
                    op |= (reg_c3b << 13) | (reg_b3b << 8) | (reg_a3b << 3);
                    op |=  (112 << 23) | (1 << 18); //OP_ADD
                    break;

                case 9://addi
                    op |= (immd12 << 13) | (reg_b3i << 8) | (reg_b3i << 3);
                    op |= (0 << 25);
                    break;

                case 10://movi
                    op |= (immd12 << 13) | (reg_b3i << 8) | (0 << 3);
                    op |= (0 << 25);
                    break;

                case 11: //lw_sp
                    op |= (immd12 << 13) | (reg_b3i << 8) | (29 << 3);
                    op |= (9 << 25);
                    break;

                case 12: //sw_sp
                    op |= (immd12 << 13) | (reg_b3i << 8) | (29 << 3);
                    op |= (11 << 25);
                    break;

                case 13: //lw
                    op |= (0 << 13) | (reg_b3c << 8) | (reg_a3c << 3);
                    op |= (9 << 25);
                    break;

                case 14: //sw
                    op |= (0 << 13) | (reg_b3i << 8) | (reg_a3c << 3);
                    op |= (11 << 25);
                    break;

                case 15:
                    op |= (reg_b3c << 13) | (0 << 8) | (reg_a3c << 3);
                    op |=  (112 << 23) | (0 << 18); //OP_ADD
                    break;

            }

            return op;
        }
    }

    void Simulator::execute_instructions() {
        execute(instruction1, 0);
        execute(instruction2, 1);
    }

    void Simulator::execute(uint32_t n, int u) {
        int operation;
        int p_reg = n & 0x03;
        int p_value = (n & 0x04) >> 2;
        int rs = (n & 0x0F8) >> 3;
        int rt = (n & 0x01F00) >> 8;
        int rd = (n & 0x03E000) >> 13;
        int immd12 = (n & 0x01FFE000) >> 13;
        int immd22 = (n & 0x01FFFFF8) >> 3;
        int immd27 = n & 0x07FFFFFF;
        bool exec = pr_regs[p_reg] == p_value;
        bool shadd_flag = false;
        int sh_amt;
        int sh_t;

        // type I
        if ((0x3E000000 & n) == 0x38000000) {
            operation = (n & 0x01FC0000) >> 18;

            if ((operation & 0x060) != 0) {
                sh_amt = operation & 31;
                sh_t = (operation & 0x060) >> 5;
                shadd_flag = true;
            }
        }
        // immediates - type II
        else if ((0x20000000 & n) == 0) { 
            operation = (n & 0x1E000000) >> 25;
            operation += 1000;
        // conditional jumps
        } else if ((0x38000000 & n) == 0x30000000) {
            operation = (n & 0x06000000) >> 25;
            operation += 2000;
        // unconditional jumps
        } else if ((0x30000000 & n) == 0x20000000) { 
            operation = 3000;
            operation += ((n & 0x08000000) ? 1 : 0);
            exec = true;
        }


        if (!exec) {
            return;
        }

        if (shadd_flag) {
            if (sh_t == 1) {
                regs[rd] = (regs[rs] << sh_amt) + regs[rt];
            } else if (sh_t == 2) {
                regs[rd] = ((unsigned) regs[rs] >> sh_amt) + regs[rt];
            } else {
                regs[rd] = (((signed) regs[rs]) >> sh_amt) + regs[rt];
            }

            return;
        }

        switch (operation) {
            case ADD:
                regs[rd] = regs[rs] + regs[rt];
                break;

            case SUB:
                regs[rd] = regs[rs] + ~regs[rt] + 1;
                break;

            case ADC:
                regs[rd] = regs[rs] + regs[rt];
                carry[u] = regs[rd] < regs[rs];
                break;

            case AND:
                regs[rd] = regs[rs] & regs[rt];
                break;

            case OR:
                regs[rd] = regs[rs] | regs[rt];
                break;

            case NOR:
                regs[rd] = ~(regs[rs] | regs[rt]);
                break;

            case XOR:
                regs[rd] = regs[rs] ^ regs[rt];
                break;

            case SLLV:
                regs[rd] = regs[rs] << (regs[rt] & 0x01F);
                break;

            case SRLV:  
                regs[rd] = ((unsigned) regs[rs]) >> (regs[rt] & 0x01F);
                break;

            case SRAV:
                regs[rd] = ((signed) regs[rs]) >> (regs[rt] & 0x01F);
                break;

            case CMPEQ:
                pr_regs[rd] = cmpeq(regs[rs], regs[rt]);
                pr_regs[rd] = pr_regs[rd] || rd == 0;
                break;

            case CMPLT:
                pr_regs[rd] = cmplt(regs[rs], regs[rt]);
                pr_regs[rd] = pr_regs[rd] || rd == 0;
                break;

            case CMPGT:
                pr_regs[rd] = cmpgt(regs[rs], regs[rt]);
                pr_regs[rd] = pr_regs[rd] || rd == 0;
                break;

            case CMPLTU:
                pr_regs[rd] = cmpltu(regs[rs], regs[rt]);
                pr_regs[rd] = pr_regs[rd] || rd == 0;
                break;

            case CMPGTU:
                pr_regs[rd] = cmpgtu(regs[rs], regs[rt]);
                pr_regs[rd] = pr_regs[rd] || rd == 0;
                break;

            case ANDP:
                pr_regs[rd] = pr_regs[rs] && pr_regs[rt];
                break;

            case ORP:
                pr_regs[rd] = pr_regs[rs] || pr_regs[rt];
                break;

            case XORP:
                pr_regs[rd] = (int) pr_regs[rs] ^ (int) pr_regs[rt];
                break;

            case NORP:
                pr_regs[rd] = ! (pr_regs[rs] || pr_regs[rt]);
                break;

            case JR:
                new_sz = 3;
                pc = regs[rt];
                break;

            case JALR:
                new_sz = 3;
                regs[31] = pc;
                pc = regs[rt];
                break;

        /* immediates */
            case ADDI:
                regs[rt] = regs[rs] + sign_ext(immd12, 12);
                break;

            case ADCI:
                regs[rt] = regs[rs] + sign_ext(immd12, 12);
                break;

            case ANDI:
                regs[rt] = regs[rs] & sign_ext(immd12, 12);
                break;

            case ORI:
                regs[rt] = regs[rs] | sign_ext(immd12, 12);
                break;

            case CMPEQI:
                pr_regs[rt] = cmpeq(regs[rs], sign_ext(immd12, 12));
                pr_regs[rt] = pr_regs[rt] || rt == 0;
                break;

            case CMPLTI:
                pr_regs[rt] = cmplt(regs[rs], sign_ext(immd12, 12));
                pr_regs[rt] = pr_regs[rt] || rt == 0;
                break;

            case CMPGTI:
                pr_regs[rt] = cmpgt(regs[rs], sign_ext(immd12, 12));
                pr_regs[rt] = pr_regs[rt] || rt == 0;
                break;

            case CMPLTUI:
                pr_regs[rt] = cmpltu(regs[rs], sign_ext(immd12, 12));
                pr_regs[rt] = pr_regs[rt] || rt == 0;
                break;

            case CMPGTUI:
                pr_regs[rt] = cmpgtu(regs[rs], sign_ext(immd12, 12));
                pr_regs[rt] = pr_regs[rt] || rt == 0;
                break;

            case LW:
                regs[rt] = read_dmem(regs[rs] + sign_ext(immd12, 12), 32);
                break;

             case LB:
                regs[rt] = read_dmem(regs[rs] + sign_ext(immd12, 12), 8);
                break;

            case SW: 
                write_dmem(regs[rt], regs[rs] + sign_ext(immd12, 12), 32);
                break;

             case SB:
                write_dmem(regs[rt], regs[rs] + sign_ext(immd12, 12), 8);
                break;
         
            // conditional jumps
            case JC:
            case JCN:
                new_sz = 3;
                pc += sign_ext(immd22 << 1, 22);
                break;

            case JALC:
            case JALCN:
                new_sz = 3;
                regs[31] = pc;
                pc += sign_ext(immd22 << 1, 22);
                break;

            case J:
                new_sz = 3;
                pc += sign_ext(immd27 << 1, 27);
                break;

            case JAL: 
                new_sz = 3;
                regs[31] = pc;
                pc += sign_ext(immd27 << 1, 27);
                break;

            default:
                break;
        }
    }

    uint32_t Simulator::sign_ext(uint32_t v, int pos) {
        if ((1 << (pos - 1)) & v) {
            return ((0xFFFFFFFF >> pos) << pos) | v;
        } 

        return v;
    }

    bool Simulator::cmpeq(uint32_t a, uint32_t b) {
        return a == b;
    }

    bool Simulator::cmplt(int32_t a, int32_t b) {
        return a < b;
    }

    bool Simulator::cmpgt(int32_t a, int32_t b) {
        return a > b;
    }

    bool Simulator::cmpltu(uint32_t a, uint32_t b) {
        return a < b;
    }

    bool Simulator::cmpgtu(uint32_t a, uint32_t b) {
        return a > b;
    }

    void Simulator::write_dmem(uint32_t v, uint32_t address, int bits) {
        if (bits == 32) {
            address *= 4;
            data_memory[address]     = (v & 0xFF000000) >> 24;
            data_memory[address + 1] = (v & 0x00FF0000) >> 16;
            data_memory[address + 2] = (v & 0x0000FF00) >> 8;
            data_memory[address + 3] = (v & 0x000000FF);
        } else if (bits == 8) {
            data_memory[address] = (v & 0x000000FF);
        }
    }

    uint32_t Simulator::read_dmem(uint32_t address, int bits) {
        uint32_t v = 0;

        if (bits == 32) {
            address *= 4;
            v |= data_memory[address] << 24;
            v |= data_memory[address + 1] << 16;
            v |= data_memory[address + 2] << 8;
            v |= data_memory[address + 3];
        } else if (bits == 8) {
            v |= data_memory[address];
        }

        return v;
    }

    void Simulator::run() {
//        int a;
        while (regs[30] == 0) {
            fetch_instructions(); 
            decode_instructions();
            execute_instructions();
            update_status();
            //print_registers();
  //          std::cin >> a;
        }

        //dump_memory();
    }

    void Simulator::reset() {
        pc = 0;
        new_pc = 8;
        sz = 3;
        new_sz = 3;
        j_taken = false;

        for (int i = 0; i < 32; ++i) {
            regs[i] = 0;
        }

        pr_regs[0] = true;
        pr_regs[1] = false;
        pr_regs[2] = false;
        pr_regs[3] = false;

    }

    Simulator::Simulator() {
        reset();
    }

    void Simulator::dump_memory() {
        int t;
        std::cout << "P3\n320 240\n255\n";

        for (int i = 0; i < 2400 * 4; i += 4) {
            t = read_dmem(i, 32);

            for (int j = 31; j >= 0; --j) {
                if (t & (1 << j)) {
                    std::cout << "0 0 0\n";
                } else {
                    std::cout << "255 255 255\n";
                }
            }
        }
    }

}

