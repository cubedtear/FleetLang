grammar FleetLang;

@parser::postinclude {
#include "../src/AST/Program.h"
#include "../src/AST/Function.h"
#include "../src/AST/ExprAST.h"
#include "../src/AST/StmtAST.h"
}

program
       : functions+=function*
       ;

function
        : (retType=type | voidTy=voidType) name=ID '(' (args+=type argNames+=ID (',' args+=type argNames+=ID)*)? ')' (('{' stmts+=statement* '}') | (';'))
        ;

type
    : name=ID                                #refType
    | elem=type dim+='[]'+                   #arrayType
    | name=primitiveType                     #primType
    | '(' types+=type (',' types+=type)* ')' #tupleType
    ;

primitiveType
             : INT     #intType
             | FLOAT   #floatType
             | BOOLEAN #boolType
             | CHAR    #charType
             ;

voidType : VOID
         ;

statement
         : varType=type? left=ID '=' right=expression stmtEnd                               #assignStmt
         | IF '(' cond=expression')' whenTrue=statement (ELSE whenFalse=statement)?         #ifStmt
         | WHILE '(' cond=expression ')' whenTrue=statement                                 #whileStmt
         | RETURN expr=expression stmtEnd                                                   #returnStmt
         | stmtEnd                                                                          #emptyStmt
         | '{' stmts+=statement* '}'                                                        #blockStmt
         | varType=type name=ID stmtEnd                                                     #declStmt
         | expr=expression stmtEnd                                                          #expressionStmt
         ;

expression
          : atom=INT_LIT                                                  #intExpr
          | atom=FLOAT_LIT                                                #floatExpr
          | atom=STRING_LIT                                               #stringExpr
          | atom=TRUE                                                     #literalBool
          | atom=FALSE                                                    #literalBool
          | atom=ID                                                       #idExpr
          | sign=('+'|'-') expr=expression                                #signedExpression
          | func=ID '(' (args+=expression (',' args+=expression)*)? ')'   #funcExpr
          | left=expression op=('*'|'/') right=expression                 #opExpr
          | left=expression op=('+'|'-') right=expression                 #opExpr
          | '(' expr=expression ')'                                       #parenExpr
          | left=expression op=('=='|'!=') right=expression               #comparisonOp
          | left=expression op=('<='|'>='|'>'|'<') right=expression       #comparisonOp
          | left=expression op='&&' right=expression                      #booleanOp
          | left=expression op='||' right=expression                      #booleanOp
          | '(' expr=expression ')'                                       #parenBool
          | exprs+=expression (',' exprs+=expression)+                    #tupleExpr
          ;

stmtEnd
        : ';'
        | '\n'
        ;

INT: 'int';
FLOAT: 'float';
BOOLEAN: 'bool';
CHAR: 'char';
VOID: 'void';

IF: 'if';
WHILE: 'while';
RETURN: 'return';
ELSE: 'else';

TRUE: 'true';
FALSE: 'false';

ADD: '+';
SUBTRACT: '-';
MULTIPLY: '*';
DIVIDE: '/';

LESS_EQUAL: '<=';
GREATER_EQUAL: '>=';
LESS_THAN: '<';
GREATER_THAN: '>';
EQUALS: '==';
NOT_EQUALS: '!=';

BOOL_OR: '||';
BOOL_AND: '&&';

LPAREN: '(';
RPAREN: ')';

ID:           [_]?[a-zA-Z][a-zA-Z_0-9]*;

FLOAT_LIT:    [0-9]*'.'[0-9]+([eE][+\-]?[0-9]+)? ;
INT_LIT:      [0-9]+;
STRING_LIT:   '"' StringCharacter* '"';


WS:           [ \t\r\n]+ -> skip ;

COMMENT:      '/*' .*? '*/'    -> channel(HIDDEN);
LINE_COMMENT: '//' ~[\r\n]*    -> channel(HIDDEN);

fragment StringCharacter
        : ~["\\]
        | '\\' [btnfr"'\\] // Escape sequence
        ;