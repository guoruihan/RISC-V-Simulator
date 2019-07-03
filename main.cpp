#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>
//#include "parser.hpp"
#include "memory.hpp"
using namespace std;



int reg[33];
int pc;
// 0-31
// reg0  : wired 0
// reg10 : output a0, last 8 bits
// 32    : pc, storing current pos in memory

memory mem;

int cnt = 0;



enum label {
    LUI, AUIPC,                                                   // U-type
    JAL,                                                          // J-type
    JALR,                                                         // I-type
    BEQ, BNE, BLT, BGE, BLTU, BGEU,                               // B-type
    LB, LH, LW, LBU, LHU,                                         // I-type
    ADDI, SLTI, SLTIU, XORI, ORI, ANDI,                           // I-type
    SB, SH, SW,                                                   // S-type
    SLLI, SRLI, SRAI,                                             // I-type-special
    ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND              // R-type
};

class Parser{
private:
    unsigned int inst;

    label get_label(){
        int op_code = inst & (0x7f);
        switch (op_code) {
            case 0x37 : return LUI;
            case 0x17 : return AUIPC;
            case 0x6f : return JAL;
            case 0x67 : return JALR;
            case 0x63 : {
                        int funct3 = (inst >> 12) & (0x7);
                        if (funct3 == 0x00) return BEQ;
                        if (funct3 == 0x01) return BNE;
                        if (funct3 == 0x04) return BLT;
                        if (funct3 == 0x05) return BGE;
                        if (funct3 == 0x06) return BLTU;
                        if (funct3 == 0x07) return BGEU;
                        break;
            }
            case 0x03 : {
                        int funct3 = (inst >> 12) & (0x7);
                        if (funct3 == 0x00) return LB;
                        if (funct3 == 0x01) return LH;
                        if (funct3 == 0x02) return LW;
                        if (funct3 == 0x04) return LBU;
                        if (funct3 == 0x05) return LHU;
                        break;
            }
            case 0x13 : {
                        int funct3 = (inst >> 12) & (0x7);
                        if (funct3 == 0x00) return ADDI;
                        if (funct3 == 0x02) return SLTI;
                        if (funct3 == 0x03) return SLTIU;
                        if (funct3 == 0x04) return XORI;
                        if (funct3 == 0x06) return ORI;
                        if (funct3 == 0x07) return ANDI;

                        if (funct3 == 0x01) return SLLI;
                        if (funct3 == 0x05) {
                            int funct7 = (inst >> 25);
                            if (funct7 == 0x00) return SRLI;
                            if (funct7 == 0x20) return SRAI;
                        }                        
                        break;
            }
            case 0x23 : {
                        int funct3 = (inst >> 12) & (0x7);
                        if (funct3 == 0x00) return SB;
                        if (funct3 == 0x01) return SH;
                        if (funct3 == 0x02) return SW;
                        break;
            }
            case 0x33 : {
                        int funct3 = (inst >> 12) & (0x7);
                        if (funct3 == 0x00){
                            int funct7 = (inst >> 25) & (0x7F);
                            if (funct7 == 0x00) return ADD;
                            if (funct7 == 0x20) return SUB;
                        }
                        if (funct3 == 0x01) return SLL;
                        if (funct3 == 0x02) return SLT;
                        if (funct3 == 0x03) return SLTU;
                        if (funct3 == 0x04) return XOR;
                        if (funct3 == 0x05) {
                            int funct7 = (inst >> 25) & (0x7F);
                            if (funct7 == 0x00) return SRL;
                            if (funct7 == 0x20) return SRA;
                        }
                        if (funct3 == 0x06) return OR;
                        if (funct3 == 0x07) return AND;
                        break;
            }
        }
    }

public:
    Parser(int _inst = 0){
        inst = _inst;
    }
    ~Parser(){
    }

