A simple assembly interpreter made entirely in C. The instruction set i have designed myself, and the interpreter itself is till in development.

Current Status:

\-Implemented basic instructions:

\-MOV (To move the second operand into the first operand) requires the first operand to be a storable location (for now only registers)

\-ADD (To add the two operands, and store the result in the first operand) requires the first operand to be a register

\-SUB (To subtract the second operand from the first, and store the result in the first operand) requires the first operand to be a register

\-END (To finish execution of program with a error code of 0) requires the two operands to be empty

\-Implemented two types of registers, the R family, and X family of registers. The R registers being used for intergers and X for floating point numbers

\-Implemented constant operands. They cannot be used as the first operand, as the first operand usually stores the result.

Usage details:

In order to start using the language, you need the compiled exe a program.txt file (case sensitive).

\-Compilation

To compile the program its as simple as calling your preffered C compiler of choice, and simply compiling, no extra flags, no external libraries used

\-Writing Programs

In you program.txt file, you write each instruction line by line, i reccomend using notepad so that its clear what exatcly you are writing, and what will the interpreter read. Each instruction consists of the instruction name (MOV,ADD,SUB,END etc).

Example:

MOV R1 5

Note that comments are also allowed using the '#', everything after the '#' will be ignored

Registers:

This interpreter uses 16 registers, 8 of them are int registers going by the name of R, and the oher 8 floaitng point registers going by the name of X. in order to use them you have to use the appropriate prefix (either R or X) and then the register which you want to use.

The interpeter doesnt assume what is in these registers, only the first operand dictates what type of operation will be done, and what way the second operand will be interpeted, so in ADD R1 X1 X1's bits will be interpreted as a interger, keep in mind this is not a value cast. Same applies for constants.

Constants:

This interpreter supports both int and float constants. for a int constant you have to just write the number down as an operand (ex. MOV R1 500), as for floating point numbers use the '.' to signify that its a floating number (MOV X1 5.0). Keep in mind that the first operand dictates how these will be interpreted.

MOV R1 5.0 does not move 5 into R1. it moves the floating point representation of 5.0 in bits into R1, which is approximatly a million or something. Do not mix registers with their opposite values

As of this moment the intepreter uses the debug.txt to print out information. including the value of X1. At a later date this will be fixed.

Issues to resolve:

\-No error checking, if you make an error in writing the program will not stop, however rest assured that no memory corruption or segmentation faults take place (i think)

Features to add

\-IN and OUT instructions, which is the interpeters input and output

\-CAST to cast the value of one type on to another

\-CMP and JUMP, for control flow

\-and many more..
