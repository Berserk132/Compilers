#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

/*
int ia; int ib; int ic;                         { declare three integer variables }
{int ia }                                       { raise duplicate variable declaration error }
real ra; real rb; real rc;                      { declare three double variables }
bool ba; bool bb; bool bc;                      { declare three boolean variables }

read ia; read ib; read ic;                      { read the three integer variables }
read ra; read rb; read rc;                      { read the three double variables }
read ba; read bb; read bc;                      { read the three boolean variables }

write ia+ib+ic;                                 { operations between integer variables }
write ia-ib-ic;                                 { operations between integer variables }
write ia*ib*ic;                                 { operations between integer variables }
write ia/ib/ic;                                 { operations between integer variables }

write ra+rb+rc;                                 { operations between double variables }
write ra-rb-rc;                                 { operations between double variables }
write ra*rb*rc;                                 { operations between double variables }
write ra/rb/rc;                                 { operations between double variables }


{ write ra + ia;}                               { this will fail if uncommented (operation between different type)}
{ write ra + ba;}                               { this will fail if uncommented (operation between different type)}

if ba then write 5 + 60 end;                    { if ba equal true print 65 }
if bb then write 60.5 + 80.6 end;               { if ba equal true do print the value }
if bc then write 3 end;                         { if ba equal true print 3 }

if ba = bb then                                 { if ba equals bb then check if ba equals bc then print 123 }
    if ba = bc then
        write 123
    end
end;

int n; int i; bool equal;                       { declare new three variable }
i := 0;                                         { set i equals zero }
read n;                                         { read in n value }
repeat                                          { start of while loop }
    write i;                                    { print i value }
     equal:= i = n;                             { set equals to the return value of i == n }
     write equal                                { print equal }
until equal = 0;                                { condition of the while loop }


int ctr;                                        { declare a integer variable }
{ write x; }                                    { write undefined variable , will crash if uncommented }
{ ctr := x }                                    { write assign to undefined variable , will crash if uncommented }
{ write 5.5 + 10; }                             { raise error if uncommented }
int result;                                     { declare a integer variable }
ctr := 10;                                      { set 10 to ctr }

if 0<ctr then                                   { compute only if ctr>=1 }
  result := ctr;                                { set result = ctr }
  repeat                                        { start repeat statement }
    result := result + 1;                       { increase result by 1 }
    write result;                               { write result "this will print Values from 11 to 20" }
    ctr:=ctr-1                                  { decrease ctr by 1 }
  until ctr=0                                   { end repeat }
end
*/

// sequence of statements separated by ;
// no procedures - no declarations
// all variables are integers
// variables are declared simply by assigning values to them :=
// if-statement: if (boolean) then [else] end
// repeat-statement: repeat until (boolean)
// boolean only in if and repeat conditions < = and two mathematical expressions
// math expressions integers only, + - * / ^
// I/O read write
// Comments {}

////////////////////////////////////////////////////////////////////////////////////
// Strings /////////////////////////////////////////////////////////////////////////

bool Equals(const char* a, const char* b)
{
    return strcmp(a, b)==0;
}

bool StartsWith(const char* a, const char* b)
{
    int nb=strlen(b);
    return strncmp(a, b, nb)==0;
}

void Copy(char* a, const char* b, int n=0)
{
    if(n>0) {strncpy(a, b, n); a[n]=0;}
    else strcpy(a, b);
}

void AllocateAndCopy(char** a, const char* b)
{
    if(b==0) {*a=0; return;}
    int n=strlen(b);
    *a=new char[n+1];
    strcpy(*a, b);
}

////////////////////////////////////////////////////////////////////////////////////
// Input and Output ////////////////////////////////////////////////////////////////

#define MAX_LINE_LENGTH 10000

struct InFile
{
    FILE* file;
    int cur_line_num;

    char line_buf[MAX_LINE_LENGTH];
    int cur_ind, cur_line_size;

    InFile(const char* str) {file=0; if(str) file=fopen(str, "r"); cur_line_size=0; cur_ind=0; cur_line_num=0;}
    ~InFile(){if(file) fclose(file);}

    void SkipSpaces()
    {
        while(cur_ind<cur_line_size)
        {
            char ch=line_buf[cur_ind];
            if(ch!=' ' && ch!='\t' && ch!='\r' && ch!='\n') break;
            cur_ind++;
        }
    }

    bool SkipUpto(const char* str)
    {
        while(true)
        {
            SkipSpaces();
            while(cur_ind>=cur_line_size) {if(!GetNewLine()) return false; SkipSpaces();}

            if(StartsWith(&line_buf[cur_ind], str))
            {
                cur_ind+=strlen(str);
                return true;
            }
            cur_ind++;
        }
        return false;
    }

    bool GetNewLine()
    {
        cur_ind=0; line_buf[0]=0;
        if(!fgets(line_buf, MAX_LINE_LENGTH, file)) return false;
        cur_line_size=strlen(line_buf);
        if(cur_line_size==0) return false; // End of file
        cur_line_num++;
        return true;
    }

    char* GetNextTokenStr()
    {
        SkipSpaces();
        while(cur_ind>=cur_line_size) {if(!GetNewLine()) return 0; SkipSpaces();}
        return &line_buf[cur_ind];
    }

    void Advance(int num)
    {
        cur_ind+=num;
    }
};

struct OutFile
{
    FILE* file;
    OutFile(const char* str) {file=0; if(str) file=fopen(str, "w");}
    ~OutFile(){if(file) fclose(file);}

    void Out(const char* s)
    {
        fprintf(file, "%s\n", s); fflush(file);
    }
};

////////////////////////////////////////////////////////////////////////////////////
// Compiler Parameters /////////////////////////////////////////////////////////////

struct CompilerInfo
{
    InFile in_file;
    OutFile out_file;
    OutFile debug_file;

