
#include <bits/stdc++.h>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cstring>
#include <regex>
#include <math.h>
#include <sstream>
using namespace std;
int pc = 0;
int la = 0;
string mem[4000];
int tpc;
map<string, char> tab;
map<string, int> label;
int ship=0,offset_of_first_load_inst;
int stringbin_to_dec(string s)
{
    int n = 0;
    int p = 1;
    for (int i = s.length() - 1; i >= 0; i--)
    {
        if (s[i] == '1')
        {
            n += p;
        }
        p *= 2;
    }
    return n;
}
int stringdec_to_dec(string s)
{
    int n = 0;
    int p = 1;
    if (s[0] != '-')
    {
        for (int i = 0; i < s.length(); i++)
        {
            n = n * 10 + s[i] - '0';
        }
        return n;
    }
    else
    {
        for (int i = 1; i < s.length(); i++)
        {
            n = n * 10 + s[i] - '0';
        }
        return -n;
    }
}
void loadhexword(string s)
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
    mem[la] = s.substr(len - 2, 2);
    mem[la + 1] = s.substr(len - 4, 2);
    mem[la + 2] = s.substr(len - 6, 2);
    mem[la + 3] = s.substr(len - 8, 2);
}
void loadhexhalf(string s)
{
    int len = s.length();
    if (len == 1)
        s = "000" + s;
    if (len == 2)
        s = "00" + s;
    if (len == 3)
        s = "0" + s;

    len = s.length();
    mem[la] = s.substr(len - 2, 2);
    mem[la + 1] = s.substr(len - 4, 2);
}
void exit1()
{
    cout << "code should not run";
    exit(0);
}
void exit2()
{
    cout << "infinite loop";
    exit(0);
}
string convert_to_length_5(string s)
{

    int l = 5 - s.length();
    string conct = "";
    while (l--)
    {
        conct = conct + "0";
    }
    return conct + s;
}
string convert_to_length_20(string s)
{

    int l = 20 - s.length();
    string conct = "";
    while (l--)
    {
        conct = conct + "0";
    }
    return conct + s;
}
string convert_to_length_12(string s)
{
    int l = 12 - s.length();
    string conct = "";
    while (l--)
    {
        conct = conct + "0";
    }
    return conct + s;
}
string dec_to_bin(int n)
{
    string ans;
    while (n > 0)
    {
        char t = 48 + n % 2;
        n /= 2;
        ans += t;
    }
    reverse(ans.begin(), ans.end());
    return ans;
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
string bin_to_hex(string bin)
{

    string hex = "";
    int i;
    int p;

    for (i = 0; i < 32; i += 4)
    {
        p = bin.length() - i;

        string temp;
        if (p < 4)
        {
            temp = bin.substr(0, p);
        }
        else
        {
            temp = bin.substr(p - 4, 4);
        }
        int b = bin_to_dec(temp);

        if (b <= 9)
        {
            char x = 48 + b;

            hex = x + hex;
        }
        else
        {
            char y = 55 + b; //65-10
            hex = y + hex;
        }
        temp = "";
        if (p <= 4)
        {
            return hex;
        }
    }
    return hex;
}
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
string r_type(string l)
{
    //add, and, or, sll, slt, sra, srl, sub, xor, mul, div, rem
    // 0000000 00010 00001  000   00011  0110011
    // func7   rs2   rs1   func3   rd     opcode
    map<string, string> hashmap;
    ifstream opcode;
    opcode.open("opcode.txt");
    string type, func_opcode;
    while (!opcode.eof())
    {
        opcode >> type >> func_opcode;
        hashmap.insert(pair<string, string>(type, func_opcode));
    }
    //hashmap ready
    stringstream instruction(l);
    string word;
    instruction >> word;
    string func_and_opcode;
    func_and_opcode = hashmap.find(word)->second; // for add, 0000000 000 0110011(func7func3opcode)
    int rs1, rs2, rd;
    char j, i;
    instruction >> word;
    i = word[1];
    i = (int)i - 48;
    rd = i;
    j = (int)word[2] - 48;
    if (j < 10 && j >= 0)
        rd = 10 * i + j;

    instruction >> word;
    i = word[1];
    i = (int)i - 48;
    rs1 = i;
    j = (int)word[2] - 48;
    if (j < 10 && j >= 0)
        rs1 = 10 * i + j;

    instruction >> word;
    i = word[1];
    i = (int)i - 48;
    rs2 = i;
    j = (int)word[2] - 48;
    if (j < 10 && j >= 0)
        rs2 = 10 * i + j;

    if(rs1<0 || rs1>32 || rs2<0 || rs2>32 || rd<0 || rd>32){
        cout<<"Invalid register numbers given"<<endl;
        exit(0);
    }
    if (instruction >> word)
    {
        cout << "error :"<< " : got more than three arguments." << endl;
        exit(0);
    }
    string machine_code, r1, r2, rout, func3, func7, op;

    r1 = convert_to_length_5(dec_to_bin(rs1));
    r2 = convert_to_length_5(dec_to_bin(rs2));
    rout = convert_to_length_5(dec_to_bin(rd));

    func7 = func_and_opcode.substr(0, 7); //func 7
    func3 = func_and_opcode.substr(7, 3);
    op = func_and_opcode.substr(10, 7);

    machine_code = func7 + r2 + r1 + func3 + rout + op;
    machine_code = bin_to_hex(machine_code);
    machine_code = "0x" + machine_code;

    string PC = dec_to_bin(pc);

    PC = bin_to_hex(PC);

    machine_code = "0x" + PC + "     " + machine_code;
    pc += 4;
    return machine_code;
}
string u_type(string l)
{
    // lui x4,0x12AB7
    //x4 = value<<12   0x12AB7237
    //   imm[31:12]                    rd      opcode
    //  00000000000000010010           10101   0110111
    map<string, string> hashmap;
    ifstream opcode;
    opcode.open("opcode.txt");
    string type, func_opcode;
    while (!opcode.eof())
    {
        opcode >> type >> func_opcode;
        hashmap.insert(pair<string, string>(type, func_opcode));
    }
    //hashmap ready
    stringstream instruction(l);
    string word;
    instruction >> word;
    string func_and_opcode;
    func_and_opcode = hashmap.find(word)->second; //  0110111(opcode)
    int rd, imm;
    char j, i;
    instruction >> word;
    i = word[1];
    i = (int)i - 48;
    rd = i;
    j = (int)word[2] - 48;
    if (j < 10 && j >= 0)
        rd = 10 * i + j;
       
    
    string dup_imm, x;
    stringstream DUPLICATE_instruction(l);
    DUPLICATE_instruction >> x;
    DUPLICATE_instruction >> x;
    DUPLICATE_instruction >> dup_imm;
    int dup_imm_start = (int)dup_imm[0];
    
    if (dup_imm.substr(0, 2) == "0b")
    { // positive binary
        dup_imm = dup_imm.substr(2, (dup_imm.length()) - 2);
        imm = bin_to_dec(dup_imm);
        if (imm > 1048575){
            cout <<  "Immediate value ( " << imm << " ) out of range " << endl;
            exit(0);
        }
    }
    else if (dup_imm.substr(0, 2) == "0x")
    { // positive hexadecimal
        dup_imm = dup_imm.substr(2, (dup_imm.length()) - 2);

        stringstream ss;
        ss << hex << dup_imm;
        ss >> imm;
        if (imm > 1048575){
            cout <<  "Immediate value ( " << imm << " ) out of range " << endl;
            exit(0);
        }
    }
    else if (dup_imm_start <= 57 && dup_imm_start >= 48)
    {

        instruction >> imm;
        if (imm> 1048575)
        {
            cout << "Immediate value ( " << imm << " ) out of range " << endl;
            exit(0);
        }
        
    }
    if(rd<0 || rd>32){
        cout<<"Invalid register numbers given"<<endl;
        exit(0);
    } 
    if (instruction >> word)
    {
        cout << "error :"<< " : got three arguments(expexted 2)." << endl;
        exit(0);
    } 

    string machine_code, rout, imme, imme_20, imme_11, imme_10_to_1, imme_19_to_12, op;

    imme = convert_to_length_20(dec_to_bin(imm));
    rout = convert_to_length_5(dec_to_bin(rd));

    op = func_and_opcode;

    machine_code = imme + rout + op;
    machine_code = bin_to_hex(machine_code);
    machine_code = "0x" + machine_code;

    string PC = bin_to_hex(dec_to_bin(pc));

    machine_code = "0x" + PC + "     " + machine_code;
    pc += 4;
    return machine_code;
}
string i_type(string l)
{
    ifstream c;
    map<string, string> code;
    string::iterator j, k;
    int rd = 0, rs = 0, rem, imme = 0, flag = 0;
    string ans, a, fin, f, op;
    c.open("opcode.txt");
    string s1, s2, im;
    while (!c.eof())
    {
        c >> s1 >> s2;
        code.insert(pair<string, string>(s1, s2));
    }
    stringstream s(l);
    string i, temp;
    s >> i;
    string word = i;
    string new1, new2;
    temp = code.find(i)->second; //funct3+opcode
    s >> i;                    //rd
    rd = 0;
    for (j = i.begin() + 1; j != i.end(); j++)
    {
        rd = 10 * rd + ((int)(*j) - 48);
        //rem++;
    }
    rem = 0;

    if (word == "andi" || word == "addi" || word == "ori" || word == "jalr") // andi,addi,ori,jalr
    {
              s >> i; //rs
              rs=0;
              for (j = i.begin() + 1; j != i.end(); j++)
              {
                  rs = 10 * rs + ((*j) - 48);
                  //rem++;
              }
              s >> i; //immed
              if(i[0]=='x'){
                  cout<<"The third argument has to be an immediate value,not a register"<<endl;
                  exit(0);
              }
              string ii;

             if (i.substr(0, 2) == "0b")
              { // positive binary
                      ii = i.substr(2, (i.length()) - 2);
                      imme = bin_to_dec(ii);

                       if (imme > 2047)
                     {
                        cout << "Immediate " << imme << " is not in range" << endl;
                        exit(0);
                     }
                  //imme=ii2;
              }
             else if (i.substr(0, 3) == "-0b")
             { // negative binary
                      ii = i.substr(3, (i.length()) - 3);
                      imme = bin_to_dec(ii);
                      if (imme > 2047)
                     {
                        cout << "Immediate " << imme << " is not in range" << endl;
                        exit(0);
                     }

                     imme*=-1;
             }
             else if (i.substr(0, 2) == "0x")
             { // positive hexadecimal
                     ii = i.substr(2, (i.length()) - 2);

                     stringstream ss;
                     ss << hex << ii;
                     ss >> imme;
                     if (imme > 2047)
                     {
                         cout << "Immediate " << imme << " is not in range" << endl;
                         exit(0);
                     }
             }
          else if (i.substr(0, 3) == "-0x")
           { // negative hexadecimal
              ii = i.substr(3, (i.length()) - 3);

                  stringstream ss;
                  ss << hex << ii;
                  ss >> imme;
                  if (imme > 2047)
                  {
                      cout << "Immediate " << imme << " is not in range" << endl;
                      exit(0);
                  }
                      imme*=-1;

           }
          else{
              stringstream st(i);      //addi x1,x1,-9??
              st>>imme;

          }
      
      
    }
     else if (word == "lb" ||  word == "lw" || word == "lh") //lb,lw,lh
     {
        s >> i;
            j = i.begin();
            if(i[0]=='x'||i[0]=='('){
                cout<<" no offset/immediate field given"<<endl;
                exit(0);
            }
            else if (*j >= 48 && *j <= 57)
            {
                imme=0;
                for (; (*j) != '('; j++)
                {
                    imme = (10 * imme) + ((*j) - 48); //offset
                   // rem++;
                }
                
                rs = 0;
                for (k = j + 2; (*k) != ')'; k++) //lb x1 100(x2)
                {
                    rs = 10 * rs + ((*k) - 48);
                   // rem++;
                }
            }
             else //lw x1 var1
              {   
                  flag = 1;
                  string var,rd_;
                  stringstream ss11(l);
                  ss11>>var;//lw
                  ss11>>rd_;//x1
                  ss11>>var;//var1
                   map<string, int>::iterator itr2; 

                 // cout<<"i am here "<<tpc;
                  new1 = "auipc "+rd_ + " 65536";
                  new2 = u_type(new1);     //auipc x1 65536
                   map<string, int>::iterator itr;

                  for (itr2 = label.begin(); itr2 != label.end(); ++itr2) { 

                  string lbl;
                  stringstream ss(itr2->first);
                  ss>>lbl;
                  if(itr2->first==var){
                      tpc=itr2->second;       
                  }

                  }
                  //offset_of_first_load_inst;
                  if(ship==0){
                  imme = tpc-pc+4; 
                  ship=1;
                  offset_of_first_load_inst=imme;
                  }
                  else{
                      imme=offset_of_first_load_inst-pc; 
                  }
                   stringstream ss1; 
                   ss1<<hex<<imme;
                  string hexv;
                  ss1>>hexv;
                  string im=HexToBin(hexv);
                  reverse(im.begin(),im.end());
                  im=im.substr(0,12);
                  reverse(im.begin(),im.end());
                  //cout<<endl<<endl<<tpc<<endl<<endl;
                  // // new2=word+" x"+rd+" "+imme+"(x"+rd+")";      //lw x1 -tpc(x1)  offset is -tpc
                   new2 = new2 + "\r\n"; //result from auipc
                   rs = rd;

              }
      }
    if(imme <-2047 && imme >=2048)
    {
       cout<<"Error : immediate ="<<imme<<" is out of range\n";
       exit(0);
    } 
    if (s >> i)
    {
        cout << "error :"<< " : got more than three arguments." << endl;
        exit(0);
    }
    if(rs<0 || rs>32 || rd<0 || rd>32){
        cout<<"Invalid register numbers given"<<endl;
        exit(0);
    }  
    if(imme<0){
       
       stringstream ss; 
       ss<<hex<<imme;
       string hexv;
       ss>>hexv;
       a=HexToBin(hexv);
       reverse(a.begin(),a.end());
       a=a.substr(0,12);
       reverse(a.begin(),a.end());
       ans+=a;
    }
    else{ 
    a = dec_to_bin(imme);
    a = convert_to_length_12(a);
    ans += a;
    }
    a = dec_to_bin(rs);
    a = convert_to_length_5(a);
    ans += a;
    f = temp.substr(0, 3);
    ans += f;
    a = dec_to_bin(rd);
    a = convert_to_length_5(a);
    ans += a;
    op = temp.substr(3, 7);
    ans += op;
    fin = bin_to_hex(ans);
    fin = "0x" + fin;

    string PC = bin_to_hex(dec_to_bin(pc));

    fin = "0x" + PC + "     " + fin;
    pc += 4;
    if (flag == 0)
        return fin;
    else
    {
        new2 = new2 + fin;
        return new2;
    }
          
}
string s_type(string l)
{
    ifstream c;
    map<string, string> code;
    string::iterator j, k;
    int rs1 = 0, rs2 = 0, rem = 0, imme = 0;
    c.open("opcode.txt");
    string s1, s2, im;
    while (!c.eof())
    {
        c >> s1 >> s2;
        code.insert(pair<string, string>(s1, s2));
    }
    stringstream s(l);
    string i, temp;
    s >> i;
    temp = code.find(i)->second; //funct3+opcode
    s >> i;                      //rs2
    rs2=0;
    for (j = i.begin() + 1; j != i.end(); j++)
    {
        rs2 = 10 * rs2 + ((*j) - 48);
       // rem++;
    }
    imme = 0;
    s >> i; //immed
     if(i[0]=='x'||i[0]=='('){
                cout<<" no offset/immediate field given"<<endl;
                exit(0);
            }
    for (j = i.begin(); (*j) != '('; j++)
    {
        imme = (10 * imme) + ((*j) - 48);
        //rem++;
    }
    rs1 = 0;
    for (k = j + 2; (*k) != ')'; k++)
    {
        rs1 = (10 * rs1) + ((*k) - 48);
       // rem++;
    }
    if(imme <-2047 && imme >=2048)
    {
       cout<<"Error : immediate ="<<imme<<" is out of range\n";
       exit(0);
    }  
    if (s >> i)
    {
        cout << "error :"<< " : got more than three arguments." << endl;
        exit(0);
    }
    if(rs1<0 || rs1>32 || rs2<0 || rs2>32){
        cout<<"Invalid register numbers given"<<endl;
        exit(0);
    }  
    string ans, a, fin, im1, im2, f, op;
    a = dec_to_bin(imme);
    a = convert_to_length_12(a);
    im1 = a.substr(7, 5);
    im2 = a.substr(0, 7);
    ans += im2;
    a = dec_to_bin(rs2);
    a = convert_to_length_5(a);
    ans += a;
    a = dec_to_bin(rs1);
    a = convert_to_length_5(a);
    ans += a;
    f = temp.substr(0, 3);
    ans += f;

    ans += im1;
    op = temp.substr(3, 7);
    ans += op;
    fin = bin_to_hex(ans);
    fin = "0x" + fin;

    string PC = bin_to_hex(dec_to_bin(pc));

    fin = "0x" + PC + "     " + fin;
    pc += 4;
    return fin;
}
string uj_type(string l)
{
    //jal x2 13
    //13(odd)=>12=>stored 6  if imm is odd, it is stored as imm/2 and then mutiplied by 2 .So, effectively, imm-1
    // imm[20]  imm[10:1]   imm [11]   imm [19:12]         rd      opcode
    //    0     0000000110     0        00000000          00010    1101111
    map<string, string> hashmap;
    ifstream opcode;
    opcode.open("opcode.txt");
    string type, func_opcode;
    while (!opcode.eof())
    {
        opcode >> type >> func_opcode;
        hashmap.insert(pair<string, string>(type, func_opcode));
    }
    //hashmap ready
    stringstream instruction(l);

    string word;
    instruction >> word;
    string func_and_opcode;
    func_and_opcode = hashmap.find(word)->second; //  0110011(opcode)
    int rd, imm = 0;
    char j, i;
    instruction >> word;
    i = word[1];
    i = (int)i - 48;
    rd = i;
    j = (int)word[2] - 48;
    if (j < 10 && j >= 0)
        rd = 10 * i + j;

    if(rd<0 || rd>32){
        cout<<"Invalid register number given"<<endl;
        exit(0);
    }

    string dup_imm, x;
    stringstream DUPLICATE_instruction(l);
    DUPLICATE_instruction >> x;
    DUPLICATE_instruction >> x;
    DUPLICATE_instruction >> dup_imm;
    int dup_imm_start = (int)dup_imm[0];
    
    if (dup_imm.substr(0, 2) == "0b")
    { // positive binary
        dup_imm = dup_imm.substr(2, (dup_imm.length()) - 2);
        imm = bin_to_dec(dup_imm);
        if (imm > 2047){
            cout <<  "offset ( " << imm << " ) out of range " << endl;
            exit(0);
        }
        if (imm == 0){
            cout << "Infinite loop (offset should be > 0)" << endl;
            exit(0);
        }
    }
    else if (dup_imm.substr(0, 3) == "-0b")
    { // negative binary
        dup_imm = dup_imm.substr(3, (dup_imm.length()) - 3);
        imm = bin_to_dec(dup_imm);
        if (imm > 2047){
            cout <<  "offset ( " << imm << " ) out of range " << endl;
            exit(0);
        }
        if (imm == 0){
            cout << "Infinite loop (offset should be > 0)" << endl;
            exit(0);
        }
        imm*=-1;
    }
    else if (dup_imm.substr(0, 2) == "0x")
    { // positive hexadecimal
        dup_imm = dup_imm.substr(2, (dup_imm.length()) - 2);

        stringstream ss;
        ss << hex << dup_imm;
        ss >> imm;
        if (imm > 2047){
            cout <<  "offset ( " << imm << " ) out of range " << endl;
            exit(0);
        }
        if (imm == 0){
            cout << "Infinite loop (offset should be > 0)" << endl;
            exit(0);
        }
    }
    else if (dup_imm.substr(0, 3) == "-0x")
    { // negative hexadecimal
        dup_imm = dup_imm.substr(3, (dup_imm.length()) - 3);

        stringstream ss;
        ss << hex << dup_imm;
        ss >> imm;
        if (imm > 2047){
            cout <<  "offset ( " << imm << " ) out of range " << endl;
            exit(0);
        }
        if (imm == 0){
            cout << "Infinite loop (offset should be > 0)" << endl;
            exit(0);
        }
        imm*=-1;
    }
    else if (dup_imm_start <= 57 && dup_imm_start >= 48)
    {

        instruction >> imm;
        if (abs(imm)> 2047)
        {
            cout << "offset ( " << imm << " ) out of range " << endl;
            exit(0);
        }
        if (imm == 0)
        {
            cout << "Infinite loop (offset should be > 0)" << endl;
            exit(0);
        }
    }
    else{int yatch=0;
    string lbl;
        map<string, int>::iterator itr; 
        for (itr = label.begin(); itr != label.end(); ++itr) { 
        
            
            stringstream ss(itr->first);
            ss>>lbl;
            if(lbl==dup_imm){
                yatch=1;
                imm=itr->second;
                imm=imm-pc;
            }
            
        }
        if(yatch==0){
            cout<<"This label "<<lbl<<"does not exist"<<endl;
            exit(0);
        }
    }
    
    imm /= 2;

    string machine_code, rout, imme, imme_20, imme_11, imme_10_to_1, imme_19_to_12, op;
    
    if(imm<0){
    //   cout<<"----------negative----------";
       stringstream ss; 
       ss<<hex<<imm;
       string hexv;
       ss>>hexv;
       imme=HexToBin(hexv);
       reverse(imme.begin(),imme.end());
       imme=imme.substr(0,20);
       reverse(imme.begin(),imme.end());
      // cout<<imme<<endl;
    }    

    else{imme = convert_to_length_20(dec_to_bin(imm));}
    rout = convert_to_length_5(dec_to_bin(rd));

    op = func_and_opcode;
    imme_20 = imme[0];
    imme_10_to_1 = imme.substr(10, 10);
    imme_11 = imme[9];
    imme_19_to_12 = imme.substr(1, 8);
    imme = imme_20 + imme_10_to_1 + imme_11 + imme_19_to_12;

    machine_code = imme + rout + op;
    machine_code = bin_to_hex(machine_code);
    machine_code = "0x" + machine_code;
    string PC = bin_to_hex(dec_to_bin(pc));

    machine_code = "0x" + PC + "     " + machine_code;

    pc += 4;
    return machine_code;
}
string sb_type(string l)
{

    ifstream c;
    map<string, string> code;
    string::iterator j, k;
    int rs22 = 0, rs12 = 0, rem = 0, imme2 = 0;

    c.open("opcode.txt");
    string s1, s2, im;
    while (!c.eof())
    {
        c >> s1 >> s2;
        code.insert(pair<string, string>(s1, s2));
    }
    stringstream s(l);

    string i, temp;
    s >> i;
    temp = code.find(i)->second; //funct3+opcode

    s >> i; //rs12
    string PC11 = bin_to_hex(dec_to_bin(pc));
    if (i.substr(0, 1) != "x")
    {
        cout << "error in 0x" << PC11 << " :register " << i << " not recognised" << endl;
        exit(0);
    }
    string i_check = i.substr(1, i.length() - 1);
    regex re111("[^0-9]");
    smatch m111;
    if (regex_search(i_check, m111, re111) == true)
    { // not decimal
        cout << "error in 0x" << PC11 << " :register " << i << " not recognised" << endl;
        exit(0);
    }
    for (j = i.begin() + 1; j != i.end(); j++)
    {
        rs12 = 10 *rs12  + ((*j) - 48);
        rem++;
    }
    rem = 0;
    //cout<<rs12<<endl;
    if (rs12 > 31 || rs12 < 0)
    {
        cout << "error in 0x" << PC11 << " :register " << i << " not recognised" << endl;
        exit(0);
    }

    s >> i; //rs22
    if (i.substr(0, 1) != "x")
    {
        cout << "error in 0x" << PC11 << " :register " << i << " not recognised" << endl;
        exit(0);
    }
    i_check = i.substr(1, i.length() - 1);
    regex re1112("[^0-9]");
    smatch m1112;
    if (regex_search(i_check, m1112, re1112) == true)
    { // not decimal
        cout << "error in 0x" << PC11 << " :register " << i << " not recognised" << endl;
        exit(0);
    }
    for (j = i.begin() + 1; j != i.end(); j++)
    {
        rs22 = 10 * rs22 + ((*j) - 48);
        rem++;
    }
    rem = 0;
    if (rs22 > 31 || rs22 < 0)
    {
        cout << "error in 0x" << PC11 << " :register " << i << " not recognised" << endl;
        exit(0);
    }

    s >> i; //immed
    string ii;
    int ii2 = 0;

    if (i.length()>=2&&i.substr(0, 2) == "0b")
    { // positive binary
        ii = i.substr(2, (i.length()) - 2);
        ii2 = bin_to_dec(ii);
        if (ii2 > 2047)
        {
            cout << "error in 0x" << PC11 << " :offset " << i << " is not in range # too far" << endl;
            exit(0);
        }
        if (ii2 == 0)
        {
            cout << "error in 0x" << PC11 << " :infinite loop" << endl;
            exit(0);
        }
    }
    if (i.length() >= 3&&i.substr(0, 3) == "-0b")
    { // negative binary
        ii = i.substr(3, (i.length()) - 3);
        ii2 = bin_to_dec(ii);
        if (ii2 > 2047)
        {
            cout << "error in 0x" << PC11 << " :offset " << i << " is not in range # too far" << endl;
            exit(0);
        }
        if (ii2 == 0)
        {
            cout << "error in 0x" << PC11 << " :infinite loop" << endl;
            exit(0);
        }
        ii2 = 0 - ii2;
    }
    if (i.length() >= 2&&i.substr(0, 2) == "0x")
    { // positive hexadecimal
        ii = i.substr(2, (i.length()) - 2);

        stringstream ss;
        ss << hex << ii;
        ss >> ii2;
        if (ii2 > 2047)
        {
            cout << "error in 0x" << PC11 << " :offset " << i << " is not in range # too far" << endl;
            exit(0);
        }
        if (ii2 == 0)
        {
            cout << "error in 0x" << PC11 << " :infinite loop" << endl;
            exit(0);
        }
    }
    if (i.length() >= 3&&i.substr(0, 3) == "-0x")
    { // negative hexadecimal
        ii = i.substr(3, (i.length()) - 3);

        stringstream ss;
        ss << hex << ii;
        ss >> ii2;
        if (ii2 > 2047)
        {
            cout << "error in 0x" << PC11 << " :offset " << i << " is not in range # too far" << endl;
            exit(0);
        }
        if (ii2 == 0)
        {
            cout << "error in 0x" << PC11 << " :infinite loop" << endl;
            exit(0);
        }
        ii2 = 0 - ii2;
    }
    else
    {
        regex re1("[^0-9]");
        smatch m1;
        if (regex_search(i, m1, re1) == false)
        { // positive decimal
            stringstream ss2(i);
            ss2 >> ii2;
            if (ii2 > 2047)
            {
                cout << "error in 0x" << PC11 << " :offset " << i << " is not in range # too far" << endl;
                exit(0);
            }
            if (ii2 == 0)
            {
                cout << "error in 0x" << PC11 << " :infinite loop" << endl;
                exit(0);
            }
        }
        else
        {
            string ineg, ineg2;
            ineg = i.substr(0, 1);

            if (ineg == "-")
            {
                ineg2 = i.substr(1, (i.length()) - 1);
                regex re1("[^0-9]");
                smatch m1;
                if (regex_search(ineg2, m1, re1) == false)
                { // negative decimal
                    stringstream ss2(ineg2);
                    ss2 >> ii2;
                    if (ii2 > 2047)
                    {
                        cout << "error in 0x" << PC11 << " :offset " << i << " is not in range # too far" << endl;
                        exit(0);
                    }
                    if (ii2 == 0)
                    {
                        cout << "error in 0x" << PC11 << " :infinite loop" << endl;
                        exit(0);
                    }
                    ii2 = 0 - ii2;
                }
            }
            else
            {            
                                   //label
                int pc_of_l1, current_pc = pc; //assuming pc_of_l1 is the pc of instruction following l1
                if (label.find(i) == label.end())
                {
                    cout << "label : " << i << " not found";
                    exit(0);
                }
                pc_of_l1 = label.find(i)->second;
                
                ii2 = pc_of_l1 - current_pc;
                if (ii2 < -2048 || ii2 > 2047)
                {
                    cout << "error in 0x" << PC11 << " label : " << i << " is not in range # too far" <<endl;
                    exit(0);
                }
                if (ii2 == 0)
                {
                    cout << "error in 0x" << PC11 << " :infinite loop" << endl;
                    exit(0);
                }
                
            }
        }
    }
    if (s >> i)
    {
        cout << "error in 0x" << PC11 << " : got more than three arguments." << endl;
        exit(0);
    }
    string c2;
    imme2 = ii2;
    if ((imme2 % 2) == 0)
        ;
    else
    {
        imme2 = imme2 - 1;
    }
    imme2 = imme2 >> 1;

    if (imme2 > 0)
    {
        c2 = dec_to_bin(imme2);
        c2 = convert_to_length_12(c2);
        
    }
    else
    {
        stringstream ss3;
        ss3 << hex << imme2;
        string res = ss3.str();
        string res2;
        res2 = res.substr(5, 3);
       
        c2 = HexToBin(res2);
    }
    // cout<<endl<<c2<<endl;
    string im1, im2, im3, im4;
    // im1=c2.substr(11,1);
    // im2=c2.substr(4,6);
    // im3=c2.substr(0,4);
    // im4=c2.substr(10,1);
    im1 = c2[0];
    im2 = c2.substr(2, 6);
    im3 = c2.substr(8, 4);
    im4 = c2[1];
    string a2, b2;
    a2 = dec_to_bin(rs12);
    a2 = convert_to_length_5(a2); // a2 has rs12 in string form
   // cout<<a2<<endl;
    b2 = dec_to_bin(rs22);
    b2 = convert_to_length_5(b2); // b2 has rs22 in string form
   // cout<<temp<<endl;
    string f2, op2;
    f2 = temp.substr(0, 3);  // f2 has func3 in sting form
    op2 = temp.substr(3, 7); // op2 has opcode in sting form
   // cout<<f2<<endl;
    string ans2;

    ans2 = im1 + im2 + b2 + a2 + f2 + im3 + im4 + op2; // ans2 should be 32 bit binary code

    string fin2;
    fin2 = bin_to_hex(ans2);
    fin2 = "0x" + fin2;
    
    string PC2 = bin_to_hex(dec_to_bin(pc));
    fin2 = "0x" + PC2 + "     " + fin2;
    pc += 4;
    return fin2;
}
string runinst(string l)
{
    istringstream ss(l);
    string inst;
    ss >> inst;
    //cout<<inst<<" ";
    auto itr = tab.find(inst);
    if (itr == tab.end())
    {
        cout << "Invalid Instruction!" << endl;
        exit(0);
    }
    string cd;
    if (itr != tab.end())
    {
        char type = itr->second;
        //cout<<type<<endl;

        if (type == 'R')
            cd = r_type(l);

        if (type == 'I')
        {
            cd = i_type(l);
            //cout<<"here"<<endl;
        }

        if (type == 'S')
            cd = s_type(l);

        if (type == 'B')
            cd = sb_type(l);

        if (type == 'U')
            cd = u_type(l);

        if (type == 'J')
            cd = uj_type(l);
    }
    return cd;
}
int main()
{

    ifstream data;
    data.open("data.txt");
    string line;
    char tp;
    while (!data.eof())
    {

        data >> tp >> line;
        if (tp == '$')
            break;
        tab.insert(pair<string, char>(line, tp));
        // cout<<tp<<line<<endl;
    }
    ifstream asmbl;

    ofstream mach;
    string l;
    asmbl.open("asmbl.asm");
    mach.open("mcode.mc");
    regex re("(.*[^:]):(.*)");
    smatch match;
    int t = 1;
    //making the label list
    while (!asmbl.eof())
    {

        getline(asmbl, l);
        //cout<<l<<"ip";
        if (l == ".data")
        {
            t = 0;
            continue;
        }
        else if (l == ".text")
        {
            t = 1;
            continue;
        }
        else if (regex_search(l, match, re) == true)
        {
            string k = match.str(1);
            istringstream spn(k);
            spn>>k;
            istringstream ss1(match.str(2));
            string wrd;
            ss1 >> wrd;

            if (wrd == ".word")
            {
                if (label.find(k) == label.end())
                {
                    label.insert(pair<string, int>(k, la));
                    //cout<<"instr"<<k<<la<<endl;
                }
                else
                {
                    cout << k << " label declared twice! Error" << endl;
                    exit(0);
                }
                while (ss1 >> wrd)
                {
                    la += 4;
                    //cout<<la<<" "<<endl;
                }
            }
            else if (wrd == ".byte")
            {
                if (label.find(k) == label.end())
                {
                    label.insert(pair<string, int>(k, la));
                }
                else
                {
                    cout << k << " label declared twice! Error" << endl;
                    exit(0);
                }
                while (ss1 >> wrd)
                {
                    la += 1;
                }
            }
            else if (wrd == ".half")
            {
                if (label.find(k) == label.end())
                {
                    label.insert(pair<string, int>(k, la));
                }
                else
                {
                    cout << k << " label declared twice! Error" << endl;
                    exit(0);
                }
                while (ss1 >> wrd)
                {
                    la += 2;
                }
            }
            else if (wrd == ".dword")
            {
                if (label.find(k) == label.end())
                {
                    label.insert(pair<string, int>(k, la));
                }
                else
                {
                    cout << k << " label declared twice! Error" << endl;
                    exit(0);
                }
                while (ss1 >> wrd)
                {
                    la += 8;
                }
            }
            else if (wrd == ".asciiz")
            {
                if (label.find(k) == label.end())
                {
                    label.insert(pair<string, int>(k, la));
                }
                else
                {
                    cout << k << " label declared twice! Error" << endl;
                    exit(0);
                }
                ss1 >> wrd;
                int l = wrd.length() - 1;
                la += l;
            }
            else

            {
                //cout << l << "isnt" << endl;
                if (label.find(k) == label.end())
                {
                    label.insert(pair<string, int>(k, pc));
                    //cout << k << "lbl" << endl;
                }
                else
                {
                    cout << k << " label declared twice! Error" << endl;
                    exit(0);
                }
            }
            // for (auto itr = label.begin(); itr != label.end(); itr++)
            // {
            //     cout << itr->first << " " << itr->second << endl;
            // }
            //cout << l << endl;
        }
        else
        {
            //cout<<t<<" ";
            istringstream stt(l);
            string lll;
            stt >> lll;
            if (lll != "\0" && t == 1)
            {
                //cout << l << " ";
                if (lll != "lw" && lll != "lb" && lll != "ld" )
                    pc+=4;
                else{
                    stt>>lll;
                    if(lll!="\0"){
                        stt>>lll;
                        if (lll != "\0"){
                        
                            regex re1("[(]");
                            smatch m1;
                            if (regex_search(lll, m1, re1) == true){
                                pc+=4;
                            }
                            else pc+=8;
                           
                        }
                        else
                        {
                            cout << "Invalid instruction at pc=" << pc << endl;
                            exit(0);
                        }
                    }
                    else
                    {
                        cout << "Invalid instruction at pc=" << pc << endl;
                        exit(0);
                    }
                }
            }
            // cout<<pc<<" ";
            //cout << l << "isnt" << endl;
        }
    }
    pc = 0;
    la = 0;
    // for(auto itr=label.begin();itr!=label.end();itr++){
    //     cout<<itr->first<<" "<<itr->second<<endl;

    // }
    asmbl.close();
    //loading memory
    asmbl.open("asmbl.asm");
    while (!asmbl.eof())
    {
        getline(asmbl, l);

        if (regex_search(l, match, re) == true)
        {
            string k = match.str(1);
            istringstream ss1(match.str(2));
            string wrd;
            ss1 >> wrd;
            //cout<<wrd<<" j";
            if (wrd == ".word")
            {

                while (ss1 >> wrd)
                {
                    //cout << wrd << " i";
                    if (wrd.substr(0, 2) == "0b")
                    { //binary
                        int t = stringbin_to_dec(wrd);
                        stringstream ss;
                        ss << hex << t;
                        string hexv;
                        ss >> hexv;
                        loadhexword(hexv);
                    }
                    if (wrd.substr(0, 2) == "0x") //hexadecimal
                    {
                        string hexv = wrd.substr(2, wrd.length() - 2);
                        loadhexword(hexv);
                    }
                    else
                    {
                        regex re1("^(^-?[0-9]+$)");
                        smatch m1;
                        if (regex_search(wrd, m1, re1) == true)
                        { //decimal
                            // cout<<"hh"<<endl;
                            int t = stringdec_to_dec(wrd);
                            stringstream ss;
                            ss << hex << t;
                            string hexv;
                            ss >> hexv;
                            loadhexword(hexv);
                        }
                        else
                        { //label
                            auto f = label.find(wrd);
                            if (f == label.end())
                            {
                                cout << wrd << " label not defined but used" << endl;
                            }
                            else
                            {
                                int t = f->second;
                                stringstream ss;
                                ss << hex << t;
                                string hexv;
                                ss >> hexv;
                                loadhexword(hexv);
                            }
                        }
                    }
                    la += 4;
                }
            }
            if (wrd == ".byte")
            {
                while (ss1 >> wrd)
                {
                    if (wrd.substr(0, 2) == "0b")
                    { //binary
                        int t = stringbin_to_dec(wrd);
                        stringstream ss;
                        ss << hex << t;
                        string hexv;
                        ss >> hexv;
                        if (hexv.length() == 1)
                            hexv = "0" + hexv;
                        mem[la] = hexv.substr(hexv.length() - 2, 2);
                    }
                    if (wrd.substr(0, 2) == "0x") //hexadecimal
                    {
                        string hexv = wrd.substr(2, wrd.length() - 2);
                        if (hexv.length() == 1)
                            hexv = "0" + hexv;
                        mem[la] = hexv.substr(hexv.length() - 2, 2);
                    }
                    else
                    {
                        regex re1("^(^-?[0-9]+$)");
                        smatch m1;
                        if (regex_search(wrd, m1, re1) == true)
                        { //decimal
                            int t = stringdec_to_dec(wrd);
                            stringstream ss;
                            ss << hex << t;
                            string hexv;
                            ss >> hexv;
                            if (hexv.length() == 1)
                                hexv = "0" + hexv;
                            mem[la] = hexv.substr(hexv.length() - 2, 2);
                        }
                        else
                        { //label
                            auto f = label.find(wrd);
                            if (f == label.end())
                            {
                                cout << wrd << " label not defined but used" << endl;
                            }
                            else
                            {
                                //insert
                                int t = f->second;
                                stringstream ss;
                                ss << hex << t;
                                string hexv;
                                ss >> hexv;
                                if (hexv.length() == 1)
                                    hexv = "0" + hexv;
                                mem[la] = hexv.substr(hexv.length() - 2, 2);
                            }
                        }
                    }
                    la += 1;
                }
            }
            if (wrd == ".half")
            {
                while (ss1 >> wrd)
                {
                    if (wrd.substr(0, 2) == "0b")
                    { //binary
                        int t = stringbin_to_dec(wrd);
                        stringstream ss;
                        ss << hex << t;
                        string hexv;
                        ss >> hexv;
                        loadhexhalf(hexv);
                    }
                    if (wrd.substr(0, 2) == "0x") //hexadecimal
                    {
                        string hexv = wrd.substr(2, wrd.length() - 2);
                        loadhexhalf(hexv);
                    }
                    else
                    {
                        regex re1("^(^-?[0-9]+$)");
                        smatch m1;
                        if (regex_search(wrd, m1, re1) == true)
                        { //decimal
                            int t = stringdec_to_dec(wrd);
                            stringstream ss;
                            ss << hex << t;
                            string hexv;
                            ss >> hexv;
                            loadhexhalf(hexv);
                        }
                        else
                        { //label
                            auto f = label.find(wrd);
                            if (f == label.end())
                            {
                                cout << wrd << " label not defined but used" << endl;
                            }
                            else
                            {
                                int t = f->second;
                                stringstream ss;
                                ss << hex << t;
                                string hexv;
                                ss >> hexv;
                                loadhexhalf(hexv);
                            }
                        }
                    }
                    la += 2;
                }
            }

            if (wrd == ".asciiz")
            {
                ss1 >> wrd;
                char c = wrd[0];
                char cl = wrd[wrd.length() - 1];
                if (c != '\"' || cl != '\"')
                {
                    cout << "Unable to parse string error!" << endl;
                    exit(0);
                }
                for (int i = 1; i < wrd.length() - 1; i++)
                {
                    c = wrd[i];
                    int tmp = (int)c;
                    stringstream ss;
                    ss << hex << tmp;
                    string hexv;
                    ss >> hexv;
                    mem[la] = hexv;
                    la += 1;
                }
                mem[la] = "\0";
                la += 1;
                if (ss1 >> wrd)
                {
                    cout << "ERROR!" << endl;
                    exit(0);
                }
            }
        }
    }
    //cout<<la<<endl;
    cout << "Loaded Memory-" << endl;

    for (int i = 0; i < la; i++)
    {
        cout << mem[i] << " ";
        if (i % 4 == 3)
            cout << endl;
    }
    t = 1;
    asmbl.close();
    asmbl.open("asmbl.asm");
    while (!asmbl.eof())
    {

        getline(asmbl, l);
        //cout<<l<<endl;
        istringstream stt(l);
        string lll;
        stt >> lll;
        if (lll == "\0")
            continue;
        if (l == ".data")
        {
            t = 0;
            continue;
        }
        if (l == ".text")
        {
            t = 1;
            continue;
        }
        if (t == 1)
        {
            if (regex_search(l, match, re) == false)
            {
                string cd = runinst(l);
                mach << cd << endl;
            }
        }
    }
    for (int i = 0; i < la; i++)
    {
        long long int adh = 268435456 + i;
        string st;
        stringstream scc;
        scc << hex << adh;
        scc>>st;
        mach <<"0x"<< st << " "
             << "0x" << mem[i] << endl;
    }
    mach.close();

}