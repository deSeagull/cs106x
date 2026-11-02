/*
 * File: parser.cpp
 * ----------------
 * This file implements the parser.h interface.
 */

#include <iostream>
#include <string>
#include "error.h"
#include "exp.h"
#include "parser.h"
#include "strlib.h"
#include "tokenscanner.h"
#include "ssutil.h"
using namespace std;

static Expression *readE(TokenScanner& scanner, int prec, SSModel& model);
static Expression *readT(TokenScanner& scanner, SSModel& model);
static int precedence(const std::string& token);

/**
 * Implementation notes: parseExp
 * ------------------------------
 * This code just reads an expression and then checks for extra tokens.
 */

Expression *parseExp(TokenScanner& scanner, SSModel& model) {
   Expression *exp = readE(scanner, 0, model);
   if (scanner.hasMoreTokens()) {
      error("Unexpected token \"" + scanner.nextToken() + "\"");
   }
   return exp;
}

/**
 * Implementation notes: readE
 * Usage: exp = readE(scanner, prec);
 * ----------------------------------
 * The implementation of readE uses precedence to resolve the ambiguity in
 * the grammar.  At each level, the parser reads operators and subexpressions
 * until it finds an operator whose precedence is greater than the prevailing
 * one.  When a higher-precedence operator is found, readE calls itself
 * recursively to read that subexpression as a unit.
 */

Expression *readE(TokenScanner& scanner, int prec, SSModel& model) {
   Expression *exp = readT(scanner, model);
   string token;
   while (true) {
      token = toUpperCase(scanner.nextToken());
      int tprec = precedence(token);
      if (tprec <= prec) break;
      Expression *rhs = readE(scanner, tprec, model);
      exp = new CompoundExp(token, exp, rhs);
   }
   scanner.saveToken(token);
   return exp;
}

/**
 * Implementation notes: readT
 * ---------------------------
 * This function scans a term, which is either an integer, an identifier,
 * or a parenthesized subexpression.
 */
Expression *readT(TokenScanner& scanner, SSModel& model) {
   string token = scanner.nextToken();
   TokenScanner::TokenType type = scanner.getTokenType(token);
   if (type == TokenScanner::WORD) {
       string s = scanner.nextToken();
       if (s == "(") {
           string cellname1 = toUpperCase(scanner.nextToken());
           if (!model.nameIsValid(cellname1)) {
               error("invaild cell reference" + cellname1);
           }
           if(scanner.nextToken() != ":") {
               error("expected a :");
           }
           string cellname2 = toUpperCase(scanner.nextToken());
           if (!model.nameIsValid(cellname2)) {
               error("invaild cell reference" + cellname2);
           }
           if (scanner.nextToken() != ")") {
               error("Unbalanced parentheses");
           }
           return new RangeFunctionExp(token, cellname1, cellname2);
       }
       else {
           scanner.saveToken(s);
           if (!model.nameIsValid(token)) {
               error("invaild cell reference " + token);
           }
           else {
               return new IdentifierExp(toUpperCase(token));
           }
       }
   }
   if (type == TokenScanner::NUMBER) return new DoubleExp(stringToReal(token));
   if (type == TokenScanner::STRING) return new TextStringExp(token.substr(1, token.length() - 2));
   if (token != "(") error("Unexpected token \"" + token + "\"");
   Expression *exp = readE(scanner, 0, model);
   if (scanner.nextToken() != ")") {
      error("Unbalanced parentheses");
   }
   return exp;
}

/**
 * Implementation notes: precedence
 * --------------------------------
 * This function checks the token against each of the defined operators
 * and returns the appropriate precedence value.
 */
int precedence(const string& token) {
   if (token == "+" || token == "-") return 1;
   if (token == "*" || token == "/") return 2;
   return 0;
}