    CompilerInfo(const char* in_str, const char* out_str, const char* debug_str)
            : in_file(in_str), out_file(out_str), debug_file(debug_str)
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////
// Scanner /////////////////////////////////////////////////////////////////////////

#define MAX_TOKEN_LEN 40

enum TokenType{
    IF, THEN, ELSE, END, REPEAT, UNTIL, READ, WRITE,
    ASSIGN, EQUAL, LESS_THAN,
    PLUS, MINUS, TIMES, DIVIDE, POWER,
    SEMI_COLON,
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    ID,
    ENDFILE, ERROR,
    INT_VAR, REAL_VAR, BOOLEAN_VAR, // added new variables Tokens
    REAL_NUM, INT_NUM, BOOLEAN_NUM, // added new numbers Tokens
    TRUE, FALSE, // added new Boolean Tokens
};

// Used for debugging only /////////////////////////////////////////////////////////
const char* TokenTypeStr[]=
        {
                "If", "Then", "Else", "End", "Repeat", "Until", "Read", "Write",
                "Assign", "Equal", "LessThan",
                "Plus", "Minus", "Times", "Divide", "Power",
                "SemiColon",
                "LeftParen", "RightParen",
                "LeftBrace", "RightBrace",
                "ID",
                "EndFile", "Error",
                "Int Var", "Real Var", "Bool Var", // added new Var types Token string
                "Real Num", "Int Num", "Bool Num", // added new numbers Token String
                "true", "false", // added new Boolean vals Token string

        };

struct Token
{
    TokenType type;
    char str[MAX_TOKEN_LEN+1];


