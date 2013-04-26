#ifndef HIVEK_ASSEMBLER_TABLE_H
#define HIVEK_ASSEMBLER_TABLE_H

namespace HivekAssembler {
    class Table {
        private:
            std::map<std::string, int> str2data_type;
            std::map<std::string, int> str2op;
            std::map<int, int> op2type;
            

        private:
            int address;
            std::map<std::string, int> branch_labels;
            std::vector<Data> data;
            std::vector<MultiInstruction> multiops;

        public:
            Table();

        public:
            void add_branch_label(const std::string& str);
            void add_data(std::string& n, std::string& t, std::string& v);

            void add_instruction (
                std::string& predicate, 
                std::string& operation,
                std::string& destination,
                std::string& operand1,
                std::string& operand2,
                std::string& shift_type,
                std::string& shamt
            );

            Data get_data_at(int i);
            int data_size();
    };
}

#endif
