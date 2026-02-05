#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define REG_COUNT 8
//Structs/Enums and Typedefs
typedef enum {
    UNDEFINED,
    MOV,
    ADD,
    SUB,
    END,
    CAST,
    MUL,
    DIV,
    MOD,
    POW,
    IN,
    OUT,
}Opcode;
typedef struct {
    char code[7];
    char op1[6];
    char op2[12];
}Inst;
typedef struct {
    union {
        int* intp;
        float* floatp;
    }value;
    enum {
        ERROR,INT,FLOAT,NOTHING,CONSTANT
    }type;
}Operand;
typedef struct {
    Opcode opcode;
    Operand op1;
    Operand op2;
}ParsedInst;
typedef struct {
    FILE* outputfile;
    int run;
    union {
        int iconst;
        float fconst;
    }constant;
    int r[REG_COUNT];
    float x[REG_COUNT];
    const char* cursor; 
}Machine;
Machine m1={0};
//Functions
//Memory Operations
int FMOV(Operand op1,Operand op2){
    if(op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type==ERROR || op2.type==NOTHING){
        fprintf(m1.outputfile,"MOV Failed: Invalid operands");
        return 1;
    }
    fprintf(m1.outputfile,"Execute MOV \n");
    if(op1.type==INT){
        *op1.value.intp=*op2.value.intp;
    }
    else if (op1.type==FLOAT){
        *op1.value.floatp=*op2.value.floatp;
    }
    fprintf(m1.outputfile,"Operand status %d",*op1.value.intp);
    return 0;
}
int FCAST(Operand op1,Operand op2){
     if(op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type==ERROR || op2.type==NOTHING){
        fprintf(m1.outputfile,"CAST Failed: Invalid operands");
        return 1;
    }
    if(op1.type==INT){
        *op1.value.intp=(int)*op2.value.floatp;
    }
    else if(op1.type==FLOAT){
        *op1.value.floatp=(float)*op2.value.intp;
    }
    return 0;
}
//Arithemtic/Math operations
int FADD(Operand op1,Operand op2){
    if(op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type==ERROR || op2.type==NOTHING){
        fprintf(m1.outputfile,"ADD Failed: Invalid operands");
        return 1;
    }
    if(op1.type==INT){
        *op1.value.intp+=*op2.value.intp;
    }
    else if (op1.type==FLOAT){
        *op1.value.floatp+=*op2.value.floatp;
    }
    return 0;
}
int FSUB(Operand op1,Operand op2){
   if(op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type==ERROR || op2.type==NOTHING){
        fprintf(m1.outputfile,"SUB Failed: Invalid operands");
        return 1;
    }
    if(op1.type==INT){
        *op1.value.intp-=*op2.value.intp;
    }
    else if (op1.type==FLOAT){
        *op1.value.floatp-=*op2.value.floatp;
    }
    return 0;
}
int FMUL(Operand op1,Operand op2){
     if(op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type==ERROR || op2.type==NOTHING){
        fprintf(m1.outputfile,"MUL Failed: Invalid operands");
        return 1;
    }
    if(op1.type==INT){
        *op1.value.intp*=*op2.value.intp;
    }
    else if (op1.type==FLOAT){
        *op1.value.floatp*=*op2.value.floatp;
    }
    return 0;
}
int FDIV(Operand op1,Operand op2){
    if(op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type==ERROR || op2.type==NOTHING){
        fprintf(m1.outputfile,"DIV Failed: Invalid operands");
        return 1;
    }
    if (*op2.value.intp==0) {
        fprintf(m1.outputfile,"DIV Failed: Cannot divide by 0");
        return 1;
    }
    if(op1.type==INT){
        *op1.value.intp/=*op2.value.intp;
    }
    else if (op1.type==FLOAT){
        *op1.value.floatp/=*op2.value.floatp;
    }
    return 0;

}
int FMOD(Operand op1,Operand op2){
    if(op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type==ERROR || op2.type==NOTHING || op1.type==FLOAT){
        fprintf(m1.outputfile,"MOD Failed: Invalid operands");
        return 1;
    }
    if (*op2.value.intp==0) {
        fprintf(m1.outputfile,"MOD Failed: Cannot module by 0");
        return 1;
    }
    *op1.value.intp%=*op2.value.intp;
    return 0;
}
int FPOW(Operand op1,Operand op2){
     if(op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type==ERROR || op2.type==NOTHING){
        fprintf(m1.outputfile,"POW Failed: Invalid operands");
        return 1;
    }
    if(op1.type==INT){
        *op1.value.intp=lround(pow(*op1.value.intp,*op2.value.intp));
    }
    else if (op1.type==FLOAT){
    *op1.value.floatp=pow(*op1.value.floatp,*op2.value.floatp);
    }
    return 0;
}
//Jumps and Misc
int FEND(Operand op1,Operand op2){
    if (op1.type==NOTHING && op2.type==NOTHING){
        m1.run=0;
        return 0;
    }
    fprintf(m1.outputfile,"END Failed: Invalid operands");
    return 1;
}
int FIN(Operand op1,Operand op2){
    if(op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type!=NOTHING){
        fprintf(m1.outputfile,"IN Failed: Invalid operands");
        return 1;
    }
    if(op1.type==INT){
        scanf("%d",op1.value.intp);
    }
    else {
        scanf("%f",op1.value.floatp);
    }
    return 0;
}
int FOUT(Operand op1,Operand op2){
    if(op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT|| op2.type!=NOTHING){
        fprintf(m1.outputfile,"OUT Failed: Invalid operands");
        return 1;
}
    if(op1.type==INT){
        printf("Output is:%d\n",*op1.value.intp);
    }
    else {
        printf("Output is:%f\n",*op1.value.floatp);
    }
    return 0;
}
//Interpreter functions
void Reset(Inst* curInst,ParsedInst* ParseInst){
    strcpy(curInst->code," ");
    strcpy(curInst->op1," ");
    strcpy(curInst->op2," ");
    ParseInst->op1.value.intp=NULL;
    ParseInst->op2.value.floatp=NULL;
    ParseInst->opcode=UNDEFINED;
    m1.constant.iconst=0;
}
Opcode CheckOpcode(Inst* curInst){
    if(strcmp(curInst->code,"MOV")==0) return MOV;
    if(strcmp(curInst->code,"ADD")==0) return ADD;
    if(strcmp(curInst->code,"SUB")==0) return SUB;
    if(strcmp(curInst->code,"END")==0) return END;
    if(strcmp(curInst->code,"CAST")==0) return CAST;
    if(strcmp(curInst->code,"MUL")==0) return MUL;
    if(strcmp(curInst->code,"DIV")==0) return DIV;
    if(strcmp(curInst->code,"POW")==0) return POW;
    if(strcmp(curInst->code,"IN")==0) return IN;
    if(strcmp(curInst->code,"OUT")==0) return OUT;
    return UNDEFINED;
}
int ReadInstruction(Inst* curInst,char* line){
    char extra=' ';
    sscanf(line,"%6s %5s %12s %c",curInst->code,curInst->op1,curInst->op2,&extra);
    if(extra!=' '){
        return 1;
    }
    return 0;
   
}
int Execute(ParsedInst* cInst){
    switch (cInst->opcode)
    {   case MOV: return FMOV(cInst->op1,cInst->op2);
        case ADD: return FADD(cInst->op1,cInst->op2);
        case SUB: return FSUB(cInst->op1,cInst->op2);
        case END: return FEND(cInst->op1,cInst->op2);
        case CAST: return FCAST(cInst->op1,cInst->op2);
        case MUL: return FMUL(cInst->op1,cInst->op2);
        case DIV: return FDIV(cInst->op1,cInst->op2);
        case POW: return FPOW(cInst->op1,cInst->op2);
        case IN: return FIN(cInst->op1,cInst->op2);
        case OUT: return FOUT(cInst->op1,cInst->op2);
        default: return 1;
    
    }
    return 0;
}
Operand ParseOperand(char* operand){
    char* endptr;
    Operand op;
    op.type=ERROR;
    op.value.floatp=NULL;
    int reg=0;
    if (strcmp(operand," ")==0){
        op.type=NOTHING;
        return op;
    }
    if(operand[0]=='R' || operand[0]=='X'){
        reg=strtol(operand+1,&endptr,10);
        if(*endptr!='\0') return op;
        if(reg>0 && reg<REG_COUNT+1){
            fprintf(m1.outputfile,"Operand parsed as register %d\n",reg);
            if (operand[0]=='R'){
                op.value.intp=&m1.r[reg-1];
                op.type=INT;
            }
            else {
                op.value.floatp=&m1.x[reg-1];
                op.type=FLOAT;
            }
            return op;
        }
        fprintf(m1.outputfile,"error Parsing operand (not a valid register) %d\n",reg);
        return op;
    }
    m1.constant.iconst=strtol(operand,&endptr,10);
    if(*endptr=='.'){
        m1.constant.fconst=strtof(operand,&endptr);
    }
    if(*endptr!='\0'){
        
        fprintf(m1.outputfile,"error parsing operand (invalid constant):%s\n",endptr);
        return op;
    }
    fprintf(m1.outputfile,"Operand parsed as constant:%d\n",m1.constant);
    op.type=CONSTANT;
    op.value.intp=&m1.constant.iconst;
    return op;
    
}

