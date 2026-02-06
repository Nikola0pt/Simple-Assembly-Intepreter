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
    PUTC,
}Opcode;
typedef struct {
    char code[7];
    char op1[100];
    char op2[12];
}Inst;
typedef struct {
    union {
        int* intp;
        float* floatp;
        char* str;
    }value;
    enum {
        ERROR,INT,FLOAT,NOTHING,CONSTANT,STRING
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
    char strbuf[100]; 
}Machine;
Machine m1={0};
//Functions
//Memory Operations
int FMOV(Operand op1,Operand op2){
    if(op1.type==STRING || op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type==ERROR || op2.type==NOTHING || op2.type==STRING){
        printf("MOV Failed: Invalid operands\n");
        return 1;
    }
    if(op1.type==INT){
        *op1.value.intp=*op2.value.intp;
    }
    else if (op1.type==FLOAT){
        *op1.value.floatp=*op2.value.floatp;
    }
    return 0;
}
int FCAST(Operand op1,Operand op2){
     if(op1.type==STRING || op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type==ERROR || op2.type==NOTHING || op2.type==STRING){
        printf("CAST Failed: Invalid operands\n");
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
    if(op1.type==STRING || op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type==ERROR || op2.type==NOTHING || op2.type==STRING){
        printf("ADD Failed: Invalid operands\n");
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
   if(op1.type==STRING || op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type==ERROR || op2.type==NOTHING || op2.type==STRING){
        printf("SUB Failed: Invalid operands\n");
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
     if(op1.type==STRING || op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type==ERROR || op2.type==NOTHING || op2.type==STRING){
        printf("MUL Failed: Invalid operands\n");
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
    if(op1.type==STRING || op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type==ERROR || op2.type==NOTHING || op2.type==STRING){
        printf("DIV Failed: Invalid operands\n");
        return 1;
    }
    if (*op2.value.intp==0) {
        printf("DIV Failed: Cannot divide by 0\n");
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
    if(op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type==ERROR || op2.type==NOTHING || op1.type==FLOAT || op1.type==STRING || op2.type==STRING){
        printf("MOD Failed: Invalid operands\n");
        return 1;
    }
    if (*op2.value.intp==0) {
        printf("MOD Failed: Cannot module by 0\n");
        return 1;
    }
    *op1.value.intp%=*op2.value.intp;
    return 0;
}
int FPOW(Operand op1,Operand op2){
     if(op1.type==STRING || op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type==ERROR || op2.type==NOTHING || op2.type==STRING){
        printf("POW Failed: Invalid operands\n");
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
    printf("END Failed: Invalid operands\n");
    return 1;
}
int FIN(Operand op1,Operand op2){
    if(op1.type==NOTHING || op1.type==ERROR || op1.type==CONSTANT || op2.type!=NOTHING || op1.type==STRING){
        printf("IN Failed: Invalid operands\n");
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
        printf("OUT Failed: Invalid operands\n");
        return 1;
}
    if(op1.type==INT){
        printf("%d",*op1.value.intp);
    }
    else if(op1.type==FLOAT) {
        printf("%f",*op1.value.floatp);
    }
    else if(op1.type==STRING){
        printf("%s",op1.value.str);
    }
    return 0;
}
int FPUTC(Operand op1,Operand op2){
    if(op1.type!=INT || op2.type!=NOTHING){
        printf("PUTC Failed: Invalid Operands\n");
        return 1;
    }
    printf("%c",(char)*op1.value.intp);
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
    m1.strbuf[0]='\0';
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
    if(strcmp(curInst->code,"PUTC")==0) return PUTC;
    return UNDEFINED;
}
int ReadInstruction(Inst* curInst,char* line){
    char extra=' ';
    int offset=0;
    sscanf(line,"%6s %n",curInst->code,&offset);
    if(line[offset]=='"'){
        curInst->op1[0]='"';
        offset++;
        char found=0;
        for(int i=1;line[offset]!='\0';i++,offset++){
            curInst->op1[i]=line[offset];
            if(line[offset]=='"') {
                found=1;
                curInst->op1[i+1]='\0';
                break;
            }
        }
        if(!found){ 
        curInst->op1[0]='\0';
        return 1; }
        offset++;
        sscanf(line+offset,"%12s %c",curInst->op2,&extra);
    }
    else sscanf(line+offset,"%99s %12s %c",curInst->op1,curInst->op2,&extra);
  //  sscanf(line,"%6s %99s %12s %c",curInst->code,curInst->op1,curInst->op2,&extra);
    if(extra!=' '){
        return 1;
    }
    if(strcmp(curInst->code," ")==0) return 1;
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
        case PUTC: return FPUTC(cInst->op1,cInst->op2);
        default: return 1;
    
    }
    return 0;
}
Operand ParseStringLiteral(char* op){
    Operand op1;
    op1.type=STRING;
    int j=0;
    for(int i=1;op[i]!='"' && op[i]!='\0';i++,j++){
        if(op[i]=='\\'){
            if(op[++i]=='n'){
                m1.strbuf[j]='\n';
            }
            else {
                m1.strbuf[j]=op[i];
                }
        }
        else {
            m1.strbuf[j]=op[i];
        }
    }
    m1.strbuf[j]='\0';
    op1.value.str=m1.strbuf;
    return op1;
}
Operand ParseOperand(char* operand){
    char* endptr;
    Operand op;
    op.type=ERROR;
    op.value.floatp=NULL;
    int reg=0;
    if(operand[0]=='"') return ParseStringLiteral(operand);
    if (strcmp(operand," ")==0){
        op.type=NOTHING;
        return op;
    }
    if(operand[0]=='R' || operand[0]=='X'){
        reg=strtol(operand+1,&endptr,10);
        if(*endptr!='\0') return op;
        if(reg>0 && reg<REG_COUNT+1){
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
        return op;
    }
    m1.constant.iconst=strtol(operand,&endptr,10);
    if(*endptr=='.'){
        m1.constant.fconst=strtof(operand,&endptr);
    }
    if(*endptr!='\0'){
        return op;
    }
    op.type=CONSTANT;
    op.value.intp=&m1.constant.iconst;
    return op;
    
}

int sgetline(char* line){
    int offset=0;
    char temp[100];
    sscanf(m1.cursor,"\n%99[^\n]%n",temp,&offset);
    m1.cursor+=offset;
    if(temp[0]=='\0') return 1;
    offset=0;
    sscanf(temp,"%99[^#]",line);
    while(*m1.cursor!='\0'&&*m1.cursor!='\n') (m1.cursor)++;
    return 0;
    
}
void DumpState(Inst* CurInst){
    fprintf(m1.outputfile,"Current Instruction: %s %s %s\n",CurInst->code,CurInst->op1,CurInst->op2);
    fprintf(m1.outputfile,"State of registers:\n");
    for(int i=0;i<REG_COUNT;i++){
        fprintf(m1.outputfile,"R%d=%d ",i+1,m1.r[i]);
    }
    fprintf(m1.outputfile,"\n");
    for(int i=0;i<REG_COUNT;i++){
        fprintf(m1.outputfile,"X%d=%f ",i+1,m1.x[i]);
    }
    fprintf(m1.outputfile,"\n");
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
        fprintf(m1.outputfile,"Failed to load file");
        return 1;
    }
    const char* const program=ReadFile(File);
    m1.cursor=program;
    while (m1.run){
        Reset(&curInst,&ParseInst);
        char line[100];
        line[0]='\0';
        if(sgetline(line)) break;
        if(line[0]=='\0') continue;
        if(ReadInstruction(&curInst,line)) {
            printf("ERROR:Instruction read unsuccessfully\n");
            break;
        }
        DumpState(&curInst);
        ParseInst.opcode=CheckOpcode(&curInst);
        if(ParseInst.opcode==UNDEFINED){
            printf("ERROR:Unknown/undefined instruction\n");
            break;
        }
        ParseInst.op1=ParseOperand(curInst.op1);
        if(ParseInst.op1.type==ERROR){
            printf("ERROR:First operand failed parse\n");
            break;
        }
        ParseInst.op2=ParseOperand(curInst.op2);
        if(ParseInst.op2.type==ERROR){
            printf("ERROR:Second operand failed parse\n");
            break;
        }
        if(Execute(&ParseInst)){
            printf("ERROR: Execution failed\n");
            break;
        }
        DumpState(&curInst);
    }
    printf("\nProgram closed with a error code %d\n",m1.run);
    fclose(File);
    fclose(m1.outputfile);
    free(program);
    return 0;

}