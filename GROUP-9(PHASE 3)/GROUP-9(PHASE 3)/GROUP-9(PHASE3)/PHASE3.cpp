#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstring>
#include <math.h>
using namespace std;
unsigned int r[32]={0};
string mem[6000];
string heap[800];
vector<string> instr;
int  n_inst=0;
int ndata=0;
int cpc = 0;
string pc;
unsigned int operand1, operand2;
map<int, pair<int, int>> bht;
bool bitp=false;
int nalu=0;
int ncon=0;
int rA_exe_to_dec=0;
int rB_exe_to_dec=0;
class fetch_returns{
public:
    string inst;
    int ppc;
    fetch_returns(){
        inst="";
        ppc=-1;
    }
};
class InterStageBuffer{
  public:
  //fetch
  string mach_F;
  int cur_pc_F;
  //decode
  string operation_type_D;
  int rA_D, rB_D;
  int destination_D;
  int immediate_value_D;
  int rs1,rs2;
  int nflush;
  int isjump;
  //execute
  string operation_type_E;
  int destination_E;
  int rregno_rd_E;          //rz
  int ppc_E;
  //memory
  string operation_type_M;
  int destination_M;
  int rregno_rd_M;
  int to_update_pc_M;
  int lregno_rd_M;
  //data forwarding
  int prev_rd,prev_p_rd;
  //flags for each step
  int is_F,is_D,is_E,is_M,is_W;   
  