    Token(){str[0]=0; type=ERROR;}
    Token(TokenType _type, const char* _str) {type=_type; Copy(str, _str);}
};

const Token reserved_words[]=
        {
                Token(IF, "if"),
                Token(THEN, "then"),
                Token(ELSE, "else"),
                Token(END, "end"),
                Token(REPEAT, "repeat"),
                Token(UNTIL, "until"),
                Token(READ, "read"),
                Token(WRITE, "write"),
                Token(INT_VAR, "int"), // INT_VAR reserved word
                Token(REAL_VAR, "real"), // REAL_VAR reserved word
                Token(BOOLEAN_VAR, "bool"), // BOOLEAN_VAR reserved word
                Token(TRUE, "true"), // TRUE Value reserved word
                Token(FALSE, "false"), // FALSE value reserved word
        };
const int num_reserved_words=sizeof(reserved_words)/sizeof(reserved_words[0]);

// if there is tokens like < <=, sort them such that sub-tokens come last: <= <
// the closing comment should come immediately after opening comment
const Token symbolic_tokens[]=
        {
                Token(ASSIGN, ":="),
                Token(EQUAL, "="),
                Token(LESS_THAN, "<"),
                Token(PLUS, "+"),
                Token(MINUS, "-"),
                Token(TIMES, "*"),
                Token(DIVIDE, "/"),
                Token(POWER, "^"),
                Token(SEMI_COLON, ";"),
                Token(LEFT_PAREN, "("),
                Token(RIGHT_PAREN, ")"),
                Token(LEFT_BRACE, "{"),
                Token(RIGHT_BRACE, "}")
        };
const int num_symbolic_tokens=sizeof(symbolic_tokens)/sizeof(symbolic_tokens[0]);

inline bool IsDigit(char ch){return (ch>='0' && ch<='9' );}
// function to return true if char is dot
inline bool IsDot(char ch, bool flag)
{

    return (ch=='.'); // return true if char is dot
}
inline bool IsLetter(char ch){return ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z'));}
inline bool IsLetterOrUnderscore(char ch){return (IsLetter(ch) || ch=='_');}

void GetNextToken(CompilerInfo* pci, Token* ptoken)
{
    ptoken->type=ERROR;
    ptoken->str[0]=0;

    int i;
    char* s=pci->in_file.GetNextTokenStr();
    if(!s)
    {
        ptoken->type=ENDFILE;
        ptoken->str[0]=0;
        return;
    }

    for(i=0;i<num_symbolic_tokens;i++)
    {
        if(StartsWith(s, symbolic_tokens[i].str))
            break;
    }

    if(i<num_symbolic_tokens)
    {
        if(symbolic_tokens[i].type==LEFT_BRACE)
        {
            pci->in_file.Advance(strlen(symbolic_tokens[i].str));
            if(!pci->in_file.SkipUpto(symbolic_tokens[i+1].str)) return;
            return GetNextToken(pci, ptoken);
        }
        ptoken->type=symbolic_tokens[i].type;
        Copy(ptoken->str, symbolic_tokens[i].str);
    }
    else if(IsDigit(s[0]))
    {
        // Code Added
        int j=1;
        bool doubleNum = false; // bool var to check if there is dot found
        while(IsDigit(s[j]) || IsDot(s[j], doubleNum)) // check if the s[j] is number or dot
        {

            if (IsDot(s[j], doubleNum)) doubleNum = true; // make the bool var equal true if there is dot found
            j++;
        }

        //ptoken->type=NUM;
        // Code Added
        if (doubleNum) ptoken->type = REAL_NUM; // if bool var equal true then this is a real number
        else ptoken->type = INT_NUM; // else then this is a integer number

        Copy(ptoken->str, s, j);
    }

    else if(IsLetterOrUnderscore(s[0]))
    {
        int j=1;
        while(IsLetterOrUnderscore(s[j])) j++;

        ptoken->type=ID;
        Copy(ptoken->str, s, j);

        for(i=0;i<num_reserved_words;i++)
        {
            if(Equals(ptoken->str, reserved_words[i].str))
            {
                ptoken->type=reserved_words[i].type;
                break;
            }
        }
    }

    int len=strlen(ptoken->str);
    if(len>0) pci->in_file.Advance(len);
}

////////////////////////////////////////////////////////////////////////////////////
// Parser //////////////////////////////////////////////////////////////////////////

// program -> stmtseq
// stmtseq -> stmt { ; stmt }
// stmt -> ifstmt | repeatstmt | assignstmt | readstmt | writestmt
// ifstmt -> if exp then stmtseq [ else stmtseq ] end
// repeatstmt -> repeat stmtseq until expr
// assignstmt -> identifier := expr
// readstmt -> read identifier
// writestmt -> write expr
// expr -> mathexpr [ (<|=) mathexpr ]
// mathexpr -> term { (+|-) term }    left associative
// term -> factor { (*|/) factor }    left associative
// factor -> newexpr { ^ newexpr }    right associative
// newexpr -> ( mathexpr ) | number | identifier

enum NodeKind{
    IF_NODE, REPEAT_NODE, ASSIGN_NODE, READ_NODE, WRITE_NODE,
    OPER_NODE, INT_NUM_NODE, REAL_NUM_NODE, BOOLEAN_NUM_NODE, ID_NODE, REAL_VAR_NODE, INT_VAR_NODE,BOOLEAN_VAR_NODE
};

// Used for debugging only /////////////////////////////////////////////////////////
const char* NodeKindStr[]=
        {
                "If", "Repeat", "Assign", "Read", "Write",
                "Oper", "Int_Num_Node", "Real_Num_Node", "Bool_Num_Node", "ID", "Real_Var", "Int_Var","Bool_var"
        };

enum ExprDataType {VOID, INTEGER, BOOLEAN, REAL};

// Used for debugging only /////////////////////////////////////////////////////////
const char* ExprDataTypeStr[]=
        {
                "Void", "Integer", "Boolean", "Real",
        };

#define MAX_CHILDREN 3

struct TreeNode
{
    TreeNode* child[MAX_CHILDREN];
    TreeNode* sibling; // used for sibling statements only

    NodeKind node_kind;

    union{TokenType oper; int inum; double dnum; bool flag; char* id;}; // defined for expression/int/identifier only
    ExprDataType expr_data_type; // defined for expression/int/identifier only

    int line_num;

    TreeNode() {int i; for(i=0;i<MAX_CHILDREN;i++) child[i]=0; sibling=0; expr_data_type=VOID;}
};

struct ParseInfo
{
    Token next_token;
};

void Match(CompilerInfo* pci, ParseInfo* ppi, TokenType expected_token_type)
{
    pci->debug_file.Out("Start Match");

    if(ppi->next_token.type!=expected_token_type) throw 0;
    GetNextToken(pci, &ppi->next_token);

    fprintf(pci->debug_file.file, "[%d] %s (%s)\n", pci->in_file.cur_line_num, ppi->next_token.str, TokenTypeStr[ppi->next_token.type]); fflush(pci->debug_file.file);
}


TreeNode* Expr(CompilerInfo*, ParseInfo*);

// newexpr -> ( mathexpr ) | number | identifier
TreeNode* NewExpr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start NewExpr");

    // Code Added
    // Compare the next token with the First() of possible statements
    if(ppi->next_token.type==REAL_NUM) // if real number then go in
    {
        TreeNode* tree=new TreeNode; // create new node
        tree->node_kind=REAL_NUM_NODE; // node kind equal a number
        tree->expr_data_type = REAL; // node expr type equal real
        char* num_str=ppi->next_token.str; // fetch the number
        //tree->num=0; //while(*num_str) tree->num=tree->num*10+((*num_str++)-'0');

        // Code Added
        tree->dnum=0; // init the double num with zero
        double tmp = atof(num_str); // use atof number to convert the num_str to double number
        tree->dnum = tmp; // make dnum equal to the double number


        tree->line_num=pci->in_file.cur_line_num;
        Match(pci, ppi, ppi->next_token.type);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    // Code Added
    // Compare the next token with the First() of possible statements
    if(ppi->next_token.type==INT_NUM)
    {
        TreeNode* tree=new TreeNode; // create new node
        tree->node_kind=INT_NUM_NODE; // node kind equal a number
        tree->expr_data_type=INTEGER; // node expr type equal int
        char* num_str=ppi->next_token.str; // fetch the number
        //tree->num=0; //while(*num_str) tree->num=tree->num*10+((*num_str++)-'0');

        // Code Added
        tree->inum=0; // init the int num with zero
        while(*num_str) tree->inum=tree->inum*10+((*num_str++)-'0'); // convert the num_str to int number
        //tree->expr_data_type = INTEGER;

        tree->line_num=pci->in_file.cur_line_num;
        Match(pci, ppi, ppi->next_token.type);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    // Code Added
    // Compare the next token with the First() of possible statements
    if (ppi->next_token.type == TRUE || ppi->next_token.type == FALSE) // if the value is true or false
    {

        TreeNode* tree=new TreeNode; // create new node
        tree->node_kind=BOOLEAN_NUM_NODE; // node kind equal a boolean number
        tree->expr_data_type=BOOLEAN; // node expr type equal boolean
        char* num_str=ppi->next_token.str; // fetch the number
        //tree->num=0; //while(*num_str) tree->num=tree->num*10+((*num_str++)-'0');

        // Code Added
        tree->flag = true; // init flag with true value
        if (ppi->next_token.type == TRUE) tree->flag = true; // if type of token is true then flag equal true
        else if  (ppi->next_token.type == FALSE) tree->flag = false; // else flag equal false

        tree->line_num=pci->in_file.cur_line_num;
        Match(pci, ppi, ppi->next_token.type);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }


    if(ppi->next_token.type==ID)
    {
        TreeNode* tree=new TreeNode;
        tree->node_kind=ID_NODE;
        AllocateAndCopy(&tree->id, ppi->next_token.str);
        tree->line_num=pci->in_file.cur_line_num;
        Match(pci, ppi, ppi->next_token.type);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    if(ppi->next_token.type==LEFT_PAREN)
    {
        Match(pci, ppi, LEFT_PAREN);
        TreeNode* tree=Expr(pci, ppi);
        Match(pci, ppi, RIGHT_PAREN);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    throw 0;
    return 0;
}

// factor -> newexpr { ^ newexpr }    right associative
TreeNode* Factor(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Factor");

    TreeNode* tree=NewExpr(pci, ppi);

    if(ppi->next_token.type==POWER)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Factor(pci, ppi);

        pci->debug_file.Out("End Factor");
        return new_tree;
    }
    pci->debug_file.Out("End Factor");
    return tree;
}

// term -> factor { (*|/) factor }    left associative
TreeNode* Term(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Term");

    TreeNode* tree=Factor(pci, ppi);

    while(ppi->next_token.type==TIMES || ppi->next_token.type==DIVIDE)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Factor(pci, ppi);

        tree=new_tree;
    }
    pci->debug_file.Out("End Term");
    return tree;
}

// mathexpr -> term { (+|-) term }    left associative
TreeNode* MathExpr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start MathExpr");

    TreeNode* tree=Term(pci, ppi);

    while(ppi->next_token.type==PLUS || ppi->next_token.type==MINUS)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Term(pci, ppi);

        tree=new_tree;
    }
    pci->debug_file.Out("End MathExpr");
    return tree;
}

// expr -> mathexpr [ (<|=) mathexpr ]
TreeNode* Expr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Expr");

