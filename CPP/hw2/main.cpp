#include <iostream>

// D.V. Malov
// SD calculator (class implementation)

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

class ErrorProcException: public std::exception
{
  virtual const char* what() const throw()
  {
    return "Processing error";
  }
};

class Calulator {
private:
  const char* ginp;   // input string to parse
  int gpos = 0;   // current position in input string

  enum lexem_type { OP, NUM, UNOP };
  bool endol = false;
  char curlex; /* current lexeme */ 
  lexem_type lextype;
  std::string lexvalue = "";

public:
  Calulator(const char* inputstr) {
    this->ginp = inputstr;
    this->gpos = 0;
  }

  int64_t start() {
    this->getlex();
    return expr();
  }

private:
  char nextchar() {
    char curr = this->ginp[this->gpos];
    if (curr == '\0') curr = '\n'; else (this->gpos)++;
    return curr;
  }

  // go to previous position in input string
  void goback() {
    this->gpos--;
  }

  // get next lexeme
  void getlex() {
    if (this->endol) return;
    // delete spaces and read first symbol of lexem
    while ((this->curlex = this->nextchar()) == ' ');
    if (this->curlex == '\n') return;

    bool endoflex = false;

    this->lexvalue = "";
    this->lextype = OP;

    do
    {
      switch (this->curlex) {
        case '0': case '1': case '2': case '3': case '4': 
        case '5': case '6': case '7': case '8': case '9':
          this->lextype = NUM;
          this->lexvalue += this->curlex;
          this->curlex = this->nextchar();
          break;
        case '+':
        case '*':
        case '/':
        case '-':
        case '^':
          endoflex = true;
          if (this->lextype == NUM){
            this->goback();
            this->curlex = '0';
          }
          else {
            this->lextype = OP;
          }
          break;
        case '\n':
          this->endol = true;
          endoflex = true;
          break;
        default:
          this->error();
          return;
        }
    } while (!endoflex);
  }

  void error(void) {
    throw ErrorProcException();
  }

  int64_t expr() {
    int64_t e = this->add();
    bool sign;
    while ((this->curlex == '+') || (this->curlex == '-')) {
      sign = (this->curlex == '+');
      getlex();
      if (sign)
        e += this->add();
      else {
        e -= this->add();
      }
    }
    return e;
  }

  int64_t add() {
    int64_t a = this->mult();
    bool sign;
    while ((this->curlex == '*') || (this->curlex == '/'))
    {
      sign = (this->curlex == '*');
      this->getlex();
      if (sign)
        a *= this->mult();
      else
      {
        int64_t bottom = this->mult();
        if (bottom == 0) this->error();
        a = a/bottom;
      }
    }
    return a;
  }

  int64_t power(int64_t base, int64_t exp) {
    int64_t ret = 1;
    if ((base*exp) == 0) this->error();
    if (exp == 0) return 1;
    if (exp < 0) this->error();
    for (int i=0; i<exp; i++) ret *= base;
    return ret;
  }

  int64_t mult() {
    int64_t a = this->oppow();
    while (this->curlex == '^')
       {this->getlex(); a = this->power(a, this->mult());}
    return a;
  }

  int64_t oppow() {
    int64_t m;
    if (lextype == NUM) {
      m = std::stoi(this->lexvalue);
    }
    else {
      switch (this->curlex) {
        case ('-'):
          getlex();
          if (this->lextype == NUM) {
            m = std::stoi(this->lexvalue) * (-1);
          }
          else
            m = this->expr();
          break;
        case '(': 
          this->getlex(); 
          m = this->expr();
          if (this->curlex == ')') break;
        /* the error if there is no closing bracket */
        default: this->error();
      }
    }

    getlex();
    return m;
  }
};

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "error" << std::endl;
    return 1;
  }
  else {
    try {
      std::string params = argv[1];
      params = ReplaceAll(params, " ", "");
      const char* inp = params.c_str();

      Calulator calc(inp);
      int64_t result = calc.start();
      std::cout << result << std::endl;
    }
    catch (ErrorProcException& ex) {  // parsing error - my custom exception
      std::cout << "error" << std::endl;
      return 1;
    }
    catch (std::out_of_range& ex) {   // trying to calc too big value
      std::cout << "error" << std::endl;
      return 1;
    }
  }
  return 0;
}