  fetch_returns temp_fetch;   
  InterStageBuffer()
  {
      destination_D=-1;
        ppc_E=-1;
    isjump=0;
      rs1=-1;
      rs2=-1;
      prev_rd=-1;
      prev_p_rd=-1;
      is_F=0;
      is_D=0;
      is_E=0;
      is_M=0;
      is_W=0;
      nflush=1;
  }

};
class decode_returns
{
public:
    string operation_type;
    int rA, rB, rs1,rs2;
    int destination;
    int immediate_value;
    int pred;
    int ppc;
    int isjump;
    decode_returns()
    {
        pred=0;
        isjump=0;
        operation_type = "";
        rA = -1;
        rB = -1;
        rs1=-1;
        rs2=-1;
        destination = -1;
        immediate_value = -1;
        ppc=-1;
    }
};
class execute_returns
{
public:
    string operation_type;
    int destination;
    int rregno_rd;
    int ppc;
    execute_returns()
    {
        ppc=-1;
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
void bhtinsert(int sb_de, int imme)
{
    if (bht.find(cpc) == bht.end())
    {
        bht[cpc] = pair<int, int>(sb_de, cpc + imme);
    }
    else
    {
        bht[cpc] = pair<int, int>(sb_de, cpc + imme);
    }
}
decode_returns decode(fetch_returns ff)
{
    string mach_bin=ff.inst;
    int ppc=ff.ppc;
    string rd, rs1, rs2, imme;
    string opcode, funct3, funct7;
    int a, b, imm, ra, rb, regno_rd;
    //cout<<mach_bin<<endl;
    decode_returns ans; // return structure
    ans.ppc=ppc;
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

        ans.rs1=a;
        ans.rs2=b;

        ra = r[a];
        rb = r[b];

        cout << "\nDECODE:\nOperation is " << operation << "\nFirst operand: x" << a << "\nSecond operand: x" << b << "\nDestination register: x" << regno_rd;
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
        ans.rs1=a;
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
        cout << "\nDECODE:\nOperation is " << operation << "\nFirst operand: x" << a << "\nImmediate value:" << b << "\nDestination register: x" << regno_rd;
        cout << "\nRead register x" << a << " =" << ra;

        ans.operation_type = operation;
        ans.rA = ra;
        ans.destination = regno_rd;
        // ans.rB=b;
        ans.immediate_value = b;
        if(operation=="jalr"){
            // ncon++;
            if(!bitp){
                ans.pred=0;
               // cpc+=b-4;
            }
            else{
               // cout<<ra<<" "<<endl;
                if(bht.find(ppc)==bht.end())
                {    ans.pred=0;
                    bht[ppc]=pair<int,int>(1,ra+b);
                 //   cout<<bht[ppc].second<<" "<<ra<<" "<<b<<endl;
                }
                else{
                   // cout<<cpc<<" "<<ra<<" "<<b<<" "<<endl;
                    if(cpc==ra+b) ans.pred=1;
                    else{
                        ans.pred=0;
                        bht[ppc] = pair<int, int>(1, ra + b);
                        //cout<<"inserted0"<<endl;
                    }
                   
                }
                
            }
            ans.isjump=1;
            
        }
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
        ans.rs1=a;
        ans.rs2=b;
        char sign_bit = imme[0];
        if (sign_bit == '1')
        {
            imme = findTwoscomplement(imme);
            imm = bin_to_dec(imme);
            imm *= -1;
        }
        else
            imm = bin_to_dec(imme);
        cout << "\nDECODE:\nOperation is " << operation << "\n First operand x" << a << ", Offset " << imm << "\n Destination register x" << b;
        cout << "\nRead registers x" << a << " =" << ra;

        ans.operation_type = operation;
        ans.rA = ra;
       
        ans.destination = b;
        ans.immediate_value = imm;
    }
    else if (opcode == "1100011")
    { //SB be x2 x3 L
        string imme12, imme10_5, imme11, imme4_1;
        // ncon++;
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
        if(rA_exe_to_dec!=0){
            ra=rA_exe_to_dec;
            rA_exe_to_dec=0;
        }
        if(rB_exe_to_dec!=0){
            rb=rB_exe_to_dec;
            rB_exe_to_dec=0;
        }
        ans.rs1=a;
        ans.rs2=b;
        int imm;
        char sign_bit = imme[0];
        ans.isjump=1;
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
        cout << "\nDECODE:\nOperation is " << operation << "\nFirst operand: x" << a << "\nSecond operand: x" << b << "\nImmediate value: " << imm;
        ans.operation_type = operation;
        ans.rA = ra;
        ans.rB = rb;
        ans.rs1=a;
        ans.rs2=b;
        ans.immediate_value = imm;
        int sb_de = -1;
        if (operation == "beq")
        {
            // cout<<endl<<ra<< " "<<rb<<endl;
            if (ra == rb)
            {
                if(cpc==ppc+imm) ans.pred=1;
                else ans.pred=0;
                sb_de = 1;

            }
            else
            {
                //cout<<"k"<<endl;
                if (cpc == ppc + 4) ans.pred=1;
                else ans.pred=0;
                sb_de = 0;
            }
            //cout<<ppc<<" "<<cpc<<endl;
            if(bitp)
                cout << "\nBranch prediction in " << operation << "\nThe prediction was " << ans.pred<< endl;
            
        }
        else if (operation == "bne")
        {
           // cout<<"ra="<<ra<<" rb="<<rb<<" ppc="<<ppc<<" cpc="<<cpc<<endl;
            if (ra != rb)
            {
                if (cpc == ppc + imm)
                    ans.pred = 1;
                else
                    ans.pred = 0;
                sb_de = 1;
            }
            else
            {
                
                if (cpc == ppc + 4)
                    ans.pred = 1;
                else
                    ans.pred = 0;
                sb_de = 0;
            }
            if(bitp)
             cout << "\nBranch prediction in " << operation << "\nThe decision is"<<sb_de<<"and  prediction was " << ans.pred << endl;
        }
        else if (operation == "bge")
        {

            if (ra >= rb)
            {
                if (cpc == ppc + imm)
                    ans.pred = 1;
                else
                    ans.pred = 0;
                sb_de = 1;
            }
            else
            {
                if (cpc == ppc + 4)
                    ans.pred = 1;
                else
                    ans.pred = 0;
                sb_de = 0;
            }
            if(bitp)
            cout << "\nBranch prediction in " << operation << "\nThe prediction was " << ans.pred << endl;
        }
        else if (operation == "blt")
        {

            if (ra < rb)
            {
                if (cpc == ppc + imm)
                    ans.pred = 1;
                else
                    ans.pred = 0;
                sb_de = 1;
            }
            else
            {
                if (cpc == ppc + 4)
                    ans.pred = 1;
                else
                    ans.pred = 0;
                sb_de = 0;
            }
            if(bitp)
                cout << "\nBranch prediction in " << operation << "\nThe prediction was " << ans.pred << endl;
        }
        if(bitp){
            if(sb_de==1)
                 bht[ppc]=pair<int,int>(sb_de,ppc+imm);
            else
                bht[ppc]=pair<int,int>(sb_de,ppc+imm);


        }
        //  if(!ans.pred){
        //      cpc+=imm-4;
        //  }
       
    }
    else if (opcode == "1101111")
    { //UJ format jal x1 L
        // ncon++;
        rd = mach_bin.substr(20, 5);
        regno_rd = bin_to_dec(rd);

        string imme_20, imme_11, imme_10_to_1, imme_19_to_12;
        imme_20 = mach_bin[0];                  // imme[20]
        imme_10_to_1 = mach_bin.substr(1, 10);  // imme[10:1]
        imme_11 = mach_bin[11];                 //imme[11]
        imme_19_to_12 = mach_bin.substr(12, 8); //imme[19:12]
        imme = imme_20 + imme_19_to_12 + imme_11 + imme_10_to_1;
        char sign_bit = imme[0];
        ans.isjump=1;
        if (sign_bit == '1')
        {
            imme = findTwoscomplement(imme);
            imm = bin_to_dec(imme);
            imm *= -1;
            
            

              // ans.pred=1;
       // bhtinsert(1, imm);
        }
        else
            imm = bin_to_dec(imme);
        imm *= 2;
        cout << "\nDECODE:\nOperation is " << operation << "\n Jump: " << imm << "\n Return register x" << regno_rd << endl;
        ans.operation_type = operation;
        ans.immediate_value = imm;
        ans.destination = regno_rd;
        if (!bitp)
        {
            ans.pred = 0;
            // cpc+=b-4;
        }
        else
        {
            //cout << ppc << " " << cpc << " " << imm << " " << endl;
            if (bht.find(ppc) == bht.end())
            {
                ans.pred = 0;
                bht[ppc] = pair<int, int>(1, imm + ppc);
            }
            else
            {
                ans.pred = 1;
            }
        }
    }
    else if (opcode == "0110111" || opcode == "0010111")
    { // U format lui,auipc

        rd = mach_bin.substr(20, 5);
        regno_rd = bin_to_dec(rd);
        imme = mach_bin.substr(0, 20);
        imm = bin_to_dec(imme);
        imm *= pow(2, 12);
        cout << "\nDECODE:\nOperation is " << operation << "\n Upper immediate: " << imm << "\n Return register x" << regno_rd << endl;
        ans.operation_type = operation;
        ans.immediate_value = imm;
        ans.destination = regno_rd;
    }
    return ans;
}
fetch_returns fetch(int pc)
{
    fetch_returns ff;
    string inst, mach_bin, opcode;
    // istringstream s(l);
    // s>>pc;
    // s>>inst;
    //inst=inst.substr(2,8);
    inst = instr[pc];
    // pc=pc.substr(2,pc.length()-2);
    int niki;
    // if (inst == "00000C13")
    //     cin>>niki;
    mach_bin = HexToBin(inst);
    stringstream hexc;
    hexc << hex << pc * 4;
    string pch;
    hexc >> pch;
    ff.ppc=cpc;
    cout << "\nFETCH:\tFetch instruction 0x" << inst << " from address 0x" << pch<< "\t";
    opcode = mach_bin.substr(25, 7);

    // if (opcode == "1100011")
    //     ; //SB format
    // else if (opcode == "1101111")
    //     ; //jal UJ format
    // else if (opcode == "1100111")
    //     ; //jalr
    // else
    
    if(bitp){
        auto f = bht.find(cpc);
        if (f== bht.end())
                    cpc += 4; //next pc
        else{
            
            if(f->second.first==0) cpc+=4;
            else cpc=f->second.second;
            
            cout << "\nPC found in BHT predicted target address for next iteration-" <<cpc<<endl;
        }
    }
    else{
        cpc+=4;
    }
    

    ff.inst= mach_bin;
    return ff;
}
decode_returns decode_NonP(string mach_bin)
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
string fetch_NonP(int pc)
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
    int ppc=ans.ppc;
    execute_returns execute_ans;
    execute_ans.ppc=ppc;
    string operation = ans.operation_type;
    int ra = ans.rA;
    int rb = ans.rB;
    int regno_rd = ans.destination;
    int imme = ans.immediate_value;

