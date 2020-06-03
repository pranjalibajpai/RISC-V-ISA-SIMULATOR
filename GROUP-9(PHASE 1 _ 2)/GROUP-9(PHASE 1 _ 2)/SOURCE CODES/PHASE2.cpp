#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstring>
#include <math.h>

using namespace std;
unsigned int r[32];
string mem[6000];
//string stck[2000];
string heap[800];
vector<string> instr;
int cpc = 0;
string pc;
unsigned int operand1, operand2;

class decode_returns
{
public:
    string operation_type;
    int rA, rB;
    int destination;
    int immediate_value;

    decode_returns()
    {

        operation_type = "";
        rA = -1;
        rB = -1;
        destination = -1;
        immediate_value = -1;
    }
};
class execute_returns
{
public:
    string operation_type;
    int destination;
    int rregno_rd;
    execute_returns()
    {

        operation_type = "";
        destination = -1;
        rregno_rd = -1;
    }
};
class memory_returns
{
public:
    string operation_type;
    int destination;
    int rregno_rd;
    int to_update_pc;
    int lregno_rd;

    memory_returns()
    {

        operation_type = "";
        destination = -1;
        rregno_rd = -1;
        to_update_pc = -1;
        lregno_rd = -1;
    }
};
string HexToBin(string hexdec)
{
    long int i = 0;
    string out = "";
    while (hexdec[i])
    {

        switch (hexdec[i])
        {
        case '0':
            out = out + "0000";
            break;
        case '1':
            out = out + "0001";
            break;
        case '2':
            out = out + "0010";
            break;
        case '3':
            out = out + "0011";
            break;
        case '4':
            out = out + "0100";
            break;
        case '5':
            out = out + "0101";
            break;
        case '6':
            out = out + "0110";
            break;
        case '7':
            out = out + "0111";
            break;
        case '8':
            out = out + "1000";
            break;
        case '9':
            out = out + "1001";
            break;
        case 'A':
        case 'a':
            out = out + "1010";
            break;
        case 'B':
        case 'b':
            out = out + "1011";
            break;
        case 'C':
        case 'c':
            out = out + "1100";
            break;
        case 'D':
        case 'd':
            out = out + "1101";
            break;
        case 'E':
        case 'e':
            out = out + "1110";
            break;
        case 'F':
        case 'f':
            out = out + "1111";
            break;
        default:
            cout << "\nInvalid hexadecimal digit "
                 << hexdec[i];
        }
        i++;
    }
    return out;
}
int bin_to_dec(string b)
{
    int num = 0, x = b.length() - 1;
    string::iterator i;
    
    for (i = b.begin(); i != b.end(); i++)
    {
        num += (*i - 48) * pow(2, x);
        x--;
    }
    return num;
}
string findTwoscomplement(string str)
{
    int n = str.length();
    int i;
    for (i = n - 1; i >= 0; i--)
        if (str[i] == '1')
            break;
    if (i == -1)
        return '1' + str;
    for (int k = i - 1; k >= 0; k--)
    {
        if (str[k] == '1')
            str[k] = '0';
        else
            str[k] = '1';
    }
    return str;
    ;
}
string find_operation(string opcode, string funct3, string funct7)
{
    string find_op;
    if (opcode == "0110011") //R format
        find_op = funct7 + funct3 + opcode;

    else if (opcode == "1101111" || opcode == "0010111" || opcode == "0110111") // U and UJ format
        find_op = opcode;

    else
        find_op = funct3 + opcode;

    ifstream f1;
    f1.open("operation.txt");
    map<string, string> op;
    string code, operate, ans;
    while (!f1.eof())
    {
        f1 >> code >> operate;
        op.insert(pair<string, string>(code, operate));
    }
    ans = op.find(find_op)->second;
    return ans;
}
decode_returns decode(string mach_bin)
{

    string rd, rs1, rs2, imme;
    string opcode, funct3, funct7;
    int a, b, imm, ra, rb, regno_rd;

    decode_returns ans; // return structure

    opcode = mach_bin.substr(25, 7);

    if (opcode == "1101111" || opcode == "0010111" || opcode == "0110111")
        ; //jal,auipc,lui
    else
    {
        funct3 = mach_bin.substr(17, 3);
    }
    if (opcode == "0110011") //R format
        funct7 = mach_bin.substr(0, 7);

    string operation = find_operation(opcode, funct3, funct7);

    if (opcode == "0110011") //R format
    {
        rs1 = mach_bin.substr(12, 5);
        rs2 = mach_bin.substr(7, 5);
        rd = mach_bin.substr(20, 5);

        regno_rd = bin_to_dec(rd);
        a = bin_to_dec(rs1);
        b = bin_to_dec(rs2);

        ra = r[a];
        rb = r[b];

        cout << "\n\nDecode: Operation is " << operation << "\nFirst operand: x" << a << "\nSecond operand: x" << b << "\nDestination register: x" << regno_rd;
        cout << "\nRead registers x" << a << " =" << ra << "\t x" << b << " =" << rb;

        ans.operation_type = operation;
        ans.rA = ra;
        ans.rB = rb;
        ans.destination = regno_rd;
    }
    else if (opcode == "0010011" || opcode == "0000011" || opcode == "1100111") //I format
    {
        rs1 = mach_bin.substr(12, 5);
        imme = mach_bin.substr(0, 12);
        rd = mach_bin.substr(20, 5);
        regno_rd = bin_to_dec(rd);
        a = bin_to_dec(rs1);
        char sign_bit = imme[0];
        if (sign_bit == '1')
        {
            imme = findTwoscomplement(imme);
            b = bin_to_dec(imme);
            b *= -1;
        }
        else
            b = bin_to_dec(imme);
        ra = r[a];
        rb = b;
        cout << "\n\nDecode: Operation is " << operation << "\nFirst operand: x" << a << "\nImmediate value:" << b << "\nDestination register: x" << regno_rd;
        cout << "\nRead registers x" << a << " =" << ra;

        ans.operation_type = operation;
        ans.rA = ra;
        ans.destination = regno_rd;
        // ans.rB=b;
        ans.immediate_value = b;
    }
    else if (opcode == "0100011")
    { //S format  sw x3 offset(x6)

        rs1 = mach_bin.substr(12, 5);
        rs2 = mach_bin.substr(7, 5);
        string imme1 = mach_bin.substr(0, 7);  //imme [11:5]
        string imme2 = mach_bin.substr(20, 5); //imme[4:0]
        imme = imme1 + imme2;
        a = bin_to_dec(rs1);
        b = bin_to_dec(rs2); //x3
        ra = r[a];           //x6
        int imm;
        char sign_bit = imme[0];
        if (sign_bit == '1')
        {
            imme = findTwoscomplement(imme);
            imm = bin_to_dec(imme);
            imm *= -1;
        }
        else
            imm = bin_to_dec(imme);
        cout << "\n\nDecode: Operation is " << operation << "\n First operand x" << a << ", Offset " << imm << "\n Destination register x" << b;
        cout << "\nRead registers x" << a << " =" << ra;

        ans.operation_type = operation;
        ans.rA = ra;
        ans.destination = b;
        ans.immediate_value = imm;
    }
    else if (opcode == "1100011")
    { //SB be x2 x3 L
        string imme12, imme10_5, imme11, imme4_1;
        rs1 = mach_bin.substr(12, 5);
        rs2 = mach_bin.substr(7, 5);
        imme12 = mach_bin[0];             //imme [12]
        imme10_5 = mach_bin.substr(1, 6); //imme[10:5]
        imme4_1 = mach_bin.substr(20, 4); //ime[4:1]
        imme11 = mach_bin[24];
        string imme = imme12 + imme11 + imme10_5 + imme4_1;
        a = bin_to_dec(rs1);
        b = bin_to_dec(rs2);
        ra = r[a];
        rb = r[b];
        int imm;
        char sign_bit = imme[0];
        if (sign_bit == '1')
        {
            imme = findTwoscomplement(imme);
            imm = bin_to_dec(imme);
            imm *= -1;
        }
        else
            imm = bin_to_dec(imme);
       // cout << imm << endl;
        imm *= 2;
       /// cout << imm;
        cout << "\n\nDecode: Operation is " << operation << "\nFirst operand: x" << a << "\nSecond operand: x" << b << "\nImmediate value: " << imm;
        ans.operation_type = operation;
        ans.rA = ra;
        ans.rB = rb;
        ans.immediate_value = imm;
    }
    else if (opcode == "1101111")
    { //UJ format jal x1 L

        rd = mach_bin.substr(20, 5);
        regno_rd = bin_to_dec(rd);

        string imme_20, imme_11, imme_10_to_1, imme_19_to_12;
        imme_20 = mach_bin[0];                  // imme[20]
        imme_10_to_1 = mach_bin.substr(1, 10);  // imme[10:1]
        imme_11 = mach_bin[11];                 //imme[11]
        imme_19_to_12 = mach_bin.substr(12, 8); //imme[19:12]
        imme = imme_20 + imme_19_to_12 + imme_11 + imme_10_to_1;
        char sign_bit = imme[0];
        if (sign_bit == '1')
        {
            imme = findTwoscomplement(imme);
            imm = bin_to_dec(imme);
            imm *= -1;
        }
        else
            imm = bin_to_dec(imme);
        imm *= 2;
        cout << "\n\nDecode: Operation is " << operation << "\n Jump: " << imm << "\n Return register x" << regno_rd << endl;
        ans.operation_type = operation;
        ans.immediate_value = imm;
        ans.destination = regno_rd;
    }
    else if (opcode == "0110111" || opcode == "0010111")
    { // U format lui,auipc

        rd = mach_bin.substr(20, 5);
        regno_rd = bin_to_dec(rd);
        imme = mach_bin.substr(0, 20);
        imm = bin_to_dec(imme);
        imm *= pow(2, 12);
        cout << "\n\nDecode: Operation is " << operation << "\n Upper immediate: " << imm << "\n Return register x" << regno_rd << endl;
        ans.operation_type = operation;
        ans.immediate_value = imm;
        ans.destination = regno_rd;
    }
    return ans;
}
string fetch(int pc)
{

    string inst, mach_bin, opcode;
    // istringstream s(l);
    // s>>pc;
    // s>>inst;
    //inst=inst.substr(2,8);
    inst = instr[pc];
    // pc=pc.substr(2,pc.length()-2);

    mach_bin = HexToBin(inst);
    stringstream hexc;
    hexc << hex << pc * 4;
    string pch;
    hexc >> pch;

    cout << "\nFetch instruction 0x" << inst << " from address 0x" << pch << " ## "
         << "\t";
    opcode = mach_bin.substr(25, 7);

    if (opcode == "1100011")
        ; //SB format
    else if (opcode == "1101111")
        ; //jal UJ format
    else if (opcode == "1100111")
        ; //jalr
    else
        cpc += 4; //next pc

    return mach_bin;
}
execute_returns execute(decode_returns ans)
{

    execute_returns execute_ans;
    string operation = ans.operation_type;
    int ra = ans.rA;
    int rb = ans.rB;
    int regno_rd = ans.destination;
    int imme = ans.immediate_value;

    int rregno_rd = -1;
    if (operation == "add")
    {
        rregno_rd = ra + rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "and")
    {
        rregno_rd = ra & rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "or")
    {
        rregno_rd = ra | rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "sll")
    {
        rregno_rd = ra << rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "slt")
    {
        rregno_rd = (ra < rb) ? 1 : 0;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "sra")
    {
        string new_string = "";
        string i_bit = "-1";
        if (ra >= 0)
            i_bit = "0";
        else
            i_bit = "1";
        int temp_rb = rb % 32;
        if (rb < 0)
        {
            temp_rb = temp_rb + 32;
        }
        stringstream sas;
        sas << hex << ra;
        string res = sas.str();
        string res_bin = HexToBin(res);
        new_string = res_bin.substr(0, (32 - temp_rb));
        while (temp_rb != 0)
        {
            new_string = i_bit + new_string;
            temp_rb--;
        }
        if (i_bit == "0")
        {
            rregno_rd = bin_to_dec(new_string);
        }
        else
        {
            new_string = findTwoscomplement(new_string);
            rregno_rd = 0 - bin_to_dec(new_string);
        }
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "srl")
    {
        rregno_rd = ra >> rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "sub")
    {
        rregno_rd = ra - rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "xor")
    {
        rregno_rd = ra ^ rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "mul")
    {
        rregno_rd = ra * rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "div")
    {
        rregno_rd = ra / rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "rem")
    {
        rregno_rd = ra % rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }

    else if (operation == "addi")
    {
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " " << ra << " and " << imme << endl;
    }
    else if (operation == "andi")
    {
        rregno_rd = ra & imme;
        cout << "\nExecute " << operation << " " << ra << " and " << imme << endl;
    }
    else if (operation == "ori")
    {
        rregno_rd = ra | imme;
        cout << "\nExecute " << operation << " " << ra << " and " << imme << endl;
    }
    else if (operation == "lb")
    {
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " EA -> add " << ra << " and " << imme << endl;
    }
    else if (operation == "ld")
    {
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " EA -> add " << ra << " and " << imme << endl;
    }
    else if (operation == "lh")
    {
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " EA -> add " << ra << " and " << imme << endl;
    }
    else if (operation == "lw")
    {
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " EA -> add " << ra << " and " << imme << endl;
    }
    else if (operation == "jalr")
    {
        rregno_rd = cpc;
        cpc = ra + imme;
        cout << "\nExecute " << operation << " pc-> add " << ra << " and " << imme << endl;
    }

    else if (operation == "sb")
    {
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " EA-> add " << ra << " and " << imme << endl;
    }
    else if (operation == "sd")
    {
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " EA-> add " << ra << " and " << imme << endl;
    }
    else if (operation == "sh")
    {
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " EA-> add " << ra << " and " << imme << endl;
    }
    else if (operation == "sw")
    {
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " EA-> add " << ra << " and " << imme << endl;
    }

    else if (operation == "beq")
    {
        int sb_de = -1;
        if (ra == rb)
        {
            rregno_rd = imme;
            sb_de = 1;
        }
        else
        {
            rregno_rd = 4;
            sb_de = 0;
        }
        cout << "\nExecute " << operation << " if 1->True if 0->False. The decision is: " << sb_de << endl;
    }
    else if (operation == "bne")
    {
        int sb_de = -1;
        if (ra != rb)
        {
            rregno_rd = imme;
            sb_de = 1;
        }
        else
        {
            rregno_rd = 4;
            sb_de = 0;
        }
        cout << "\nExecute " << operation << " if 1->True if 0->False. The decision is: " << sb_de << endl;
    }
    else if (operation == "bge")
    {
        int sb_de = -1;
        if (ra >= rb)
        {
            rregno_rd = imme;
            sb_de = 1;
        }
        else
        {
            rregno_rd = 4;
            sb_de = 0;
        }
        cout << "\nExecute " << operation << " if 1->True if 0->False. The decision is: " << sb_de << endl;
    }
    else if (operation == "blt")
    {
        int sb_de = -1;
        if (ra < rb)
        {
            rregno_rd = imme;
            sb_de = 1;
        }
        else
        {
            rregno_rd = 4;
            sb_de = 0;
        }
        cout << "\nExecute " << operation << " if 1->True if 0->False. The decision is: " << sb_de << endl;
    }
    else if (operation == "auipc")
    {
        rregno_rd = cpc + imme - 4;
        cout << "\nExecute " << operation << " ->add " << (cpc - 4) << " and " << imme << endl;
    }
    else if (operation == "lui")
    {
        rregno_rd = imme;
        cout << "\nExecute " << operation << " " << imme << endl;
    }
    else if (operation == "jal")
    {
        rregno_rd = imme;
        cout << "\nExecute " << operation << " no Execute operation " << endl;
    }
    else
        cout << "invalid input ";

    execute_ans.operation_type = operation;
    execute_ans.destination = regno_rd;
    execute_ans.rregno_rd = rregno_rd;
    return execute_ans;
} //rregno_rd is RZ
void loadhexword(string s, int la)
{
    int len = s.length();
    if (len == 1)
        s = "0000000" + s;
    if (len == 2)
        s = "000000" + s;
    if (len == 3)
        s = "00000" + s;
    if (len == 4)
        s = "0000" + s;
    if (len == 5)
        s = "000" + s;
    if (len == 6)
        s = "00" + s;
    if (len == 7)
        s = "0" + s;
    len = s.length();
    //cout << s << endl;
    int lah;
    lah = 268435456 + la;

    string hexx;
    stringstream hexc;
    hexc << hex << lah;
    hexc >> hexx;

        mem[la] = s.substr(len - 2, 2);

    cout << "In memory location 0x" << hexx << " strored 0x" << s.substr(len - 2, 2) << endl;

        mem[la + 1] = s.substr(len - 4, 2);

    hexc << hex << lah + 1;
    hexc >> hexx;
    cout << "In memory location 0x" << hexx << " strored 0x" << s.substr(len - 4, 2) << endl;
    
        mem[la + 2] = s.substr(len - 6, 2);

    hexc << hex << lah + 2;
    hexc >> hexx;
    cout << "In memory location 0x" << hexx << " strored 0x" << s.substr(len - 6, 2) << endl;
    
        mem[la + 3] = s.substr(len - 8, 2);

    hexc << hex << lah + 3;
    hexc >> hexx;
    cout << "In memory location 0x" << hexx << " strored 0x" << s.substr(len - 8, 2) << endl;
    //cout<<s<<endl;
}
void loadhexhalf(string s, int la)
{
    int lah;

    lah = 268435456 + la;

    string hexx;
    stringstream hexc;
    hexc << hex << lah;
    hexc >> hexx;
    int len = s.length();
    if (len == 1)
        s = "000" + s;
    if (len == 2)
        s = "00" + s;
    if (len == 3)
        s = "0" + s;

    len = s.length();

    mem[la] = s.substr(len - 2, 2);

    cout << "In memory location 0x" << hexx << " strored 0x" << s.substr(len - 2, 2) << endl;

    mem[la + 1] = s.substr(len - 4, 2);

    hexc << hex << lah + 1;
    hexc >> hexx;
    cout << "In memory location 0x" << hexx << " strored 0x" << s.substr(len - 4, 2) << endl;
}
memory_returns memory_access(execute_returns execute_ans)
{
    memory_returns memory_ans;
    string operation = execute_ans.operation_type;
    int regno_rd = execute_ans.destination;
    int rregno_rd = execute_ans.rregno_rd;

    int to_update_pc = -1;
    string mem_l = "";
    int lregno_rd = -1;

    if (operation == "add" || operation == "and" || operation == "ori" || operation == "andi" || operation == "addi" || operation == "rem" || operation == "div" || operation == "mul" || operation == "xor" || operation == "sub" || operation == "srl" || operation == "slt" || operation == "or" || operation == "sll" || operation == "sra")
    {
        cout << "Memory: No memory operation " << endl;
    }
    else if (operation == "lb")
    {
        rregno_rd -= 268435456;
        mem_l = "0x" + mem[rregno_rd];
        stringstream gg;
        gg << std::hex << mem_l;
        gg >> lregno_rd;
        cout << "Memory: " << operation << "in x" << regno_rd << " from memory address " << rregno_rd << endl;
    }
    // else if (operation == "ld")
    // {
    //     rregno_rd -= 268435456;
    //     mem_l = "0x" + mem[rregno_rd + 7] + mem[rregno_rd + 6] + mem[rregno_rd + 5] + mem[rregno_rd + 4] + mem[rregno_rd + 3] + mem[rregno_rd + 2] + mem[rregno_rd + 1] + mem[rregno_rd];
    //     stringstream gg;
    //     gg << std::hex << mem_l;
    //     gg >> lregno_rd;
    //     cout << "Memory: " << operation << "in x" << regno_rd << " from memory address " << rregno_rd << endl;
    // }
    else if (operation == "lh")
    {
        rregno_rd -= 268435456;
        mem_l = "0x" + mem[rregno_rd + 1] + mem[rregno_rd];
        stringstream gg;
        gg << std::hex << mem_l;
        gg >> lregno_rd;
        cout << "Memory: " << operation << " in x" << regno_rd << " from memory address " << rregno_rd << endl;
    }
    else if (operation == "lw")
    {
        rregno_rd -= 268435456;
        mem_l = "0x" + mem[rregno_rd + 3] + mem[rregno_rd + 2] + mem[rregno_rd + 1] + mem[rregno_rd];
        stringstream gg;
        gg << std::hex << mem_l;
        gg >> lregno_rd;
        cout << "Memory: " << operation << " in x" << regno_rd << " from memory address " << rregno_rd << endl;
    }
    else if (operation == "jalr")
    {
        to_update_pc = rregno_rd + 4;
        cout << "Memory: " << operation << " in mux Y : " << to_update_pc << endl;
    }

    else if (operation == "sb")
    {
        int lah;
        if (rregno_rd >= 268435456 && rregno_rd < 268441456)
        {
            rregno_rd -= 268435456;
            lah = rregno_rd;
        }
        else
        {
            cout << "Invalid Memory Access!" << endl;
            exit(0);
        }
        stringstream hexc;
        string lahh;

        hexc << hex << lah;
        hexc >> lahh;
        int num = r[regno_rd];
        ostringstream str1;
        str1 << hex << num;
        string ge = str1.str();
        if (ge.length() == 1)
            ge = "0" + ge;
        int len = ge.length();

        mem[rregno_rd] = ge.substr(len - 2, 2);

        cout << "In memory location 0x" << lahh << " strored 0x" << ge.substr(len - 2, 2) << endl;
    }
    // else if (operation == "sd")
    // {
    //     int num = r[regno_rd];
    //     ostringstream str1;
    //     str1 << hex << num;
    //     string ge = str1.str();

    //     mem[rregno_rd + 7] = ge.substr(0, 2);
    //     mem[rregno_rd + 6] = ge.substr(2, 2);
    //     mem[rregno_rd + 5] = ge.substr(4, 2);
    //     mem[rregno_rd + 4] = ge.substr(6, 2);
    //     mem[rregno_rd + 3] = ge.substr(8, 2);
    //     mem[rregno_rd + 2] = ge.substr(10, 2);
    //     mem[rregno_rd + 1] = ge.substr(12, 2);
    //     mem[rregno_rd] = ge.substr(14, 2);
    //     cout << "Memory: " << operation << "in EA " << endl;
    // }
    else if (operation == "sh")
    {
        int lah;

        if (rregno_rd >= 268435456 && rregno_rd < 268441456)
        {
            rregno_rd -= 268435456;
            lah = rregno_rd;
        }
        else
        {
            cout << "Invalid Memory Access!" << endl;
            exit(0);
        }
        int num = r[regno_rd];

        ostringstream str1;
        str1 << hex << num;

        string ge = str1.str();
        loadhexhalf(ge, rregno_rd);
        cout << "Memory: " << operation << "in EA " << endl;
    }
    else if (operation == "sw")
    {
        int lah;

        if (rregno_rd >= 268435456 && rregno_rd < 268441456)
        {
            rregno_rd -= 268435456;
            lah = rregno_rd;
        }
        else
        {
            cout << "Invalid Memory Access!" << endl;
            exit(0);
        }
        int num = r[regno_rd];
        ostringstream str1;
        str1 << hex << num;
        string ge = str1.str();
        // mem[rregno_rd + 3] = ge.substr(0, 2);
        // mem[rregno_rd + 2] = ge.substr(2, 2);
        // mem[rregno_rd + 1] = ge.substr(4, 2);
        // mem[rregno_rd] = ge.substr(6, 2);
        loadhexword(ge, rregno_rd);
        //cout << "Memory: " << operation << " in EA " << endl;
    }

    else if (operation == "beq")
    {
        cpc += rregno_rd;
        cout << "Memory: " << operation << " mux INC select and pc: " << cpc << endl;
    }
    else if (operation == "bne")
    {
        cpc += rregno_rd;
        cout << "Memory: " << operation << " mux INC select and pc: " << cpc << endl;
    }
    else if (operation == "bge")
    {
        cpc += rregno_rd;
        cout << "Memory: " << operation << " mux INC select and pc: " << cpc << endl;
    }
    else if (operation == "blt")
    {
        cpc += rregno_rd;
        cout << "Memory: " << operation << " mux INC select and pc: " << cpc << endl;
    }
    else if (operation == "auipc")
    {
        cout << "Memory: No memory operation " << endl;
    }
    else if (operation == "lui")
    {
        cout << "Memory: No memory operation " << endl;
    }
    else if (operation == "jal")
    {
        to_update_pc = cpc + 4;
        cpc += rregno_rd;
        cout << "Memory: " << operation << "in mux Y : " << to_update_pc << " and pc: " << cpc << endl;
    }
    else
        cout << "invalid input ";

    memory_ans.operation_type = operation;
    memory_ans.destination = regno_rd;
    memory_ans.rregno_rd = rregno_rd;
    memory_ans.to_update_pc = to_update_pc;
    memory_ans.lregno_rd = lregno_rd;
    return memory_ans;
}
int register_update(memory_returns memory_ans)
{

    string operation = memory_ans.operation_type;
    int regno_rd = memory_ans.destination;
    int rregno_rd = memory_ans.rregno_rd;
    int to_update_pc = memory_ans.to_update_pc;
    int lregno_rd = memory_ans.lregno_rd;
    if (regno_rd == 0)
    {
        cout << "Writeback: nothing to write as x0 can't be modified " << endl;
        return 0;
    }
    if (operation == "add")
    {
        r[regno_rd] = rregno_rd;
        cout << "Writeback: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "and")
    {
        r[regno_rd] = rregno_rd;
        cout << "Writeback: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "or")
    {
        r[regno_rd] = rregno_rd;
        cout << "Writeback: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "sll")
    {
        r[regno_rd] = rregno_rd;
        cout << "Writeback: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "slt")
    {
        r[regno_rd] = rregno_rd;
        cout << "Writeback: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "sra")
    {
        r[regno_rd] = rregno_rd;
        cout << "Writeback: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "srl")
    {
        r[regno_rd] = rregno_rd;
        cout << "Writeback: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "sub")
    {
        r[regno_rd] = rregno_rd;
        cout << "Writeback: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "xor")
    {
        r[regno_rd] = rregno_rd;
        cout << "Writeback: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "mul")
    {
        r[regno_rd] = rregno_rd;
        cout << "Writeback: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "div")
    {
        r[regno_rd] = rregno_rd;
        cout << "Writeback: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "rem")
    {
        r[regno_rd] = rregno_rd;
        cout << "Writeback: " << rregno_rd << " to x" << regno_rd << endl;
    }

    else if (operation == "addi")
    {
        r[regno_rd] = rregno_rd;
        cout << "Writeback: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "andi")
    {
        r[regno_rd] = rregno_rd;
        cout << "Writeback: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "ori")
    {
        r[regno_rd] = rregno_rd;
        cout << "Writeback: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "lb")
    {
        r[regno_rd] = lregno_rd;
        cout << "Writeback: " << lregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "ld")
    {
        r[regno_rd] = lregno_rd;
        cout << "Writeback: " << lregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "lh")
    {
        r[regno_rd] = lregno_rd;
        cout << "Writeback: " << lregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "lw")
    {
        r[regno_rd] = lregno_rd;
        cout << "Writeback: " << lregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "jalr")
    {
        r[regno_rd] = to_update_pc;
        cout << "Writeback: " << to_update_pc << " to x" << regno_rd << endl;
    }

    else if (operation == "sb")
    {
        cout << "Writeback: Nothing to write " << endl;
    }
    else if (operation == "sd")
    {
        cout << "Writeback: Nothing to write " << endl;
    }
    else if (operation == "sh")
    {
        cout << "Writeback: Nothing to write " << endl;
    }
    else if (operation == "sw")
    {
        cout << "Writeback: Nothing to write " << endl;
    }

    else if (operation == "beq")
    {
        cout << "Writeback: Nothing to write " << endl;
    }
    else if (operation == "bne")
    {
        cout << "Writeback: Nothing to write " << endl;
    }
    else if (operation == "bge")
    {
        cout << "Writeback: Nothing to write " << endl;
    }
    else if (operation == "blt")
    {
        cout << "Writeback: Nothing to write " << endl;
    }

    else if (operation == "auipc")
    {
        r[regno_rd] = rregno_rd;
        cout << "Writeback: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "lui")
    {
        r[regno_rd] = rregno_rd;
        cout << "Writeback: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "jal")
    {
        r[regno_rd] = to_update_pc;
        cout << "Writeback: " << to_update_pc << " to x" << regno_rd << endl;
    }
    else
        cout << "invalid input ";

    return 0;
}
int main()
{
    int clock=0;
    r[0] = 0;
    r[2] = 268440455;
    ifstream f;
    string l;
    decode_returns decode_ans;
    execute_returns execute_ans;
    memory_returns memory_ans;
    f.open("mcode.mc");
    while (!f.eof())
    {
        getline(f, l);
        if (l == "")
            continue;
        istringstream ss(l);
        string pcn;
        ss >> pcn;
        pcn = pcn.substr(2, pcn.length() - 2);
        stringstream hxc;
        hxc << hex << pcn;
        int ipc;
        hxc >> ipc;
        ss >> pcn;
        string inst = pcn.substr(2, pcn.length() - 2);
        if (ipc < 268435456)
        {
            instr.push_back(inst);
        }
        else
        {
            mem[ipc - 268435456] = inst;
        }
    }
    f.close();
    auto i = instr.begin();
    while (i < instr.end())
    {

        string mach_in_bin = fetch(i - instr.begin());
        decode_ans = decode(mach_in_bin);
        execute_ans = execute(decode_ans);
        memory_ans = memory_access(execute_ans);
        register_update(memory_ans);
        i = cpc / 4 + instr.begin();
        clock++;
        cout<<"Number of clock cycles= "<<clock<<"\n";
    }
    ofstream mach;
    mach.open("data_mem.mc");
    long long int adh = 268435456;
    // string st;
    // stringstream scc;
    // scc << hex << adh;
    // scc >> st;
    // mach << "0x" << st << " ";
     for (int i = 0; i < 6000; i++)
    {
        adh = 268435456 + i;

        string st;
        stringstream scc;
        scc << hex << adh;
        scc >> st;
        if(i%4==0)
            mach <<endl<< "0x" << st << " ";
        //  if (i % 4 == 3)
        // {
        //         mach<< endl;
               
        // }
        mach <<"0x" << mem[i] <<"\t";
    }
    mach.close();

    ofstream machr;
    machr.open("data_r.mc");
    for (int i = 0; i < 32; i++)
    {
        machr << "r" << i << "\t" << r[i] << endl;
    }
    machr.close();
    return 0;
}
