/* ollieberzs 2018
** parse.cpp
** parsing oca tokens into AST
*/ 

#include <iostream>
#include "parse.hpp"
#include "lex.hpp"

OCA_BEGIN

Expression::Expression(const std::string& type, const std::string& val)
    : type(type), val(val) {}
// ----------------------------

void parse(ParseState& state)
{
    state.current = 0;
    state.errorToken = 0;
    while (state.current < state.ls->tokens.size() - 1)
    {
        ExprPtr e = nullptr;
        if (parseExpr(e, state)) state.exprs.push_back(e);
        else parseError(state, "Invalid syntax token");
    }
}
// ----------------------------

bool parseExpr(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;
    if (getToken(state).type == "(")
    {
        ExprPtr intern = nullptr;
        state.current++; 
        state.errorToken++;
        if (!parseExpr(intern, state)) return unparse(orig, state);
        if (getToken(state).type != ")") parseError(state, "Missing ')' before token");
        state.current++; 
        state.errorToken++;
        out = std::make_shared<Expression>("outer", "");
        out->left = intern;
    }
    else if (!parseBlock(out, state)
        && !parseCall(out, state)
        && !parseDef(out, state)
        && !parseStr(out, state)
        && !parseInt(out, state)
        && !parseFloat(out, state) 
        && !parseBool(out, state))
    {
        return unparse(orig, state);
    }

    // check for attachables
    ExprPtr attach = nullptr;
    // out = expression, first arg
    while (parseAttach(attach, state))
    {
        ExprPtr args = attach->right;
        ExprPtr newArgs = std::make_shared<Expression>("arg", "");
        // put expression as first argument
        newArgs->left = out;
        newArgs->right = args;
        // apply new arguments
        attach->right = newArgs;
        // make attached call new expression
        out = attach;
    }

    return true;
}

bool parseCall(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    if (getToken(state).type != "name" && getToken(state).type != "oper") 
        return unparse(orig, state);

    std::string name = getToken(state).val;
    uint line = getToken(state).line;
    state.current++; 
    state.errorToken++;

    // check for assignment
    if (getToken(state).val == "=")
    {
        name += "=";
        state.current++; 
        state.errorToken++;
    }

    bool hasParen = false;
    if (getToken(state).type == "(")
    {
        state.current++; 
        state.errorToken++;
        hasParen = true;
    }

    ExprPtr arg = nullptr;
    if (!parseArgs(arg, state, line)) return unparse(orig, state);

    if (getToken(state).type != ")" && hasParen)
    {
        parseError(state, "Missing ')' before token");
    }
    else if (hasParen) 
    {
        state.current++; 
        state.errorToken++;
    }

    // check for block
    ExprPtr blck = nullptr;
    parseBlock(blck, state);

    out = std::make_shared<Expression>("call", name);
    out->right = arg;
    out->left = blck;

    return true;
}

bool parseAttach(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    if (getToken(state).type == ".")
    {
        state.current++; 
        state.errorToken++;
        if (parseCall(out, state)) return true;
    }
    // Check for operators
    if (getToken(state).type == "oper" && parseCall(out, state))
    {
        return true;
    }

    return unparse(orig, state);
}

bool parseBlock(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    // check if is block
    if (getToken(state).type != "do") return unparse(orig, state);
    state.current++; 
    state.errorToken++;

    out = std::make_shared<Expression>("block", "");
    ExprPtr mainBody = std::make_shared<Expression>("main", "");
    ExprPtr elseBody = std::make_shared<Expression>("else", "");

    // get parameters
    ExprPtr pars = nullptr;
    if (getToken(state).type == "|")
    {
        state.current++; 
        state.errorToken++;
        if (!parseParams(pars, state, getToken(state).line))
        {
            parseError(state, "No parameters for block with '||'");
        }
        if (getToken(state).type != "|")
        {
            state.current--;
            parseError(state, "Expected '|' after token");
        }
        state.current++; 
        state.errorToken++;
    }

    // get main body
    ExprPtr mainExprs = std::make_shared<Expression>("expr", "");
    ExprPtr e = mainExprs;
    while (getToken(state).type != "end" && getToken(state).type != "else")
    {
        if (!parseExpr(e->left, state)) parseError(state, "Expected an expression");
        e->right = std::make_shared<Expression>("expr", "");
        e = e->right;
    }

    // get else body
    ExprPtr elseExprs = std::make_shared<Expression>("expr", "");
    e = elseExprs;
    if (getToken(state).type == "else")
    {
        state.current++; 
        state.errorToken++;
        while (getToken(state).type != "end")
        {
            if (!parseExpr(e->left, state)) parseError(state, "Expected an expression");
            e->right = std::make_shared<Expression>("expr", "");
            e = e->right;
        }
    }
    state.current++; 
    state.errorToken++;

    // build block
    mainBody->left = pars;
    mainBody->right = mainExprs;
    elseBody->right = elseExprs;
    out->left = mainBody;
    out->right = elseBody;
    return true;
}

