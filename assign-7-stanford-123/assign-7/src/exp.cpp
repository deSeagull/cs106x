/**
 * File: exp.cpp
 * -------------
 * This file implements the exp.h interface.
 */

#include <string>
#include "exp.h"
#include "strlib.h"
#include "error.h"
#include "ssmodel.h"
#include "ssutil.h"
using namespace std;

/**
 * Implementation notes: Expression
 * --------------------------------
 * The Expression class itself implements only those methods that
 * are not designated as pure virtual.
 */

Expression::Expression() {
   /* Empty */
}

Expression::~Expression() {
   /* Empty */
}

/**
 * Implementation notes: DoubleExp
 * -------------------------------
 * The ConstantExp subclass represents a numeric constant.  The eval
 * method simply returns that value.
 */

DoubleExp::DoubleExp(double value) {
   this->value = value;
}

double DoubleExp::eval(SSModel &) const {
   return value;
}

string DoubleExp::toString() const {
   return realToString(value);
}

ExpressionType DoubleExp::getType() const {
   return DOUBLE;
}

Set<string> DoubleExp::getDependencies() const {
    Set<string> set1;
    return set1;
}

double DoubleExp::getDoubleValue() const {
   return value;
}

/**
 * Implementation notes: TextStringExp
 * -----------------------------------
 * The IdentifierExp subclass represents a text string constant.  The
 * implementation of eval simply returns 0.0.
 */

TextStringExp::TextStringExp(const string& str) {
    this->str = str;
}

double TextStringExp::eval(SSModel &) const {
    return 0.0;
}

string TextStringExp::toString() const {
    return str;
}

ExpressionType TextStringExp::getType() const {
    return TEXTSTRING;
}

Set<string> TextStringExp::getDependencies() const {
    Set<string> set1;
    return set1;
}

string TextStringExp::getTextStringValue() const {
    return '"' + str + '"';
}
/**
 * Implementation notes: IdentifierExp
 * -----------------------------------
 * The IdentifierExp subclass represents a variable name.  The
 * implementation of eval looks up that name in the evaluation context.
 */

IdentifierExp::IdentifierExp(const string& name) {
   this->name = name;
}

double IdentifierExp::eval(SSModel & model) const {
    return model.getCellValue(this->name);
}

string IdentifierExp::toString() const {
   return name;
}

ExpressionType IdentifierExp::getType() const {
   return IDENTIFIER;
}

Set<string> IdentifierExp::getDependencies() const {
    Set<string> set1;
    set1.add(toUpperCase(name));
    return set1;
}

string IdentifierExp::getIdentifierName() const {
   return name;
}

/**
 * Implementation notes: CompoundExp
 * ---------------------------------
 * The implementation of eval for CompoundExp evaluates the left and right
 * subexpressions recursively and then applies the operator.  Assignment is
 * treated as a special case because it does not evaluate the left operand.
 */

CompoundExp::CompoundExp(const string& op, const Expression *lhs, const Expression *rhs) {
   this->op = op;
   this->lhs = lhs;
   this->rhs = rhs;
}

CompoundExp::~CompoundExp() {
   delete lhs;
   delete rhs;
}

double CompoundExp::eval(SSModel & model) const {
   double right = rhs->eval(model);
   double left = lhs->eval(model);
   if (op == "+") return left + right;
   if (op == "-") return left - right;  
   if (op == "*") return left * right;
   if (op == "/") return left / right; // divide by 0.0 gives ±INF
    
   error("Illegal operator in expression.");
   return 0.0;
}

string CompoundExp::toString() const {
   return '(' + lhs->toString() + ' ' + op + ' ' + rhs->toString() + ')';
}

ExpressionType CompoundExp::getType() const {
   return COMPOUND;
}

Set<string> CompoundExp::getDependencies() const {
    Set<string> set1;
    Set<string>set2 = rhs->getDependencies();
    Set<string>set3 = lhs->getDependencies();
    set1 = set2 + set3;
    return set1;
}

string CompoundExp::getOperator() const {
   return op;
}

const Expression *CompoundExp::getLHS() const {
   return lhs;
}

const Expression *CompoundExp::getRHS() const {
   return rhs;
}

/**
 * Implementation notes: CompoundExp
 */

RangeFunctionExp::RangeFunctionExp(const string functionname, const string cellname1, const string cellname2) {
    this->cellname1 = cellname1;
    this->cellname2 = cellname2;
    this->functionname = functionname;
    table = setFuncTable();
}
double RangeFunctionExp::eval(SSModel& model) const {
    Vector<double> values;
    location loc1;
    location loc2;
    stringToLocation(cellname1, loc1);
    stringToLocation(cellname2, loc2);
    for(int i = loc1.row; i <= loc2.row; i++) {
        for (char j = loc1.col; j <= loc2.col; j++) {
            location tempt{j, i};
            string s = locationToString(tempt);
            values.add(model.getCellValue(s));
        }
    }
    string s = toLowerCase(functionname);
    if(table.containsKey(s)){
        return table[s](values);
    } else{
        error("invalid function name " + s);
    }
}

string RangeFunctionExp::toString() const {
    return functionname + '(' + cellname1 + ':' + ' ' + cellname2 + ')';
}

ExpressionType RangeFunctionExp::getType() const {
    return RANGEFUNCTION;
}

Set<string> RangeFunctionExp::getDependencies() const {
    Set<string> set1;
    location loc1;
    location loc2;
    stringToLocation(cellname1, loc1);
    stringToLocation(cellname2, loc2);
    for(int i = loc1.row; i <= loc2.row; i++) {
        for (char j = loc1.col; j <= loc2.col; j++) {
            location tempt{j, i};
            string s = locationToString(tempt);
            set1.add(s);
        }
    }
    return set1;
}

string RangeFunctionExp::getRangeFunctionName() const {
    return functionname + '(' + cellname1 + ':' + ' ' + cellname2 + ')';
}

/**
 * Implementation notes: EvaluationContext
 * ---------------------------------------
 * The methods in the EvaluationContext class simply call the appropriate
 * method on the map used to represent the symbol table.
 */

void EvaluationContext::setValue(const string& var, double value) {
   symbolTable.put(var, value);
}

double EvaluationContext::getValue(const string& var) const {
   return symbolTable.get(var);
}

bool EvaluationContext::isDefined(const string& var) const {
   return symbolTable.containsKey(var);
}