    int rregno_rd = -1;
    if (operation == "add")
    {
        nalu++;
        rregno_rd = ra + rb;
        cout << "\nEXECUTE " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "and")
    {
        nalu++;
        rregno_rd = ra & rb;
        cout << "\nEXECUTE " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "or")
    {
        nalu++;
        rregno_rd = ra | rb;
        cout << "\nEXECUTE " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "sll")
    {
        nalu++;
        rregno_rd = ra << rb;
        cout << "\nEXECUTE " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "slt")
    {
        nalu++;
        rregno_rd = (ra < rb) ? 1 : 0;
        cout << "\nEXECUTE " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "sra")
    {nalu++;
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
        cout << "\nEXECUTE " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "srl")
    {nalu++;
        rregno_rd = ra >> rb;
        cout << "\nEXECUTE " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "sub")
    {nalu++;
        rregno_rd = ra - rb;
        cout << "\nEXECUTE " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "xor")
    {nalu++;
        rregno_rd = ra ^ rb;
        cout << "\nEXECUTE " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "mul")
    {nalu++;
        rregno_rd = ra * rb;
        cout << "\nEXECUTE " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "div")
    {nalu++;
        rregno_rd = ra / rb;
        cout << "\nEXECUTE " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "rem")
    {nalu++;
        rregno_rd = ra % rb;
        cout << "\nEXECUTE " << operation << " " << ra << " and " << rb << endl;
    }

