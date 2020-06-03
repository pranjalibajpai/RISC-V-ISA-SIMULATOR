# RISC-V-ISA-SIMULATOR

Functional Simulator for subset of RISC-V instruction Set
PHASE 1 & 2:
Directory Structure:
  |
  |- SOURCE CODES
      |
      |- PHASE1.cpp
      |- PHASE2.cpp
      |- opcode.txt
      |- data.txt
      |- operation.txt
  |- TESTS
      |
      |- bubble_sort.asm
      |- factorial.asm
      |- fibonacci.asm
  |- OUTPUTS
      |- mcode(sample).mc
      |- data_mem(sample).mc
      |- data_r(sample).mc
      |- output_bubbleSort
      |- output_fibonacci(5)
      |- output_factorial(7!)
  |- DOCS
      |- Design docs
      |- GROUP_README
      
How to execute
$ g++ PHASE1.cpp
$ ./a.out
$ g++ PHASE2.cpp
$ ./a.out

Limitations
●	Pseudo instructions not supported.
●	Data memory is limited to 6000 bytes. 
●	Floating point numbers not supported.

Work Split:
PHASE-I
MAIN FUNCTION (INCLUDING LABELS AND DIRECTIVES):	Nikita 
R-TYPE, I-TYPE, S-TYPE,J-TYPE:                    Pranjali and Gurseerat
U TYPE:                                         	David 
SB TYPE: 	                                        Govind	    
PHASE-II
FETCH & DECODE:                     Pranjali and Gurseerat
EXECUTE, MEMORY UPDATE, WRITEBACK:	Govind, Nikita and David
____________________________________________________________________________
PHASE 3:
Pipelined Functional Simulator for Subset of RISC-V instruction set
Directory Structure:
  |
  |- SOURCE CODES
      |
      |- PHASE1.cpp
      |- PHASE3.cpp
      |- opcode.txt
  |- TESTS
      |
      |- bubble_sort.docx
      |- factorial.docx
      |- fibcode.docx
      |- PALINDROME.docx
  |- DOCS
      |- REPORT(README).pdf

RUNNING THE CODE:
-Generate Machine Code: g++ PHASE1.cpp (Input File: asmbl.asm)
(mcode.mc created)
-Running Simulation : g++ PHASE3.cpp

INPUT & OUTPUT:
A machine code is sent as input to the simulator and its pipelined execution
is carried out using the techniques of bit prediction, stalling, flushing and
data forwarding. These techniques aim to optimize the program by
minimizing the execution time ( or the number of cycles ) and running the
maximum number of instructions possible in the same cycle.
The program outputs the details of all the steps executed per cycle along
with the various control changes like jumps, predictions, flushes and stalls.
The following stats are printed at the end of execution:
For both Pipelined & Non-Pipelined Execution:
Stat1: Total number of cycles
Stat2: Total instructions executed
Stat3: CPI (Cycles per Instruction)
Stat4: Number of Data-transfer (load and store) instructions executed
Stat5: Number of ALU instructions executed
Only for Pipelined execution:
Stat6: Number of Control instructions executed
Stat7: Number of stalls/bubbles in the pipeline
Stat8: Number of data hazards
Stat9: Number of control hazards
Stat10: Number of branch mispredictions
Stat11: Number of stalls due to data hazards
Stat12: Number of stalls due to control hazards
The register and memory data is written to files data_r.mc and data_mem.mc
respectively. The stats are written to a stats.txt file.

FUNCTIONALITIES & FEATURES:
● Pipelining : Functionality to switch between Pipelined and Non
Pipelined execution.
● Data Forwarding: Knob to enable /disable data forwarding.
● Pipeline Registers: Functionality to output register values and values of
the inter state buffers
○ At the end of each cycle
○ At the end of a specified cycle number
● Register File: Functionality to print register file at the end of each cycle
● Bit prediction: Knob to enable/disable bit prediction.

Test plan
We have successfully tested the simulator with following assembly programs:
● Fibonacci Program
● Bubble Sort
● Factorial Program
● Palindrome

Work Split:
CONCEPT BUILDING AND BASIC PIPELINING :DAVID
BIT PREDICTION :NIKITA & GOVIND
FLUSHING :NIKITA & GOVIND
STALLING :GURSEERAT & PRANJALI
DATA FORWARDING :GURSEERAT & PRANJALI
____________________________________________________________________________
Team Members:
GURSEERAT KAUR 2018CSB1093
NIKITA AGARWAL 2018CSB1109
PRANJALI BAJPAI 2018EEB1243
GOVIND BANSAL 2018EEB 1147
DAVID DAHIYA 2018EEB1143