bool parseDef(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    if (getToken(state).type != "def") return unparse(orig, state);
    state.current++; 
    state.errorToken++;
    if (getToken(state).type != "name") parseError(state, "Definition must have a name");

    std::string name = getToken(state).val;
    state.current++; 
    state.errorToken++;
    out = std::make_shared<Expression>("def", "");
    if (!parseExpr(out->right, state))
    {
        parseError(state, "No expression provided to definition");
    }

    return true;
}
// ----------------------------

bool parseArgs(ExprPtr& out, ParseState& state, uint line)
{
    ExprPtr arg = std::make_shared<Expression>("arg", "");
    if (getToken(state).line == line && getToken(state).type != "oper"
        && getToken(state).type != "do" && parseExpr(arg->left, state))
    {
        // check for more arguments if followed by comma
        ExprPtr anotherArg = nullptr;
        if (getToken(state).type == ",")
        {
            state.current++; 
            state.errorToken++;
            if (!parseArgs(anotherArg, state, line))
            {
                parseError(state, "Expected an argument after ','");
            }
        }
        arg->right = anotherArg;
    }

    out = arg;
    return true;
}

bool parseParams(ExprPtr& out, ParseState& state, uint line)
{
    uint orig = state.current;

    ExprPtr param = nullptr;
    if (getToken(state).line == line && getToken(state).type != "oper" 
        && getToken(state).type == "name")
    {
        param = std::make_shared<Expression>("param", getToken(state).val);
        state.current++; 
        state.errorToken++;
        // check for more parameters if followed by comma
        ExprPtr anotherParam = nullptr;
        if (getToken(state).type == ",")
        {
            state.current++; 
            state.errorToken++;
            if (!parseParams(anotherParam, state, line))
            {
                parseError(state, "Expected a parameter after ','");
            }
        }
        param->right = anotherParam;
    }

    if (!param) return unparse(orig, state);
    out = param;
    return true;
}
// ----------------------------

bool parseStr(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    if (getToken(state).type != "str") return unparse(orig, state);

    out = std::make_shared<Expression>("str", getToken(state).val);
    state.current++; 
    state.errorToken++;
    
    return true;
}

bool parseInt(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    if (getToken(state).type != "int") return unparse(orig, state);

    out = std::make_shared<Expression>("int", getToken(state).val);
    state.current++; 
    state.errorToken++;

    return true;
}

bool parseFloat(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    if (getToken(state).type != "float") return unparse(orig, state);

    out = std::make_shared<Expression>("float", getToken(state).val);
    state.current++; 
    state.errorToken++;

    return true;
}

bool parseBool(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    if (getToken(state).type != "bool") return unparse(orig, state);

    out = std::make_shared<Expression>("bool", getToken(state).val);
    state.current++; 
    state.errorToken++;
    
    return true;
}
// ----------------------------

const Token& getToken(ParseState& state)
{
    if (state.current < state.ls->tokens.size() - 1) return state.ls->tokens[state.current];
    else return state.ls->tokens[state.ls->tokens.size() - 1];
}

bool unparse(uint orig, ParseState& state)
{
    state.errorToken = state.current;
    state.current = orig;
    return false;
}
// ----------------------------

void parseError(const ParseState& state, const std::string& message)
{
    const Token& errTok = state.ls->tokens[state.current];

    // get error line
    std::string line = "";
    uint count = 1;
    for (char c : state.ls->source)
    {
        if (c == '\n')
        {
            if (count == errTok.line) break;
            count++;
            line = "";
        }
        else line += c;
    }

    uint size = (errTok.val == "" ? errTok.type.size() : errTok.val.size());
    std::string lineStart = line.substr(0, errTok.col - size - 1);
    std::string lineEnd = line.substr(errTok.col - 1, line.size());

    system("printf '\033[1A'");
    std::cout << "\033[38;5;14m";
    std::cout << "-- ERROR -------------------- " << state.ls->sourceName << "\n";
    std::cout << "\033[0m";
    std::cout << errTok.line << "| ";
    std::cout << "\033[38;5;15m";
    std::cout << lineStart;
    std::cout << "\033[48;5;9m";
    std::cout << (errTok.val == "" ? errTok.type : errTok.val);
    std::cout << "\033[0m";
    std::cout << "\033[38;5;15m";
    std::cout << lineEnd << "\n";
    std::cout << "\033[0m";
    std::cout << message << "\n";

    std::cin.get();
    exit(1);
}

void printExpr(const Expression& e, uint indent)
{
    for (uint i = 0; i < indent; i++) std::cout << "|   ";

    std::cout << "<" + e.type << ">" << e.val << "\n";

    if (e.left) printExpr(*e.left, indent + 1);
    if (e.right) printExpr(*e.right, indent + 1);
}

OCA_END
