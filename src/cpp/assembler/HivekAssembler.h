#include <vector>
#include <stack>
#include <map>
#include <iostream>
#include <fstream>

namespace HivekAssembler {
    enum MultiOpSize {
        MULTI_OP1x16, MULTI_OP1x32, MULTI_OP2x32, MULTI_OP2x16 
    };

    enum DataType { 
        DATA_ASCII, DATA_32BITS 
    };

    enum OperationFunc { 
        // TYPE_I
        ADD, SUB, ADC, SBC, ADDS, ADCS, SUBS, SBCS, 
        AND, OR, NOR, XOR,
        SLL, SRL, SRA,
        LWR, SWR, LBR, SBR,
        CMPEQ, CMPLT, CMPGT, CMPLTU, CMPGTU,
        ANDP, ORP, XORP, NORP,

        //TYPE_Ib
        JR, JALR,

        // TYPE_II
        ADDI, ADCI, ADDIS, ADCIS,
        ANDI, ORI,
        CMPEQI, CMPLTI, CMPGTI, CMPLTUI, CMPGTUI,
        LW, SW, LB, SB,

        // TYPE_III
        JC, JALC,

        // TYPE_IV
        J, JAL

    };

    enum OperationType { 
        TYPE_I, TYPE_II, TYPE_III, TYPE_IV, TYPE_V,
        TYPE_Ib 
    };

    class Data;
    class Operation;
    class MultiOperation;
    class Table;
    class Assembler;
    class CodeGen;
}