    pair<char, label> parse(){
        label inst_label = get_label();
        switch (inst_label) {
            case LUI: case AUIPC: return pair<char, label>('U', inst_label);
            case JAL: return pair<char, label>('J', inst_label);
            case JALR:
            case LB: case LH: case LW: case LBU: case LHU:
            case ADDI: case SLTI: case SLTIU: case XORI: case ORI: case ANDI: 
            case SLLI: case SRLI: case SRAI: return pair<char, label>('I', inst_label);
            case BEQ: case BNE: case BLT: case BGE: case BLTU: case BGEU: return pair<char, label>('B', inst_label);
            case SB: case SH: case SW: return pair<char, label>('S', inst_label);
            case ADD: case SUB: case SLL: case SLT: case SLTU: case XOR: case SRL: case SRA: case OR: case AND: return pair<char, label>('R', inst_label);
        }
    }

    void run(){
        pair<char, label> inst_pair = parse();
        char inst_type = inst_pair.first;
        label inst_label = inst_pair.second;
        switch (inst_type) {
            case 'U' : {
                int rd = (inst >> 7) & (0x1F);
                if (inst_label == LUI) {
                    //int inst_31 = (inst >> 31), inst_20_30 = (inst >> 20) & (0x7FF), inst_12_19 = (inst >> 12) & (0xFF);
                    int inst1 = inst;
                    int imm = (inst1 >> 12) << 12;
                    reg[rd] = imm;


                    pc += 4;
                }
                if (inst_label == AUIPC) {
                    int inst1 = inst;
                    int imm = (inst1 >> 12) << 12;

                    pc += imm;
                    reg[rd] = pc;
                }
                break;
            }
            case 'J' : {
                int rd = (inst >> 7) & (0x1F);
                // int inst_31 = inst >> 31;
                // int inst_12_19 = (inst >> 12) & (0xFF);
                // int inst_20 = (inst >> 20) & (0x1);
                // int inst_25_30 = (inst >> 25) & (0x3F);
                // int inst_21_24 = (inst >> 21) & (0xF);
                // if (inst_31 == 1) inst_31 = (0xFFF) << 20;
                // inst_12_19 = inst_12_19 << 12;
                // inst_20 = inst_20 << 11;
                // inst_25_30 = inst_25_30 << 5;
                // inst_21_24 = inst_21_24 << 1;
                // int imm = inst_31 | inst_12_19 | inst_20 | inst_25_30 | inst_21_24;
                int imm;
                imm = -(inst >> 31) << 20;
                imm += ((inst << 1) >> 22) << 1;
                imm += ((inst << 11) >> 31) << 11;
                imm += ((inst << 12) >> 24) << 12;

                reg[rd] = pc + 0x4;

                pc += imm;
                break;
            }
            case 'I' : {
                int inst1 = inst;
                int imm = (inst1 >> 20);
                int rs1 = (inst >> 15) & (0x1F);
                int rd  = (inst >> 7) & (0x1F);
                if (inst_label == JALR) {
                    int inst1 = inst;
                    int imm = (inst1 >> 20) & (0x7FF);
                    int sign = imm << 11;
                    if (sign == 1) imm = imm | 0xFFFFF;
                    
                    reg[rd] = pc + 0x4;

                    pc = (reg[rs1] + imm) & (-2);
                   // cout << "JALR " << dec << pc << endl;
                }
                if (inst_label == LB) {
                    char t;

                    int add = reg[rs1] + imm;
                    memcpy(&t, mem.mem + add, 1);
                    //int t = mem.load_from_pos(reg[rs1] + imm, 1);
                    //int sign = t >> 7;
                    //if (sign == 1) t |= 0xFFFFFF00;
                    reg[rd] = t;

                    pc += 4;
                }
                if (inst_label == LH) {
                    short t;
                    int add = reg[rs1] + imm;
                    memcpy(&t, mem.mem + add, 2);
                    //int t = mem.load_from_pos(reg[rs1] + imm, 2);
                    //int sign = t >> 15;
                    //if (sign == 1) t |= 0xFFFF0000;
                    reg[rd] = t;

                    pc += 4;
                }
                if (inst_label == LW) {
                    int t;
                    int add = reg[rs1] + imm;
                    memcpy(&t, mem.mem + add, 4);
                    //int t = mem.load_from_pos(pos, 4);
                    //reg[rd] = t;
                    //mem.load_w(pos, reg[rd]);
                    reg[rd] = t;
                    
                    pc += 4;
                }
                if (inst_label == LBU){
                    unsigned char t;
                    int add = reg[rs1] + imm;
                    memcpy(&t, mem.mem + add, 1);
                    //int t = mem.load_from_pos(reg[rs1] + imm, 1);
                    reg[rd] = t;
                    
                    pc += 4;
                }
                if (inst_label == LHU){
                    unsigned short t;
                    int add = reg[rs1] + imm;
                    memcpy(&t, mem.mem + add, 2);
                    //int t = mem.load_from_pos(reg[rs1] + imm, 2);
                    reg[rd] = t;
                    
                    pc += 4;
                }
                if (inst_label == ADDI){
                    int t;
                    int sign = imm >> 11;
                    if (sign == 1) t = imm | 0xFFFFF000;
                    else t = imm;

                    reg[rd] = reg[rs1] + t;
                    
                    pc += 4;
                }
                if (inst_label == SLTI){
                    int t;
                    int sign = imm >> 11;
                    if (sign == 1) t = imm | 0xFFFFF000;
                    else t = imm;

                    if (reg[rs1] < t) reg[rd] = 1;
                    else reg[rd] = 0;
                    
                    pc += 4;
                }
                if (inst_label == SLTIU){
                    int t;
                    int sign = imm >> 11;
                    if (sign == 1) t = imm | 0xFFFFF000;
                    else t = imm;
                    unsigned int s1 = reg[rs1];
                    unsigned int s2 = t;
                    if (s1 < s2) reg[rd] = 1;
                    else reg[rd] = 0;
                    
                    pc += 4;
                }

                if (inst_label == XORI){
                    int t;
                    int sign = imm >> 11;
                    if (sign == 1) t = imm | 0xFFFFF000;
                    else t = imm;

                    reg[rd] = reg[rs1] ^ t;
                    
                    pc += 4;
                }
                if (inst_label == ORI){
                    int t;
                    int sign = imm >> 11;
                    if (sign == 1) t = imm | 0xFFFFF000;
                    else t = imm;

                    reg[rd] = reg[rs1] | t;
                    
                    pc += 4;
                }
                if (inst_label == ANDI){
                    int t;
                    int sign = imm >> 11;
                    if (sign == 1) t = imm | 0xFFFFF000;
                    else t = imm;

                    reg[rd] = reg[rs1] & t;
                    
                    pc += 4;
                }

                if (inst_label == SLLI){
                    unsigned int immu = imm;
                    int shift = (immu << 27) >> 27;

                    reg[rd] = reg[rs1] << shift;
                    
                    pc += 4;
                }
                if (inst_label == SRLI){
                    unsigned int immu = imm;
                    unsigned int s1 = reg[rs1];
                    int shift = (immu << 27) >> 27;
                    reg[rd] = s1 >> shift;
                    
                    pc += 4;
                }
                if (inst_label == SRAI) {
                    unsigned int immu = imm;
                    int shift = (immu << 27) >> 27;
                    reg[rd] = reg[rs1] >> shift;
                    pc += 4; 
                }
                break;
            }
            case 'B' : {
                int rs1 = (inst >> 15) & (0x1F), rs2 = (inst >> 20) & (0x1F);
                // int imm_11 = ((inst >> 7) & (0x1)) << 11;
                // int imm_1_4 = ((inst >> 8) & (0xF)) << 1;
                // int imm_12 = -(inst >> 31) << 12;
                // int imm_5_10 = ((inst >> 25) & (0x3F)) << 5;
                // int imm = imm_1_4 | imm_5_10 | imm_11 | imm_12;
                int imm = -(inst >> 31) << 12;
                imm += ((inst << 1) >> 26) << 5;
                imm += ((inst << 20) >> 28) << 1;
                imm += ((inst << 24) >> 31) << 11;

                if (inst_label == BEQ) 
                    if (reg[rs1] == reg[rs2]) pc += imm;
                    else pc += 4;

                if (inst_label == BNE)
                    if (reg[rs1] != reg[rs2]) pc += imm;
                    else pc += 4;
// to fix
                if (inst_label == BLT){
                    // signed compare
                    int s1 = reg[rs1], s2 = reg[rs2];
                    if (s1 < s2) pc += imm;
                    else pc += 4;
                }

                if (inst_label == BLTU) {
                    // unsigned compare
                    unsigned int s1 = reg[rs1];
                    unsigned int s2 = reg[rs2];
                    if (s1 < s2) pc += imm;
                    else pc += 4;
                }
                if (inst_label == BGE) {
                    // signed compare
                    int s1 = reg[rs1], s2 = reg[rs2];
                    if (s1 >= s2) pc += imm;
                    else pc += 4;
                }
                if (inst_label == BGEU) {
                    // unsigned compare
                    unsigned int s1 = reg[rs1];
                    unsigned int s2 = reg[rs2];
                    if (s1 >= s2) pc += imm;
                    else pc += 4;
                }
//
                break;
            }
            case 'S' : {
                int inst1 = inst;
                //int imm1 = (inst1 >> 25) & (0x7F), imm2 = (inst1 >> 7) & (0x1F);
                //int imm = imm1 << 5 | imm2;
                int imm = (inst1 >> 25) << 5;
                imm += (inst << 20) >> 27;

                int rs1 = (inst >> 15) & (0x1F), rs2 = (inst >> 20) & (0x1F);
                if (inst_label == SW)  {
                    int x = reg[rs2];
                    int add = imm + reg[rs1];
                    mem.write_w(add, x);
                    pc += 4;
                }
                if (inst_label == SH) {
                    short x = reg[rs2];
                    int add = imm + reg[rs1];
                    mem.write_h(add, x);
                    pc += 4;
                }
                if (inst_label == SB){
                    char x = reg[rs2];
                    int add = imm + reg[rs1];
                    mem.write_b(add, x);
                    pc += 4;
                }
                break;
            }
            case 'R' : {
                int rs1 = (inst >> 15) & (0x1F), rs2 = (inst >> 20) & (0x1F), rd = (inst >> 7) & (0x1F);
                if (inst_label == ADD) reg[rd] = reg[rs1] + reg[rs2];
                if (inst_label == SUB) reg[rd] = reg[rs1] - reg[rs2];
                if (inst_label == AND) reg[rd] = reg[rs1] & reg[rs2];
                if (inst_label == OR)  reg[rd] = reg[rs1] | reg[rs2];
                if (inst_label == XOR) reg[rd] = reg[rs1] ^ reg[rs2];
                if (inst_label == SLT) {
                    if (reg[rs1] < reg[rs2]) reg[rd] = 1;
                    else reg[rd] = 0;
                }
                if (inst_label == SLTU) {
                    // unsigned compare
                    unsigned int s1 = reg[rs1];
                    unsigned int s2 = reg[rs2];
                    if (s1 < s2) reg[rd] = 1;
                    else reg[rd] = 0;
                }
                if (inst_label == SLL) {
                    //int shift = reg[rs2] & 0x1F;
                    reg[rd] = reg[rs1] << reg[rs2];
                }
                if (inst_label == SRL) {
                    unsigned int s1 = reg[rs1];
                    unsigned int s2 = reg[rs2];
                    reg[rd] = s1 >> s2;
                }
                if (inst_label == SRA) {
                    reg[rd] = reg[rs1] >> reg[rs2];
                }
                pc += 0x4;
                break;
            }
        }
        reg[0] = 0;
    }

    void load_inst(unsigned int d){
        inst = d;
    }

    int get_inst(){
        return inst;
    }
};

int main(){
    //Parser a(0x040010ef);
    //a.run();
    mem.initialize_mem();
    //mem.traverse();
    pc = 0;
    mem.change_pos(0x0);

    Parser parser;
    while (parser.get_inst() != 0x00c68223){
        ++cnt;
       // cout << dec << cnt << endl;
        // if ((cnt) % 50 == 0) {
        //     cout << "pause " << endl;
        // }
        //if (pc == 4184) getchar();
        mem.change_pos(pc);
        unsigned int inst = mem.load_word();
        //cout << "dec : " << dec << inst << endl;
        //cout << "inst is : " << hex << inst << endl;
        parser.load_inst(inst);
        parser.run();
    }   
    cout << dec << (reg[10] & 255u) << endl;
   // system("pause");
    return 0;
}