    TreeNode* tree=MathExpr(pci, ppi);

    if(ppi->next_token.type==EQUAL || ppi->next_token.type==LESS_THAN)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=MathExpr(pci, ppi);

        pci->debug_file.Out("End Expr");
        return new_tree;
    }
    pci->debug_file.Out("End Expr");
    return tree;
}

// writestmt -> write expr
TreeNode* WriteStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start WriteStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=WRITE_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, WRITE);
    if(ppi->next_token.type==ID) AllocateAndCopy(&tree->id, ppi->next_token.str);
    tree->child[0]=Expr(pci, ppi);

    pci->debug_file.Out("End WriteStmt");
    return tree;
}

// readstmt -> read identifier
TreeNode* ReadStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start ReadStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=READ_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, READ);
    if(ppi->next_token.type==ID) AllocateAndCopy(&tree->id, ppi->next_token.str);
    tree->child[0]=Expr(pci, ppi);
    //Match(pci, ppi, ID);

    pci->debug_file.Out("End ReadStmt");
    return tree;
}

// assignstmt -> identifier := expr
TreeNode* AssignStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start AssignStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=ASSIGN_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    if(ppi->next_token.type==ID) AllocateAndCopy(&tree->id, ppi->next_token.str);
    Match(pci, ppi, ID);
    Match(pci, ppi, ASSIGN); tree->child[0]=Expr(pci, ppi);


    pci->debug_file.Out("End AssignStmt");
    return tree;
}



TreeNode* StmtSeq(CompilerInfo*, ParseInfo*);

// repeatstmt -> repeat stmtseq until expr
TreeNode* RepeatStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start RepeatStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=REPEAT_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, REPEAT); tree->child[0]=StmtSeq(pci, ppi);
    Match(pci, ppi, UNTIL); tree->child[1]=Expr(pci, ppi);

    pci->debug_file.Out("End RepeatStmt");
    return tree;
}

// ifstmt -> if exp then stmtseq [ else stmtseq ] end
TreeNode* IfStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start IfStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=IF_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, IF); tree->child[0]=Expr(pci, ppi);
    Match(pci, ppi, THEN); tree->child[1]=StmtSeq(pci, ppi);
    if(ppi->next_token.type==ELSE) {Match(pci, ppi, ELSE); tree->child[2]=StmtSeq(pci, ppi);}
    Match(pci, ppi, END);

    pci->debug_file.Out("End IfStmt");
    return tree;
}


// Code Added
// function to define int var type
TreeNode* IntVarFound(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start IntVarFound");

    TreeNode* tree = new TreeNode(); // create node
    tree->node_kind = INT_VAR_NODE; // node type is int variable
    tree->line_num=pci->in_file.cur_line_num; // store line number

    Match(pci, ppi, INT_VAR); // escape int var keyword

    if (ppi->next_token.type == ID) AllocateAndCopy(&tree->id, ppi->next_token.str); // if there is a name to the variable then set it in id
    Match(pci, ppi, ID); // escape the var name

    if (ppi->next_token.type == ASSIGN) // if there is an assign
    {
        TreeNode* assignNode = new TreeNode(); // create new node
        assignNode->node_kind=ASSIGN_NODE;     // node type is assign
        assignNode->expr_data_type=INTEGER;    // varable type is integer
        assignNode->line_num=pci->in_file.cur_line_num; // line of the assign
        AllocateAndCopy(&assignNode->id, tree->id); // the string of the variable
        Match(pci, ppi, ASSIGN); // escape assign keyword
        assignNode->child[0]=Expr(pci, ppi); // assign child is the number
        tree->child[0]=assignNode; // variable child is the whole assign
    }
    pci->debug_file.Out("End IntVarFound");
    return tree;
}


// Code Added
TreeNode* RealVarFound(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start RealVarFound");

    TreeNode* tree = new TreeNode(); // create node
    tree->node_kind = REAL_VAR_NODE; // node type is double variable
    tree->line_num=pci->in_file.cur_line_num; // store line number

    Match(pci, ppi, REAL_VAR); // escape double var keyword

    if (ppi->next_token.type == ID) AllocateAndCopy(&tree->id, ppi->next_token.str); // if there is a name to the variable then set it in id
    Match(pci, ppi, ID); // escape the var name

    if (ppi->next_token.type == ASSIGN) // if there is an assign
    {
        TreeNode* assignNode = new TreeNode(); // create new node
        assignNode->node_kind=ASSIGN_NODE; // node type is assign
        assignNode->expr_data_type=REAL; // varable type is double
        assignNode->line_num=pci->in_file.cur_line_num; // line of the assign
        AllocateAndCopy(&assignNode->id, tree->id); // the string of the variable
        Match(pci, ppi, ASSIGN); // escape assign keyword
        assignNode->child[0]=Expr(pci, ppi); // assign child is the number
        tree->child[0]=assignNode; // variable child is the whole assign
    }
    pci->debug_file.Out("End RealVarFound");
    return tree;
}