    else if (operation == "addi")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nEXECUTE " << operation << " " << ra << " and " << imme << endl;
    }
    else if (operation == "andi")
    {nalu++;
        rregno_rd = ra & imme;
        cout << "\nEXECUTE " << operation << " " << ra << " and " << imme << endl;
    }
    else if (operation == "ori")
    {nalu++;
        rregno_rd = ra | imme;
        cout << "\nEXECUTE " << operation << " " << ra << " and " << imme << endl;
    }
    else if (operation == "lb")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nEXECUTE " << operation << " EA -> add " << ra << " and " << imme << endl;
    }
    else if (operation == "ld")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nEXECUTE " << operation << " EA -> add " << ra << " and " << imme << endl;
    }
    else if (operation == "lh")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nEXECUTE " << operation << " EA -> add " << ra << " and " << imme << endl;
    }
    else if (operation == "lw")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nEXECUTE " << operation << " EA -> add " << ra << " and " << imme << endl;
    }
    else if (operation == "jalr")
    {
        rregno_rd = cpc;
        
        nalu++;
        if (ans.pred == 0)
        {
            cout << "Flushing-- bubble in pipeline" << endl;
            cpc = ra + imme;
            //cpc = rregno_rd + ppc;
        }
        cout << "\nEXECUTE " << operation << " no operation " << endl;
    }

    else if (operation == "sb")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nEXECUTE " << operation << " EA-> add " << ra << " and " << imme << endl;
    }
    else if (operation == "sd")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nEXECUTE " << operation << " EA-> add " << ra << " and " << imme << endl;
    }
    else if (operation == "sh")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nEXECUTE " << operation << " EA-> add " << ra << " and " << imme << endl;
    }
    else if (operation == "sw")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nEXECUTE " << operation << " EA-> add " << ra << " and " << imme << endl;
    }

    else if (operation == "beq")
    {nalu++;
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
        if(ans.pred==0){
            cout<<"Flushing-- bubble in pipeline"<<endl;
            cpc=rregno_rd+ppc;
        }
        cout << "\nEXECUTE " << operation << " if 1->True if 0->False. The decision is: " << sb_de << endl;
    }
    else if (operation == "bne")
    {nalu++;
        int sb_de = -1;
        cout<<ra<<" "<<rb<<endl;
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
        if (ans.pred == 0)
        {
            cout << "Flushing-- bubble in pipeline" << endl;
            cpc = rregno_rd + ppc;
        }
        cout << "\nEXECUTE " << operation << " if 1->True if 0->False. The decision is: " << sb_de << endl;
    }
    else if (operation == "bge")
    {nalu++;
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
        if (ans.pred == 0)
        {
            cout << "Flushing-- bubble in pipeline" << endl;
            cpc = rregno_rd + ppc;
        }
        cout << "\nEXECUTE " << operation << " if 1->True if 0->False. The decision is: " << sb_de << endl;
    }
    else if (operation == "blt")
    {nalu++;
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
        if (ans.pred == 0)
        {
            cout << "Flushing-- bubble in pipeline" << endl;
            cpc = rregno_rd + ppc;
        }
        cout << "\nEXECUTE " << operation << " if 1->True if 0->False. The decision is: " << sb_de << endl;
    }
    else if (operation == "auipc")
    {nalu++;
        rregno_rd = ppc + imme ;
        cout << "\nEXECUTE " << operation << " ->add " << (ppc) << " and " << imme << endl;
    }
    else if (operation == "lui")
    {
        rregno_rd = imme;
        cout << "\nEXECUTE " << operation << " " << imme << endl;
    }
    else if (operation == "jal")
    {
        rregno_rd = imme;
        cout << "\nEXECUTE " << operation << " no Execute operation " << endl;
        if (ans.pred == 0)
        {
            cout << "Flushing-- bubble in pipeline" << endl;
            cpc = rregno_rd + ppc;
        }
    }
    else
        cout << "invalid input ";

    execute_ans.operation_type = operation;
    execute_ans.destination = regno_rd;
    execute_ans.rregno_rd = rregno_rd;
    return execute_ans;
} //rregno_rd is RZ
execute_returns execute_NonP(decode_returns ans)
{

    execute_returns execute_ans;
    string operation = ans.operation_type;
    int ra = ans.rA;
    int rb = ans.rB;
    int regno_rd = ans.destination;
    int imme = ans.immediate_value;

    int rregno_rd = -1;
    if (operation == "add")
    {nalu++;
        rregno_rd = ra + rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "and")
    {nalu++;
        rregno_rd = ra & rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "or")
    {nalu++;
        rregno_rd = ra | rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "sll")
    {nalu++;
        rregno_rd = ra << rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "slt")
    {nalu++;
        rregno_rd = (ra < rb) ? 1 : 0;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "sra")
    {nalu++;
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
    {nalu++;
        rregno_rd = ra >> rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "sub")
    {nalu++;
        rregno_rd = ra - rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "xor")
    {nalu++;
        rregno_rd = ra ^ rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "mul")
    {nalu++;
        rregno_rd = ra * rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "div")
    {nalu++;
        rregno_rd = ra / rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }
    else if (operation == "rem")
    {nalu++;
        rregno_rd = ra % rb;
        cout << "\nExecute " << operation << " " << ra << " and " << rb << endl;
    }

    else if (operation == "addi")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " " << ra << " and " << imme << endl;
    }
    else if (operation == "andi")
    {nalu++;
        rregno_rd = ra & imme;
        cout << "\nExecute " << operation << " " << ra << " and " << imme << endl;
    }
    else if (operation == "ori")
    {nalu++;
        rregno_rd = ra | imme;
        cout << "\nExecute " << operation << " " << ra << " and " << imme << endl;
    }
    else if (operation == "lb")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " EA -> add " << ra << " and " << imme << endl;
    }
    else if (operation == "ld")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " EA -> add " << ra << " and " << imme << endl;
    }
    else if (operation == "lh")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " EA -> add " << ra << " and " << imme << endl;
    }
    else if (operation == "lw")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " EA -> add " << ra << " and " << imme << endl;
    }
    else if (operation == "jalr")
    {nalu++;
        rregno_rd = cpc;
        cpc = ra + imme;
        cout << "\nExecute " << operation << " pc-> add " << ra << " and " << imme << endl;
    }

    else if (operation == "sb")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " EA-> add " << ra << " and " << imme << endl;
    }
    else if (operation == "sd")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " EA-> add " << ra << " and " << imme << endl;
    }
    else if (operation == "sh")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " EA-> add " << ra << " and " << imme << endl;
    }
    else if (operation == "sw")
    {nalu++;
        rregno_rd = ra + imme;
        cout << "\nExecute " << operation << " EA-> add " << ra << " and " << imme << endl;
    }

    else if (operation == "beq")
    {nalu++;
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
    {nalu++;
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
    {nalu++;
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
    {nalu++;
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
    {nalu++;
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
    int ppc=execute_ans.ppc;
    int regno_rd = execute_ans.destination;
    int rregno_rd = execute_ans.rregno_rd;

    int to_update_pc = -1;
    string mem_l = "";
    int lregno_rd = -1;

    if (operation == "add" || operation == "and" || operation == "ori" || operation == "andi" || operation == "addi" || operation == "rem" || operation == "div" || operation == "mul" || operation == "xor" || operation == "sub" || operation == "srl" || operation == "slt" || operation == "or" || operation == "sll" || operation == "sra")
    {
        cout << "MEMORY: No memory operation " << endl;
    }
    else if (operation == "lb")
    {
        rregno_rd -= 268435456;
        mem_l = "0x" + mem[rregno_rd];
        stringstream gg;
        gg << std::hex << mem_l;
        gg >> lregno_rd;
        cout << "MEMORY: " << operation << "in x" << regno_rd << " from memory address " << rregno_rd << endl;
    }
    else if (operation == "lh")
    {
        rregno_rd -= 268435456;
        mem_l = "0x" + mem[rregno_rd + 1] + mem[rregno_rd];
        stringstream gg;
        gg << std::hex << mem_l;
        gg >> lregno_rd;
        cout << "MEMORY: " << operation << " in x" << regno_rd << " from memory address " << rregno_rd << endl;
    }
    else if (operation == "lw")
    { //  cout<<rregno_rd<<"INHERE\n"<<mem[rregno_rd + 3]<<"\n" << mem[rregno_rd + 2] <<"\n"<< mem[rregno_rd + 1]<<"\n" <<mem[rregno_rd];
        rregno_rd -= 268435456;
        //  cout<<rregno_rd<<"INHERE\n"<<mem[rregno_rd + 3]<<"\n" << mem[rregno_rd + 2] <<"\n"<< mem[rregno_rd + 1]<<"\n" <<mem[rregno_rd];
        
         mem_l = "0x" + mem[rregno_rd + 3] + mem[rregno_rd + 2] + mem[rregno_rd + 1] + mem[rregno_rd];
        stringstream gg;
        gg << std::hex << mem_l;
        gg >> lregno_rd;
        cout << "MEMORY: " << operation << " in x" << regno_rd << " from memory address " << rregno_rd << endl;
    }
    else if (operation == "jalr")
    {   ncon++;
        to_update_pc = rregno_rd + 4;
        cout << "MEMORY: " << operation << " in mux Y : " << to_update_pc << endl;
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
        cout << "MEMORY: " << operation << "in EA " << endl;
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
        loadhexword(ge, rregno_rd);
        //cout << "Memory: " << operation << " in EA " << endl;
    }

    else if (operation == "beq")
    {   ncon++;
        //cpc += rregno_rd;
        cout << "MEMORY:\n" << operation << " : No memory operation  " << endl;
    }
    else if (operation == "bne")
    {   ncon++;
        //cpc += rregno_rd;
        cout << "MEMORY:\n" << operation << " : No memory operation  " << endl;
    }
    else if (operation == "bge")
    {   ncon++;
       // cpc += rregno_rd;
       cout << "MEMORY: \n" << operation << " : No memory operation  " << endl;
    }
    else if (operation == "blt")
    {   ncon++;
        //cpc += rregno_rd;
        cout << "MEMORY: \n" << operation << " : No memory operation  " << endl;
    }
    else if (operation == "auipc")
    {
        cout << "MEMORY: No memory operation " << endl;
    }
    else if (operation == "lui")
    {
        cout << "MEMORY: No memory operation " << endl;
    }
    else if (operation == "jal")
    {   ncon++;
        to_update_pc = ppc + 4;
        //cpc += rregno_rd;
        cout << "MEMORY: \n" << operation << " : No memory operation  " << endl;
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
memory_returns memory_access_NonP(execute_returns execute_ans)
{   
    memory_returns memory_ans;
    string operation = execute_ans.operation_type;
    if(operation=="lw" ||operation=="lb" ||operation=="lh" ||operation=="sw" ||operation=="sb" ||operation=="sh" )ndata++;
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
                n_inst++;

    string operation = memory_ans.operation_type;
    int regno_rd = memory_ans.destination;
    int rregno_rd = memory_ans.rregno_rd;
    int to_update_pc = memory_ans.to_update_pc;
    int lregno_rd = memory_ans.lregno_rd;
    if (regno_rd == 0)
    {
        cout << "WRITEBACK: nothing to write as x0 can't be modified " << endl;
        return 0;
    }
    if (operation == "add")
    {
        r[regno_rd] = rregno_rd;
        cout << "WRITEBACK: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "and")
    {
        r[regno_rd] = rregno_rd;
        cout << "WRITEBACK: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "or")
    {
        r[regno_rd] = rregno_rd;
        cout << "WRITEBACK: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "sll")
    {
        r[regno_rd] = rregno_rd;
        cout << "WRITEBACK: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "slt")
    {
        r[regno_rd] = rregno_rd;
        cout << "WRITEBACK: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "sra")
    {
        r[regno_rd] = rregno_rd;
        cout << "WRITEBACK: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "srl")
    {
        r[regno_rd] = rregno_rd;
        cout << "WRITEBACK: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "sub")
    {
        r[regno_rd] = rregno_rd;
        cout << "WRITEBACK: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "xor")
    {
        r[regno_rd] = rregno_rd;
        cout << "WRITEBACK: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "mul")
    {
        r[regno_rd] = rregno_rd;
        cout << "WRITEBACK: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "div")
    {
        r[regno_rd] = rregno_rd;
        cout << "WRITEBACK: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "rem")
    {
        r[regno_rd] = rregno_rd;
        cout << "WRITEBACK: " << rregno_rd << " to x" << regno_rd << endl;
    }

    else if (operation == "addi")
    {
        r[regno_rd] = rregno_rd;
        cout << "WRITEBACK: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "andi")
    {
        r[regno_rd] = rregno_rd;
        cout << "WRITEBACK: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "ori")
    {
        r[regno_rd] = rregno_rd;
        cout << "WRITEBACK: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "lb")
    {
        r[regno_rd] = lregno_rd;
        cout << "WRITEBACK: " << lregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "ld")
    {
        r[regno_rd] = lregno_rd;
        cout << "WRITEBACK: " << lregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "lh")
    {
        r[regno_rd] = lregno_rd;
        cout << "WRITEBACK: " << lregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "lw")
    {
        r[regno_rd] = lregno_rd;
        cout << "WRITEBACK: " << lregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "jalr")
    {
        r[regno_rd] = to_update_pc;
        cout << "WRITEBACK: " << to_update_pc << " to x" << regno_rd << endl;
    }

    else if (operation == "sb")
    {
        cout << "WRITEBACK: Nothing to write " << endl;
    }
    else if (operation == "sd")
    {
        cout << "WRITEBACK: Nothing to write " << endl;
    }
    else if (operation == "sh")
    {
        cout << "WRITEBACK: Nothing to write " << endl;
    }
    else if (operation == "sw")
    {
        cout << "WRITEBACK: Nothing to write " << endl;
    }

    else if (operation == "beq")
    {
        cout << "WRITEBACK: Nothing to write " << endl;
    }
    else if (operation == "bne")
    {
        cout << "WRITEBACK: Nothing to write " << endl;
    }
    else if (operation == "bge")
    {
        cout << "WRITEBACK: Nothing to write " << endl;
    }
    else if (operation == "blt")
    {
        cout << "WRITEBACK: Nothing to write " << endl;
    }

    else if (operation == "auipc")
    {
        r[regno_rd] = rregno_rd;
        cout << "WRITEBACK: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "lui")
    {
        r[regno_rd] = rregno_rd;
        cout << "WRITEBACK: " << rregno_rd << " to x" << regno_rd << endl;
    }
    else if (operation == "jal")
    {
        r[regno_rd] = to_update_pc;
        cout << "WRITEBACK: " << to_update_pc << " to x" << regno_rd << endl;
    }
    else
        cout << "invalid input ";

    return 0;
}
void execute_to_execute_df(InterStageBuffer *isb,decode_returns* decode_ans){

  if(isb->rs1==isb->prev_rd){
        if(decode_ans->rs1==0)return;
    decode_ans->rA=isb->rregno_rd_E;
    isb->rA_D=decode_ans->rA;
  }
   if(isb->rs2==isb->prev_rd){
         if(decode_ans->rs2==0)return;
    decode_ans->rB=isb->rregno_rd_E;
     isb->rB_D=decode_ans->rB;
  }
}
void memory_to_execute_df(InterStageBuffer *isb,decode_returns *decode_ans){

 if(isb->prev_p_rd==isb->rs1){
       if(decode_ans->rs1==0)return;
   decode_ans->rA=isb->rregno_rd_M;
   isb->rA_D=decode_ans->rA;
 }
 if(isb->prev_p_rd==isb->rs2)   {
       if(decode_ans->rs2==0)return;
     decode_ans->rB=isb->rregno_rd_M;
     isb->rB_D=decode_ans->rB;
 }
}
void update_decode(decode_returns decode_ans,InterStageBuffer *isb){
  
  isb->operation_type_D=decode_ans.operation_type;
  isb->rA_D=            decode_ans.rA;
  isb->rB_D=              decode_ans.rB;
  isb->immediate_value_D=decode_ans.immediate_value;
  isb->destination_D=      decode_ans.destination;           
  isb->rs1=              decode_ans.rs1;
  isb->rs2=              decode_ans.rs2;
  //cout<<decode_ans.pred<<" dp"<<endl;
  isb->nflush=decode_ans.pred;
  isb->isjump=decode_ans.isjump;
 //    cout<<"\n nFlush:"<<isb->nflush<<endl;

}
void update_execute(execute_returns execute_ans,InterStageBuffer *isb){
  
  isb->operation_type_E=   execute_ans.operation_type;
  isb->destination_E   =  execute_ans.destination;
  isb->rregno_rd_E     =execute_ans.rregno_rd;
  isb->ppc_E=execute_ans.ppc;
}
void update_memory_access(memory_returns memory_ans,InterStageBuffer* isb){
  
  isb->operation_type_M=   memory_ans.operation_type;
  isb->destination_M   =   memory_ans.destination;
  isb->rregno_rd_M     =   memory_ans.rregno_rd;
  isb->to_update_pc_M  =   memory_ans.to_update_pc;
  isb->lregno_rd_M     =   memory_ans.lregno_rd;
}
void show_pipeline_register(InterStageBuffer isb)
{
	cout<<"\n---------------------PIPELINE REGISTERS---------------------\n";
	cout<<"\t\t\tRA: "<<isb.rA_D<<"\n";
	cout<<"\t\t\tRB: "<<isb.rB_D<<"\n";
	cout<<"\t\t\tRZ: "<<isb.rregno_rd_E<<"\n";
	cout<<"\t\t\tRM: "<<isb.rregno_rd_M<<"\n";
	cout<<"\t\t\tRY: "<<isb.lregno_rd_M<<"\n";
}
void show_register()
{
	cout<<"\nREGISTER FILE\n";
	for(int i=0;i<32;i++)
	cout<<"r["<<i<<"] = "<<r[i]<<"\n";
}
int main()
{
    bool Pipe=false;
    bool data_forward=false;
    bool stall=false;
    char show_reg='n',show_pipe_reg='n';
    int cycle_pipe_reg_print=-1;
    int clock=0;
    r[0] = 0;
    r[2] = 268440455;
    ifstream f;
    string l;
    InterStageBuffer isb;
    fetch_returns fetch_ans;
    string fetch_ans_NonP;
    decode_returns decode_ans;
    execute_returns execute_ans;
    memory_returns memory_ans;
    f.open("mcode.mc");
    char p,d,reg,pipe;
    cout<<"Do you want to enable pipeline ? (y/n):\n";
    cin>>p;
    if(p=='y')
    {
    	Pipe=true;
    	cout<<"Do you want to enable data forwarding ? (y/n):\n";
    	cin>>d;
    	if(d=='y')data_forward=true;	
    	cout<<"Do you want to print information in PIPELINE REGISTERS at the end of each cycle ?(y/n):\n";
    	cin>>show_pipe_reg;
        cout<<"Do you want to enable bit prediction?"<<endl;
        char bp;
        cin>>bp;
        if(bp=='y') bitp=true;
	}
	cout<<"Do you want to print REGISTER FILE at the end of each cycle ?(y/n):\n";
    cin>>show_reg;
    if(show_pipe_reg=='n' && Pipe){
        cout<<"Do you want to print information in PIPELINE REGISTERS at the end of a particular cycle ?(y/n):\n";
        char knob5;
        cin>>knob5;
        if(knob5=='y'){
            cout<<"Enter the instruction number: \t";
            cin>>cycle_pipe_reg_print;
        }
    }
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
    int nbubble = 0;
    int nbubble_total=0;
    int ndata_haz=0;
    int temp_haz=0;
    string s="";
    if(Pipe==false){
      auto i = instr.begin();
      while (i < instr.end())
      {

          fetch_ans_NonP = fetch_NonP(i - instr.begin());
          decode_ans = decode_NonP(fetch_ans_NonP);
          execute_ans = execute_NonP(decode_ans);
          memory_ans = memory_access_NonP(execute_ans);
          register_update(memory_ans);
          i = cpc / 4 + instr.begin();
          clock++;
          if(show_reg=='y')
          show_register();
          cout<<"\n\nNumber of clock cycles= "<<clock<<"\n\n";
      }
    }
    else if(Pipe==true)
    {   
        int count_stall=0;
        clock=0;
        int itr=0;
        int special=0;
        bool B_DF_L=false;
        bool b_df=false;
        while(1)
        {
        //    auto ibht=bht.begin();
        //     while(ibht!=bht.end()){
        //         cout<<ibht->first<<" "<<ibht->second.first<< " "<<ibht->second.second<<endl;
        //         ibht++;
        //     }
            
          isb.prev_p_rd=isb.prev_rd;
            isb.prev_rd=isb.destination_D;
            
     
            if(itr<instr.size())
            {   

            if(!stall){
                 isb.is_F=1;
                }   
            }
            
            if(isb.is_F==0 && isb.is_D==0  && isb.is_E==0  && isb.is_M==0  && isb.is_W==0)
			{
            	 break;
            }
            if(isb.is_W)
            {  
                register_update(memory_ans);
                isb.is_W=0;
            }  
            if(isb.is_M)
            {   
                memory_ans=memory_access(execute_ans);
                update_memory_access(memory_ans,&isb);

                if(isb.operation_type_D=="lb" || isb.operation_type_D=="lw" ||                                              
                   isb.operation_type_D=="lh" || isb.operation_type_D=="sb" ||                                         
                   isb.operation_type_D=="sw" || isb.operation_type_D=="sh"  )
                   ndata++;
                   isb.is_M=0;
               	   isb.is_W=1;
            }
            if(isb.is_E)
            {   

                 execute_ans=execute(decode_ans);
                 update_execute(execute_ans,&isb);
                 if(isb.nflush==0 && isb.isjump==1)
				 {
                    itr=cpc/4;
                    if (itr < instr.size())
                    {

                        if (!stall)
                        {
                            isb.is_F = 1;
                        }
                    }
                    else{
                        isb.is_F=0;
                    }
                    isb.is_D=0;
                    nbubble++;
                    isb.nflush=1;
                    
                }
                isb.is_E=0;
                isb.is_M=1;
                
            }
            if (isb.is_D) {
                nbubble_total++;
                if (!stall || (data_forward)) {
                    if (b_df == true) {
                        if (execute_ans.destination == decode_ans.rs1) {
                            rA_exe_to_dec = execute_ans.rregno_rd;
                        }
                        if (execute_ans.destination == decode_ans.rs2) {
                            rB_exe_to_dec = execute_ans.rregno_rd;
                        }
                        b_df = false;
                    }
                    decode_ans = decode(fetch_ans);
                } else {
                    decode_ans = decode(isb.temp_fetch);
                    cout << "\n(STALLED)\n";
                }
                update_decode(decode_ans, & isb);
                //data forwarding                               
                if (data_forward ) {
                    if(isb.operation_type_D == "sb" || isb.operation_type_D == "sw" || isb.operation_type_D == "sh"){isb.rs2=-1;}
                    if (isb.operation_type_E == "lb" || isb.operation_type_E == "lw" ||
                        isb.operation_type_E == "lh") {
                        if (special == 2) special--;
                        if (special == 0) {
                            if (((isb.rs1 == isb.prev_rd || isb.rs2 == isb.prev_rd) && isb.prev_rd != -1) &&
                                ((isb.rs1 == isb.prev_p_rd || isb.rs2 == isb.prev_p_rd) && isb.prev_p_rd != -1)) {
                               
                               if(isb.prev_p_rd==isb.prev_rd)ndata_haz++;
                                else ndata_haz+=2;

                                if (isb.rs1 == isb.prev_rd || isb.rs2 == isb.prev_rd) {
                                    special = 1;
                                    if (isb.operation_type_D == "beq" || isb.operation_type_D == "bne" ||
                                        isb.operation_type_D == "blt" || isb.operation_type_D == "bge") {
                                        special = 2;
                                        B_DF_L = true;
                                        temp_haz++;
                                    }
                                }

                                if (isb.prev_p_rd == isb.prev_rd) {
                                    ndata_haz++;
                                    execute_to_execute_df( & isb, & decode_ans);
                                } else {
                                    ndata_haz++;
                                    execute_to_execute_df( & isb, & decode_ans);
                                    memory_to_execute_df( & isb, & decode_ans);

                                }
                            } else if (isb.rs1 == isb.prev_rd || isb.rs2 == isb.prev_rd && isb.prev_rd != -1) {
                                ndata_haz++;
                                if (isb.rs1 == isb.prev_rd || isb.rs2 == isb.prev_rd) {
                                    special = 1;
                                    if (isb.operation_type_D == "beq" || isb.operation_type_D == "bne" ||
                                        isb.operation_type_D == "blt" || isb.operation_type_D == "bge") {
                                        special = 2;
                                        B_DF_L = true;
                                        temp_haz++;
                                    }
                                }
                                execute_to_execute_df( & isb, & decode_ans);
                               // cout << "EXECUTE TO EXECUTE" << decode_ans.rB << endl;
                            } else if (isb.rs1 == isb.prev_p_rd || isb.rs2 == isb.prev_p_rd && isb.prev_p_rd != -1) {
                                ndata_haz++;
                                memory_to_execute_df( & isb, & decode_ans);
                            }

                        } else if (special == 1 && B_DF_L == false) {

                            if (memory_ans.destination == decode_ans.rs1) {
                                decode_ans.rA = memory_ans.lregno_rd;
                                isb.rA_D = decode_ans.rA;
                            }
                            if (memory_ans.destination == decode_ans.rs2) {
                                decode_ans.rB = memory_ans.lregno_rd;
                                isb.rB_D = decode_ans.rB;
                            }
                            special = 0;

                        } else if (special == 1 & B_DF_L) B_DF_L = false;
                        if (special > 0) {
                            isb.is_D = 1;
                            isb.is_E = 0;
                        } else {
                            isb.is_D = 0;
                            isb.is_E = 1;

                        }

                    } else {
                        if (((isb.rs1 == isb.prev_rd || isb.rs2 == isb.prev_rd) && isb.prev_rd != -1 && isb.prev_rd != 0) &&
                            ((isb.rs1 == isb.prev_p_rd || isb.rs2 == isb.prev_p_rd) && isb.prev_p_rd != -1  && isb.prev_p_rd != 0)) {
                            
                                
                            if (isb.rs1 == isb.prev_rd || isb.rs2 == isb.prev_rd) {


                                if (isb.operation_type_D == "beq" || isb.operation_type_D == "bne" ||
                                    isb.operation_type_D == "blt" || isb.operation_type_D == "bge")
                                    {b_df = true;temp_haz++;}
                            }
                            if (isb.prev_p_rd == isb.prev_rd) {
                                if (isb.operation_type_E == "sb" || isb.operation_type_E == "sw" ||
                                    isb.operation_type_E == "sh");
                                else{
                                    execute_to_execute_df( & isb, & decode_ans);
                                    ndata_haz++;
                                    }
                            } else {
                                if (isb.operation_type_E == "sb" || isb.operation_type_E == "sw" ||
                                    isb.operation_type_E == "sh");
                                else {

  
                                     ndata_haz+=2;
                                     execute_to_execute_df( & isb, & decode_ans);
                                    memory_to_execute_df( & isb, & decode_ans);
                                }
                            }
                        } else if (( isb.rs1 == isb.prev_rd || isb.rs2 == isb.prev_rd) && isb.prev_rd !=-1 && isb.prev_rd != 0) {
                            if (isb.rs1 == isb.prev_rd || isb.rs2 == isb.prev_rd) {
                                if (isb.operation_type_D == "beq" || isb.operation_type_D == "bne" ||
                                    isb.operation_type_D == "blt" || isb.operation_type_D == "bge")
                                    {b_df = true;temp_haz++;}
                            }

                            if (isb.operation_type_E == "sb" || isb.operation_type_E == "sw" ||
                                isb.operation_type_E == "sh");
                            else{
                                execute_to_execute_df( & isb, & decode_ans);
                                ndata_haz++;
                               }
                        } 
                        else if (isb.rs1 == isb.prev_p_rd || isb.rs2 == isb.prev_p_rd && isb.prev_p_rd != -1 && isb.prev_p_rd != 0) {
                           
                            if (isb.rs1 == isb.prev_p_rd || isb.rs2 == isb.prev_p_rd) {
                                if (isb.operation_type_D == "beq" || isb.operation_type_D == "bne" ||
                                    isb.operation_type_D == "blt" || isb.operation_type_D == "bge")
                                    { b_df = true;
                                   temp_haz++;}
                            }
                            if(isb.prev_p_rd!=-1)
                             ndata_haz++;
                             memory_to_execute_df( & isb, & decode_ans);
                            
                        }
                        // else if ((isb.rs1 == isb.prev_p_rd || isb.rs2 == isb.prev_p_rd) && isb.prev_p_rd != -1 && isb.prev_p_rd != 0 ) {
                        //     if (isb.rs1 == isb.prev_p_rd || isb.rs2 == isb.prev_p_rd) {
                        //         if (isb.operation_type_D == "beq" || isb.operation_type_D == "bne" ||
                        //             isb.operation_type_D == "blt" || isb.operation_type_D == "bge")
                        //            { b_df = true;temp_haz++;}
                        //     }
                        //      ndata_haz++;
                        //       memory_to_execute_df( & isb, & decode_ans);
                        // }
                        if (b_df == false) {
                            isb.is_D = 0;
                            isb.is_E = 1;
                        }

                    }

                }

                //stalling 
                else if(isb.operation_type_E!="sw" || isb.operation_type_E!="sb"|| isb.operation_type_E!="sh"){
                    if ((isb.rs1 == isb.prev_rd || isb.rs2 == isb.prev_rd) && (!stall) && clock > 1 && isb.prev_rd != -1 && isb.prev_rd != 0 &&
                        isb.operation_type_D != "jal") {
                        stall = true;
                        count_stall = 2;
                        isb.temp_fetch = fetch_ans;

                        ndata_haz++;
                    } 
                    else if ((isb.rs1 == isb.prev_p_rd || isb.rs2 == isb.prev_p_rd) && (!stall) && clock > 2 && isb.prev_p_rd != -1 && isb.prev_p_rd != 0 &&
                        isb.operation_type_D != "jal") {
                        stall = true;
                        count_stall = 1;
                        isb.temp_fetch = fetch_ans;

                        ndata_haz++;
                    }
                    if (count_stall > 0)
                        count_stall--;
                    else if (stall)
                        stall = false;

                    if (!stall) {
                        isb.is_D = 0;
                        isb.is_E = 1;
                    }
                }
            }
            if(isb.is_F && isb.is_D==0)
            {  
               fetch_ans= fetch(itr);
               isb.mach_F=fetch_ans.inst;
               isb.cur_pc_F=fetch_ans.ppc;
               isb.is_F=0;
               isb.is_D=1;
               itr=cpc/4;
            }
            else if(isb.is_F && isb.is_D )
            {
                isb.is_F=1;
                isb.is_D=1;
     		}
            if(show_reg=='y')
            show_register();
            if(show_pipe_reg=='y')
            show_pipeline_register(isb);
            clock++;
            if(cycle_pipe_reg_print!=-1 && cycle_pipe_reg_print==clock){
                show_pipeline_register(isb);
            } 
            cout<<"\n\n------------------------------------------------------------------------CYCLE NUMBER:\t "<<clock<<"\n\n";
        }
        
    }
    int mspred=nbubble;
    if(!bitp) mspred=0;
    ofstream mach;
    mach.open("data_mem.mc");
    long long int adh = 268435456;
     for (int i = 0; i < 6000; i++)
    {
        adh = 268435456 + i;

        string st;
        stringstream scc;
        scc << hex << adh;
        scc >> st;
        if(i%4==0)
            mach <<endl<< "0x" << st << " ";
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

    ofstream stats;
    stats.open("Stats.txt");
//for both Pipelined and non Pipelined
    stats<<"Stat1:  Total number of cycles = "<<clock<<endl;            //correct
    stats<<"Stat2:  Total instructions executed = "<<n_inst<<endl;//correct
    stats<<"Stat3:  CPI = "<<((double)clock/(double)n_inst)<<endl;//correct
    stats<<"Stat4:  Number of Data-transfer (load and store) instructions executed = "<<ndata<<endl;//correct
    stats<<"Stat5:  Number of ALU instructions executed = "<<nalu<<endl;//correct

// Only Pipelined
if(Pipe==true){
    stats<<"Stat6:  Number of Control instructions executed = "<<ncon<<endl;//correct
    stats<<"Stat7:  Number of stalls/bubbles in the pipeline = "<<nbubble_total-n_inst+nbubble<<endl;//correct
    stats<<"Stat8:  Number of data hazards = "<<ndata_haz-temp_haz/2<<endl;//correct
    stats<<"Stat9:  Number of control hazards = "<<nbubble<<endl;//correct
    stats<<"Stat10: Number of branch mispredictions = "<<mspred<<endl;//correct
    stats<<"Stat11: Number of stalls due to data hazards = "<<nbubble_total-n_inst<<endl;//coorect..
    stats<<"Stat12: Number of stalls due to control hazards = "<<nbubble<<endl;//correct
}
    return 0;
} 