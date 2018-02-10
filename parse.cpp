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

const Token& ParseState::get()
{
    if (current < ls->tokens.size() - 1) return ls->tokens[current];
    else return ls->tokens[ls->tokens.size() - 1];
}

void ParseState::next()
{
    current++;
    errorToken++;
}

bool ParseState::unparse(uint orig)
{
    errorToken = current;
    current = orig;
    return false;
}

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
    
    if (!parseBlock(out, state)
     && !parseCall(out, state)
     && !parseDef(out, state)
     && !parseVal(out, state))
    {
        return state.unparse(orig);
    }

    return true;
}

bool parseCall(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    if (state.get().type != "name") return state.unparse(orig);

    std::string name = state.get().val;
    uint line = state.get().line;
    state.next(); 
    
    // assignment sugar
    if (state.get().val == "=")
    {
        name += "=";
        state.next();
    }

    out = std::make_shared<Expression>("call", name);

    if (state.get().type != "do" && state.get().line == line) 
        parseVal(out->right, state); // argument
    if (state.get().line == line)
        parseBlock(out->left, state);

    // check for attachables
    ExprPtr attach = nullptr;
    if (parseAttach(attach, state))
    {
        ExprPtr call = out;
        out = std::make_shared<Expression>("attach", "");
        out->left = call;
        out->right = attach;
    }

    return true;
}

bool parseAttach(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    if (state.get().type == ".")
    {
        state.next(); 
        if (parseCall(out, state)) return true;
    }
    // Check for operators
    if (state.get().type == "oper" && parseCall(out, state))
    {
        return true;
    }

    return state.unparse(orig);
}

bool parseBlock(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    // check if is block
    if (state.get().type != "do") return state.unparse(orig);
    state.next(); 
    

    out = std::make_shared<Expression>("block", "");
    ExprPtr mainBody = std::make_shared<Expression>("main", "");
    ExprPtr elseBody = std::make_shared<Expression>("else", "");

    // get parameters
    ExprPtr param = nullptr;
    if (state.get().type == ":")
    {
        state.next();
        if (!parseName(param, state))
        {
            parseError(state, "No parameters for block starting with ':'");
        }
        
    }

    // get main body
    ExprPtr mainExprs = std::make_shared<Expression>("expr", "");
    ExprPtr e = mainExprs;
    while (state.get().type != "end" && state.get().type != "else")
    {
        if (!parseExpr(e->left, state)) parseError(state, "Expected an expression");
        e->right = std::make_shared<Expression>("expr", "");
        e = e->right;
    }

    // get else body
    ExprPtr elseExprs = std::make_shared<Expression>("expr", "");
    e = elseExprs;
    if (state.get().type == "else")
    {
        state.next(); 
        
        while (state.get().type != "end")
        {
            if (!parseExpr(e->left, state)) parseError(state, "Expected an expression");
            e->right = std::make_shared<Expression>("expr", "");
            e = e->right;
        }
    }
    state.next(); 
    

    // build block
    mainBody->left = param;
    mainBody->right = mainExprs;
    elseBody->right = elseExprs;
    out->left = mainBody;
    out->right = elseBody;
    return true;
}

bool parseDef(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    if (state.get().type != "def") return state.unparse(orig);
    state.next(); 
    
    out = std::make_shared<Expression>("def", "");
    if (!parseName(out->left, state)) 
        parseError(state, "Definition must have a name");
    if (!parseExpr(out->right, state))
        parseError(state, "No expression provided to definition");

    return true;
}

// ----------------------------

bool parseVal(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    if (state.get().type == "(") // if is a tuple
    {
        state.next();
        out = std::make_shared<Expression>("tup", "");
        ExprPtr curr = out;

        bool more = true;
        while (more)
        {
            std::string name = "";
            if (state.get().type == "name")
            {
                name = state.get().val;
                state.next();
                if (state.get().type == ":")
                {
                    state.next();
                }
                else
                {
                    state.current--;
                    state.errorToken--;
                    name = "";
                }
            }
            more = parseExpr(curr->left, state);
            if (!more) break;
            curr->val = name;
            curr->right = std::make_shared<Expression>("tup", "");
            curr = curr->right;
        }
        if (state.get().type != ")") parseError(state, "Missing ')' before token");
        state.next();
    }
    else // if is a single value
    {
        if (!parseStr(out, state)
         && !parseInt(out, state)
         && !parseFloat(out, state)
         && !parseBool(out, state))
        {
            return state.unparse(orig);
        }
    }

    // check for attachables
    ExprPtr attach = nullptr;
    if (parseAttach(attach, state))
    {
        ExprPtr call = out;
        out = std::make_shared<Expression>("attach", "");
        out->left = call;
        out->right = attach;
    }

    return true;
}

bool parseName(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    if (state.get().type == "(") // if is a tuple
    {
        state.next();
        out = std::make_shared<Expression>("name", "");
        ExprPtr curr = out;
        while (state.get().type == "name")
        {
            curr->val = state.get().val;
            curr->right = std::make_shared<Expression>("name", "");
            curr = curr->right;
            state.next();
        }
        if (state.get().type != ")") parseError(state, "Missing ')' before token");
        state.next();

        return true;
    }
    else // if is a single value
    {
        if (state.get().type != "name") return state.unparse(orig);
        out = std::make_shared<Expression>("name", state.get().val);
        state.next();
        return true;
    }
}

// ----------------------------

bool parseStr(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    if (state.get().type != "str") return state.unparse(orig);

    out = std::make_shared<Expression>("str", state.get().val);
    state.next(); 
    
    return true;
}

bool parseInt(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    if (state.get().type != "int") return state.unparse(orig);

    out = std::make_shared<Expression>("int", state.get().val);
    state.next(); 

    return true;
}

bool parseFloat(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    if (state.get().type != "float") return state.unparse(orig);

    out = std::make_shared<Expression>("float", state.get().val);
    state.next(); 

    return true;
}

bool parseBool(ExprPtr& out, ParseState& state)
{
    uint orig = state.current;

    if (state.get().type != "bool") return state.unparse(orig);

    out = std::make_shared<Expression>("bool", state.get().val);
    state.next(); 
    
    return true;
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