// Code Added
TreeNode* BoolVarFound(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start BooleanVarFound");

    TreeNode* tree = new TreeNode(); // create node
    tree->node_kind = BOOLEAN_VAR_NODE; // node type is int variable
    tree->line_num=pci->in_file.cur_line_num; // store line number

    Match(pci, ppi, BOOLEAN_VAR); // escape Boolean var keyword

    if (ppi->next_token.type == ID) AllocateAndCopy(&tree->id, ppi->next_token.str); // if there is a name to the variable then set it in id
    Match(pci, ppi, ID); // escape the var name

    if (ppi->next_token.type == ASSIGN)
    {
        TreeNode* assignNode = new TreeNode(); // create new node
        assignNode->node_kind=ASSIGN_NODE; // node type is assign
        assignNode->expr_data_type=BOOLEAN; // varable type is Boolean
        assignNode->line_num=pci->in_file.cur_line_num; // line of the assign
        AllocateAndCopy(&assignNode->id, tree->id); // the string of the variable
        Match(pci, ppi, ASSIGN); // escape assign keyword
        assignNode->child[0]=Expr(pci, ppi); // assign child is the number
        tree->child[0]=assignNode; // variable child is the whole assign
    }
    pci->debug_file.Out("End BooleanVarFound");
    return tree;
}

// stmt -> ifstmt | repeatstmt | assignstmt | readstmt | writestmt
TreeNode* Stmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Stmt");

    // Compare the next token with the First() of possible statements
    TreeNode* tree=0;
    if(ppi->next_token.type==IF) tree=IfStmt(pci, ppi);
    else if(ppi->next_token.type==REPEAT) tree=RepeatStmt(pci, ppi);
    else if(ppi->next_token.type==ID) tree=AssignStmt(pci, ppi);
    else if(ppi->next_token.type==READ) tree=ReadStmt(pci, ppi);
    else if(ppi->next_token.type==WRITE) tree=WriteStmt(pci, ppi);
    // Code Added
    else if (ppi->next_token.type==INT_VAR) tree= IntVarFound(pci, ppi); // if the token type is int variable
    else if (ppi->next_token.type==REAL_VAR) tree= RealVarFound(pci, ppi); // if the token type is real variable
    else if (ppi->next_token.type==BOOLEAN_VAR) tree= BoolVarFound(pci, ppi); // if the token type is boolean variable
    else throw 0;

    pci->debug_file.Out("End Stmt");
    return tree;
}

// stmtseq -> stmt { ; stmt }
TreeNode* StmtSeq(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start StmtSeq");

    TreeNode* first_tree=Stmt(pci, ppi);
    TreeNode* last_tree=first_tree;

    // If we did not reach one of the Follow() of StmtSeq(), we are not done yet
    while(ppi->next_token.type!=ENDFILE && ppi->next_token.type!=END &&
          ppi->next_token.type!=ELSE && ppi->next_token.type!=UNTIL)
    {
        Match(pci, ppi, SEMI_COLON);
        TreeNode* next_tree=Stmt(pci, ppi);
        last_tree->sibling=next_tree;
        last_tree=next_tree;
    }

    pci->debug_file.Out("End StmtSeq");
    return first_tree;
}

// program -> stmtseq
TreeNode* Parse(CompilerInfo* pci)
{
    ParseInfo parse_info;
    GetNextToken(pci, &parse_info.next_token);

    TreeNode* syntax_tree=StmtSeq(pci, &parse_info);

    if(parse_info.next_token.type!=ENDFILE)
        pci->debug_file.Out("Error code ends before file ends");

    return syntax_tree;
}

void PrintTree(TreeNode* node, int sh=0)
{
    int i, NSH=3;
    for(i=0;i<sh;i++) printf(" ");

    printf("[%s]", NodeKindStr[node->node_kind]);

    //code Added
    if(node->node_kind==REAL_VAR_NODE) // if node kind is double variable
    {
        printf("[%s]", node->id); // print the variable name
    }
    else if(node->node_kind==INT_VAR_NODE) // if node kind is int variable
    {
        printf("[%s]", node->id); // print the variable name
    }

    else if(node->node_kind==OPER_NODE) printf("[%s]", TokenTypeStr[node->oper]); // if node kind is operation node then print operation type
    // Code Added
    else if(node->node_kind==REAL_NUM_NODE) // if the node kind is double number
    {
        printf("[%f]", node->dnum); // print the value of the double variable
        //if(node->node_kind == REAL_NUM) printf("[%f]", node->dnum);
        //else if(node->node_kind == INT_NUM) printf("[%d]", node->inum);
    }
    else if(node->node_kind==INT_NUM_NODE) // if the node kind is int number
    {
        printf("[%d]", node->inum); // print the value of the int variable
        //if(node->node_kind == REAL_NUM) printf("[%f]", node->dnum);
        //else if(node->node_kind == INT_NUM) printf("[%d]", node->inum);
    }


    else if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE) printf("[%s]", node->id);

    if(node->expr_data_type!=VOID) printf("[%s]", ExprDataTypeStr[node->expr_data_type]);

    printf("\n");

    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) PrintTree(node->child[i], sh+NSH);
    if(node->sibling) PrintTree(node->sibling, sh);
}

void DestroyTree(TreeNode* node)
{
    int i;

    if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE)
        if(node->id) delete[] node->id;

    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) DestroyTree(node->child[i]);
    if(node->sibling) DestroyTree(node->sibling);

    delete node;
}

////////////////////////////////////////////////////////////////////////////////////
// Analyzer ////////////////////////////////////////////////////////////////////////

const int SYMBOL_HASH_SIZE=10007;

struct LineLocation
{
    int line_num;
    LineLocation* next;
};

struct VariableInfo
{
    char* name;
    int memloc;
    // Code Added
    ExprDataType exprDataType; // to define type of variable

    LineLocation* head_line; // the head of linked list of source line locations
    LineLocation* tail_line; // the tail of linked list of source line locations
    VariableInfo* next_var; // the next variable in the linked list in the same hash bucket of the symbol table
};

