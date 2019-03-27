#include <stdio.h>
#include <iostream>

// D.V. Malov
// SD calculator

// block of global variables for lexical analyzer for convinience of reading
std::string lexvalue = "";
enum LEXTYPE { OP, NUM, UNOP };
LEXTYPE lextype;
bool endol = false;
char curlex; /* current lexeme */ 

// input params
const char* ginp;
int gpos = 0;

void getlex (void); /* selects the next token from the input stream */ 
int expr (void); /* start token */ 
int add (void);
int mult (void);
int oppow(void); /* power operation - extension to base logic */
int power(int base, int exp);
void error (); /*  */

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

int main(int argc, char* argv[]) {
  int result;

  if (argc != 2) {
    std::cout << "error" << std::endl;
    return 1;
  }
  else {
    std::string params = "";
    for (int i = 1; i < argc; i++) {
      params += argv[i];
    }
    params = ReplaceAll(params, " ", "");

    ginp = params.c_str();
    gpos = 0;

    getlex();
    result = expr();
    if (curlex != '\n') error();
    printf("%d\n", result);
  }

  return 0;
}

char nextchar() {
  char curr = ginp[gpos];
  if (curr == '\0') curr = '\n'; else gpos++;
  return curr;
}

void goback() {
  gpos--;
}

void getlex() {
  if (endol) return;
  // delete spaces and read first symbol of lexem
  while ((curlex = nextchar()) == ' ');
  if (curlex == '\n') return;

  bool endoflex = false;
  lexvalue = "";
  lextype = OP;

  do
  {
    switch (curlex) {
      case '0': case '1': case '2': case '3': case '4': case '5':
      case '6': case '7': case '8': case '9':
        lextype = NUM;
        lexvalue += curlex;
        curlex = nextchar();
        break;
      case '+':
      case '*':
      case '/':
      case '-':
        endoflex = true;
        if (lextype == NUM){
          goback();
          curlex = '0';
        }
        else {
          lextype = OP;
        }
        break;
      case '\n':
        endol = true;
        endoflex = true;
        break;
      default:
        error();
        return;
      }
  } while (!endoflex);
}

void error(void) {
  printf("error");
  exit(1);
}

int expr() {
  int e = add();
  int sign;
  while ((curlex == '+') || (curlex == '-')) {
    sign = (curlex == '+');
    getlex();
    if (sign)
      e += add();
    else {
      e -= add();
    }
  }
  return e;
}

int add() {
  int a = mult();
  int sign;
  while ((curlex == '*') || (curlex == '/'))
  {
    sign = (curlex == '*');
    getlex();
    if (sign)
      a *= mult();
    else
    {
      int bottom = mult();
      if (bottom == 0) error();
      a = a/bottom;
    }
  }
  return a;
}

int power(int base, int exp) {
  int ret = 1;
  if ((base*exp) == 0) error();
  if (exp == 0) return 1;
  if (exp < 0) error();
  for (int i=0; i<exp; i++) ret *= base;
  return ret;
}

int mult() {
  int a = oppow();
  while (curlex == '^')
     {getlex(); a = power(a, mult());}
  return a;
}

int oppow() {
  int m;
  if (lextype == NUM) {
    m = std::stoi(lexvalue);
  }
  else {
    switch (curlex) {
      case ('-'):
        getlex();
        if (lextype == NUM) {
          m = std::stoi(lexvalue) * (-1);
        }
        else
          m = expr();
        break;
      case '(': 
        getlex(); 
        m = expr();
        if (curlex == ')') break;
      /* the error if there is no closing bracket */
      default: error();
    }
  }

  getlex();
  return m;
}

