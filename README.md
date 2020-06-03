# RISC-V-ISA-SIMULATOR

## Functional Simulator for subset of RISC-V instruction Set

### 1. How to execute
   * g++ PHASE1.cpp
   * ./a.out
   * g++ PHASE2.cpp
   * ./a.out

### 2. Work Split
   * Phase 1
       * MAIN FUNCTION (INCLUDING LABELS AND DIRECTIVES):	Nikita 
       * R-TYPE, I-TYPE, S-TYPE,J-TYPE:                    Pranjali and Gurseerat
       * U TYPE:                                         	David 
       * SB TYPE: 	                                        Govind	
   * Phase 2
       * FETCH & DECODE:                     Pranjali and Gurseerat
       * EXECUTE, MEMORY UPDATE, WRITEBACK:	Govind, Nikita and David

## Pipelined Functional Simulator for Subset of RISC-V instruction set

### 1. How to execute
   * g++ PHASE1.cpp
       * Generate Machine Code *Input File: asmbl.asm *Output File: mcode.mc
   * g++ PHASE3.cpp
   * ./a.out

### 2. Work Split
   * Phase 3
       * CONCEPT BUILDING AND BASIC PIPELINING :DAVID
       * DATA FORWARDING :GURSEERAT & PRANJALI
       * STALLING :GURSEERAT & PRANJALI
       * FLUSHING :NIKITA & GOVIND
       * BIT PREDICTION :NIKITA & GOVIND

## Team Members
   * GURSEERAT KAUR:   2018CSB1093
   * NIKITA AGARWAL:   2018CSB1109
   * PRANJALI BAJPAI:  2018EEB1243
   * GOVIND BANSAL:    2018EEB 1147  
   * DAVID DAHIYA:     2018EEB1143