struct SymbolTable
{
    int num_vars;
    VariableInfo* var_info[SYMBOL_HASH_SIZE];

    SymbolTable() {num_vars=0; int i; for(i=0;i<SYMBOL_HASH_SIZE;i++) var_info[i]=0;}

    int Hash(const char* name)
    {
        int i, len=strlen(name);
        int hash_val=11;
        for(i=0;i<len;i++) hash_val=(hash_val*17+(int)name[i])%SYMBOL_HASH_SIZE;
        return hash_val;
    }

    VariableInfo* Find(const char* name)
    {
        int h=Hash(name);
        VariableInfo* cur=var_info[h];
        while(cur)
        {
            if(Equals(name, cur->name)) return cur;
            cur=cur->next_var;
        }
        return 0;
    }

    void Insert(const char* name, int line_num, ExprDataType exprDataType)
    {
        LineLocation* lineloc=new LineLocation;
        lineloc->line_num=line_num;
        lineloc->next=0;

        int h=Hash(name);
        VariableInfo* prev=0;
        VariableInfo* cur=var_info[h];

        while(cur)
        {
            if(Equals(name, cur->name))
            {
                // just add this line location to the list of line locations of the existing var
                cur->tail_line->next=lineloc;
                cur->tail_line=lineloc;
                return;
            }
            prev=cur;
            cur=cur->next_var;
        }

        VariableInfo* vi=new VariableInfo;
        vi->head_line=vi->tail_line=lineloc;
        vi->next_var=0;
        vi->memloc=num_vars++;
        vi->exprDataType = exprDataType;
        AllocateAndCopy(&vi->name, name);

        if(!prev) var_info[h]=vi;
        else prev->next_var=vi;
    }

    void Print()
    {
        int i;
        for(i=0;i<SYMBOL_HASH_SIZE;i++)
        {
            VariableInfo* curv=var_info[i];
            while(curv)
            {
                printf("[Var=%s][Mem=%d][Type=%s]", curv->name, curv->memloc, ExprDataTypeStr[curv->exprDataType]);
                LineLocation* curl=curv->head_line;
                while(curl)
                {
                    printf("[Line=%d]", curl->line_num);
                    curl=curl->next;
                }
                printf("\n");
                curv=curv->next_var;
            }
        }
    }

    void Destroy()
    {
        int i;
        for(i=0;i<SYMBOL_HASH_SIZE;i++)
        {
            VariableInfo* curv=var_info[i];
            while(curv)
            {
                LineLocation* curl=curv->head_line;
                while(curl)
                {
                    LineLocation* pl=curl;
                    curl=curl->next;
                    delete pl;
                }
                VariableInfo* p=curv;
                curv=curv->next_var;
                delete p;
            }
            var_info[i]=0;
        }
    }
};