void sgetline(char** cursor,char* line){
    int offset=0;
    sscanf(*cursor,"\n%99[^#\n]%n",line,&offset);
    *cursor+=offset;
    if(line[0]=='\0') return;
    offset=0;
    while(**cursor!='\0'&&**cursor!='\n') (*cursor)++;
    
}



char* ReadFile(FILE* File){
    fseek(File,0,SEEK_END);
    int count=ftell(File);
    char* str=calloc(count+1,sizeof(char));
    rewind(File);
    fread(str,sizeof(char),count,File);
    str[count]='\0';
    return str;
}

int main(int argc,char* argv[]){
    FILE* File;
    m1.run=1;
    ParsedInst ParseInst;
    Inst curInst;
    File=fopen("program.txt","r");
    m1.outputfile=fopen("debug.txt","w");
    if(File==NULL){
        fprintf(stderr,"Failed to load file");
        return 1;
    }
    const char* program=ReadFile(File);
    char* cursor=program;
    while (m1.run){
        Reset(&curInst,&ParseInst);
        char line[100];
        line[0]='\0';
        sgetline(&cursor,line);
        if(line[0]=='\0') break;
        ReadInstruction(&curInst,line);
        ParseInst.opcode=CheckOpcode(&curInst);
        ParseInst.op1=ParseOperand(curInst.op1);
        ParseInst.op2=ParseOperand(curInst.op2);
        Execute(&ParseInst);
    }
    fprintf(m1.outputfile,"The output for X is:%f\n The output for R is:%d\n",m1.x[0],m1.r[0]);
    fprintf(m1.outputfile,"The program ended with code:%d",m1.run);
    fclose(File);
    fclose(m1.outputfile);
    free(program);
    return 0;

}