void Analyze(TreeNode* node, SymbolTable* symbol_table)
{
    int i;

//    if(/*node->node_kind==ID_NODE || */node->node_kind==READ_NODE /*|| node->node_kind==ASSIGN_NODE*/)
//    {
//        //cout << ExprDataTypeStr[node->expr_data_type] << " " << NodeKindStr[node->node_kind] << " " << node->line_num << endl;
//        symbol_table->Insert(node->id, node->line_num, REAL);
//    }

    // Code Added
    if (node->node_kind==INT_VAR_NODE) // if the node kind is int variable
    {

        // search in SymbolTable
        VariableInfo* tableNodeInfo = symbol_table->Find(node->id); // find the variable in symbol table
        if (tableNodeInfo) // if the variable exist
        {
            throw runtime_error("duplicate Variable declaration"); // print duplicate error
        }
        else
        {
            symbol_table->Insert(node->id, node->line_num, INTEGER); // insert the variable in the symbol table
        }
    }

    // Code Added
    if (node->node_kind==BOOLEAN_VAR_NODE) // if the node kind is boolean var
    {

        // search in SymbolTable
        VariableInfo* tableNodeInfo = symbol_table->Find(node->id); // find the variable in symbol table
        if (tableNodeInfo) // if the var exist
        {
            throw runtime_error("duplicate Variable declaration"); // print duplicate error
        }
        else
        {
            symbol_table->Insert(node->id, node->line_num, BOOLEAN); // insert the variable in the symbol table
        }
    }

    // Code Added
    if (node->node_kind==REAL_VAR_NODE) // if the node kind is double node
    {
        // search in SymbolTable
        VariableInfo* tableNodeInfo = symbol_table->Find(node->id); // find the var in symbol table
        if (tableNodeInfo) // if the var exist
        {
            throw runtime_error("duplicate Variable declaration"); // print duplicate error
        }
        else
        {
            symbol_table->Insert(node->id, node->line_num, REAL); // insert the var in the symbol table
        }
    }




    // Code Added
    if (node->node_kind==ASSIGN_NODE  ) // if node kind is assign
    {
        // search in SymbolTable
        VariableInfo* tableNodeInfo = symbol_table->Find(node->id);


        if(node->child[0])
        {
            //cout << node->id ;
            if (node->child[0]->node_kind==ID_NODE) // if node kind of the child is id
            {
                VariableInfo* childTable = symbol_table->Find(node->child[0]->id); // find the var in symbol table
                //cout << " " << ExprDataTypeStr[childTable->exprDataType] << endl;
                // if assigned value is not equal to var data type then throw error
                if (!childTable) throw runtime_error("Assign to undefined variable"); // raise error if the assigned variable is undefined
                // raise error if the assigned variable is different type
                if (ExprDataTypeStr[tableNodeInfo->exprDataType] != ExprDataTypeStr[childTable->exprDataType]) throw runtime_error("Can't Assign var to different type");
            }
        }

        if (tableNodeInfo) // if var found in symbol table
        {
            //cout << ExprDataTypeStr[tableNodeInfo->exprDataType] << endl;
            symbol_table->Insert(node->id, node->line_num, tableNodeInfo->exprDataType); // insert var in symbol table
            node->expr_data_type = tableNodeInfo->exprDataType; // and set the type of the var
        }
        else
        {
            throw runtime_error("undefined Variable"); // else throw undefined var
        }
    }



    // Code Added
    if (node->node_kind==ID_NODE) // if node kind is id
    {
        VariableInfo* tableNodeInfo = symbol_table->Find(node->id); // find if var in symbol table
        if(tableNodeInfo) // if var found
        {
            node->expr_data_type = tableNodeInfo->exprDataType; // set type of var to type from symbol table
        }
    }

    // Code Added
    if (node->node_kind==READ_NODE) // if node kind is read
    {
        VariableInfo* tableNodeInfo = symbol_table->Find(node->id); // find if var in symbol table
        if(tableNodeInfo) // if var found
        {
            node->expr_data_type = tableNodeInfo->exprDataType; // set type of var to type from symbol table
        }
        else
        {
            throw runtime_error("undefined Variable"); // else throw undefined error
        }
    }

    // Code Added
    if (node->node_kind==WRITE_NODE) // if node kind is read
    {
        if (node->child[0]->node_kind == ID_NODE)
        {
            VariableInfo* tableNodeInfo = symbol_table->Find(node->id); // find if var in symbol table
            if(!tableNodeInfo) // if var found
            {
                throw runtime_error("undefined Variable"); // else throw undefined error

            }
        }
    }

    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) Analyze(node->child[i], symbol_table);

    // code Added


    if(node->node_kind==OPER_NODE) // if node kind is operation
    {
        if(node->oper==EQUAL || node->oper==LESS_THAN) node->expr_data_type=BOOLEAN; // if oper kind is equal or less than this this is a boolean value
        //else node->expr_data_type=INTEGER;
    }
    //else if(node->node_kind==ID_NODE /*|| node->node_kind==NUM_NODE*/) node->expr_data_type=INTEGER;

    // Code Added
    if(node->node_kind==OPER_NODE) // if node kind is operation
    {
        if (node->child[0]->expr_data_type == BOOLEAN ||node->child[1]->expr_data_type == BOOLEAN) // if first child data type is bool or second child data type is bool
        {
            // if the operation is (+|-|*|/|^) then throw error
            if(node->oper==PLUS || node->oper==MINUS || node->oper==DIVIDE || node->oper==TIMES || node->oper==POWER) throw runtime_error("ERROR Operator applied to non-integers");
        }
//        if(node->child[0]->expr_data_type != node->child[1]->expr_data_type)
//        {
//            throw runtime_error("ERROR Operator applied to non-integers");
//        }


        //cout <<  "OPR = " << " " << ExprDataTypeStr[node->expr_data_type] << endl;
        // if the operation is (+|-|*|/|^)
        if(node->oper==PLUS || node->oper==MINUS || node->oper==DIVIDE || node->oper==TIMES)
        {
            //cout << ExprDataTypeStr[node->child[0]->expr_data_type] << " : " << ExprDataTypeStr[node->child[1]->expr_data_type] << endl;
            // if first child data type not equal to second child throw error
            if(node->child[0]->expr_data_type != node->child[1]->expr_data_type) throw runtime_error("Arithmetic Operation between different Types");
            // if first child data type equal double or second child data type equal double then the data type of this node is real
            if(node->child[0]->expr_data_type == REAL || node->child[1]->expr_data_type==REAL) node->expr_data_type=REAL;
            // else the data type of this node is int
            else node->expr_data_type=INTEGER;
        }
        // if operation type is power
        if(node->oper==POWER)
        {
            // if first child data type equal to int and second child data type equal to double throw error
            if(node->child[0]->expr_data_type==INTEGER && node->child[1]->expr_data_type==REAL) throw runtime_error("ERROR Power raised to non integer");
            // if first child data type equal to double and second child data type equal to int then the data type of the node is double
            if(node->child[0]->expr_data_type==REAL && node->child[1]->expr_data_type==INTEGER) node->expr_data_type=REAL;
        }
    }

    /*if(node->node_kind==OPER_NODE)
    {
        if(node->child[0]->expr_data_type!=INTEGER || node->child[1]->expr_data_type!=INTEGER)
            printf("ERROR Operator applied to non-integers\n");
    }*/
    /*if(node->node_kind==IF_NODE && node->child[0]->expr_data_type!=BOOLEAN) printf("ERROR If test must be BOOLEAN\n");
    if(node->node_kind==REPEAT_NODE && node->child[1]->expr_data_type!=BOOLEAN) printf("ERROR Repeat test must be BOOLEAN\n");
    if(node->node_kind==WRITE_NODE && node->child[0]->expr_data_type!=INTEGER) printf("ERROR Write works only for INTEGER\n");
    if(node->node_kind==ASSIGN_NODE && node->child[0]->expr_data_type!=INTEGER) printf("ERROR Assign works only for INTEGER\n");
*/
    if(node->sibling) Analyze(node->sibling, symbol_table);
}

////////////////////////////////////////////////////////////////////////////////////
// Code Generator //////////////////////////////////////////////////////////////////

template <typename T>
T Power(T a, int b)
{
    if(a==0) return 0;
    if(b==0) return 1;
    if(b>=1) return a*Power(a, b-1);
    return 0;
}

template <typename T>
T Evaluate(TreeNode* node, SymbolTable* symbol_table, double* variables)
{
    // code Added
    if(node->node_kind==INT_NUM_NODE) return node->inum; // return the int value if the node kind is int num
    if(node->node_kind==REAL_NUM_NODE) return node->dnum;// return the double value if the node kind is double num
    if(node->node_kind==BOOLEAN_NUM_NODE) return node->flag; // return the bool value if the node kind is bool num
    if(node->node_kind==ID_NODE)
    {
        //cout << variables[symbol_table->Find(node->id)->memloc] << endl;
        return variables[symbol_table->Find(node->id)->memloc];
    }


    // Code Added
    T a=Evaluate<T>(node->child[0], symbol_table, variables); // convert it to Template to return the wished type
    T b=Evaluate<T>(node->child[1], symbol_table, variables); // convert it to Template to return the wished type

//    cout << a << " " << b << endl;
//    cout << node->child[0]->inum << " " << node->child[0]->dnum << " " << ExprDataTypeStr[node->child[0]->expr_data_type] << " " << NodeKindStr[node->child[0]->node_kind] << endl;
//    cout << node->child[1]->inum << " " << node->child[1]->dnum << " " << ExprDataTypeStr[node->child[1]->expr_data_type] << " " << NodeKindStr[node->child[1]->node_kind] << endl;

    if(node->oper==EQUAL) return a==b;
    if(node->oper==LESS_THAN) return a<b;
    if(node->oper==PLUS) return a+b;
    if(node->oper==MINUS) return a-b;
    if(node->oper==TIMES) return a*b;
    if(node->oper==DIVIDE) return a/b;
    if(node->oper==POWER) return Power(a,b);
    throw 0;
    return 0;
}

void RunProgram(TreeNode* node, SymbolTable* symbol_table, double* variables)
{
    if(node->node_kind==IF_NODE)
    {
        bool cond=Evaluate<bool>(node->child[0], symbol_table, variables);
        if(cond) RunProgram(node->child[1], symbol_table, variables);
        else if(node->child[2]) RunProgram(node->child[2], symbol_table, variables);
    }
    // Code Added
    if (node->node_kind==INT_VAR_NODE) // if node kind is int var
    {
        // if there is first child run the function with this child
        if (node->child[0]) RunProgram(node->child[0], symbol_table, variables);
    }
    // Code Added
    if (node->node_kind==REAL_VAR_NODE) // if node kind is double var
    {
        // if there is first child run the function with this child
        if (node->child[0]) RunProgram(node->child[0], symbol_table, variables);
    }
    // Code Added
    if (node->node_kind==BOOLEAN_VAR_NODE) // if node kind is bool var
    {
        // if there is first child run the function with this child
        if (node->child[0]) RunProgram(node->child[0], symbol_table, variables);
    }

    // Code Added
    if(node->node_kind==ASSIGN_NODE) // if node kind is assign
    {
        // Code Added
        VariableInfo* info = symbol_table->Find(node->id); // find variable in symbol table
        if (info->exprDataType==REAL) // if var is double
        {
            double v=Evaluate<double>(node->child[0], symbol_table, variables); // get var value
            variables[symbol_table->Find(node->id)->memloc]=v; // insert it in variable array
        }
        else if (info->exprDataType==INTEGER) // if var is int
        {
            int v=Evaluate<int>(node->child[0], symbol_table, variables); // get the value
            variables[symbol_table->Find(node->id)->memloc]=v; // insert it in variable array
        }
        else if (info->exprDataType==BOOLEAN) // if var is bool
        {
            bool v=Evaluate<bool>(node->child[0], symbol_table, variables); // get the value
            variables[symbol_table->Find(node->id)->memloc]=v; // insert it in variable array
        }

    }

    if(node->node_kind==READ_NODE)
    {
        printf("Enter %s: ", node->id);
        scanf("%lf", &variables[symbol_table->Find(node->id)->memloc]);
    }

    if(node->node_kind==WRITE_NODE)
    {

        // Code Added
        if(node->child[0]->expr_data_type==INTEGER) // if first child type is int
        {

            int v=Evaluate<int>(node->child[0], symbol_table, variables); // get value of var
            printf("Val : %d\n", v); // print the value
        }
        if(node->child[0]->expr_data_type==REAL) // if first child type is double
        {
            double v=Evaluate<double>(node->child[0], symbol_table, variables); // get value of var
            printf("Val : %f\n", v); // print the value
        }
        if(node->child[0]->expr_data_type==BOOLEAN) // if first child type is bool
        {
            bool v=Evaluate<bool>(node->child[0], symbol_table, variables); // get value of var
            printf("Val : %d\n", v); // print the value
        }


    }
    if(node->node_kind==REPEAT_NODE)
    {
        do
        {
            RunProgram(node->child[0], symbol_table, variables);
        }
        while(!Evaluate<int>(node->child[1], symbol_table, variables));
    }
    if(node->sibling) RunProgram(node->sibling, symbol_table, variables);
}

void RunProgram(TreeNode* syntax_tree, SymbolTable* symbol_table)
{
    int i;
    double* variables=new double[symbol_table->num_vars];
    for(i=0;i<symbol_table->num_vars;i++) variables[i]=0.0;
    RunProgram(syntax_tree, symbol_table, variables);
    delete[] variables;
}

////////////////////////////////////////////////////////////////////////////////////
// Scanner and Compiler ////////////////////////////////////////////////////////////

void StartCompiler(CompilerInfo* pci)
{
    TreeNode* syntax_tree=Parse(pci);

    SymbolTable symbol_table;
    Analyze(syntax_tree, &symbol_table);

    printf("Symbol Table:\n");
    symbol_table.Print();
    printf("---------------------------------\n"); fflush(NULL);

    printf("Syntax Tree:\n");
    PrintTree(syntax_tree);
    printf("---------------------------------\n"); fflush(NULL);

    printf("Run Program:\n");
    RunProgram(syntax_tree, &symbol_table);
    printf("---------------------------------\n"); fflush(NULL);

    symbol_table.Destroy();
    DestroyTree(syntax_tree);
}

////////////////////////////////////////////////////////////////////////////////////
// Scanner only ////////////////////////////////////////////////////////////////////

void StartScanner(CompilerInfo* pci)
{
    Token token;

    while(true)
    {
        GetNextToken(pci, &token);
        printf("[%d] %s (%s)\n", pci->in_file.cur_line_num, token.str, TokenTypeStr[token.type]); fflush(NULL);
        if(token.type==ENDFILE || token.type==ERROR) break;
    }
}

////////////////////////////////////////////////////////////////////////////////////

int main()
{
    printf("Start main()\n"); fflush(NULL);

    CompilerInfo compiler_info("input.txt", "output.txt", "debug.txt");

    StartCompiler(&compiler_info);

    printf("End main()\n"); fflush(NULL);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
