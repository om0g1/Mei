#include <iostream>

class Token {
    public:
        enum TokenType{
            INTEGER,
            PLUS,
            MINUS,
            MUL,
            DIV,
            LPAREN,
            RPAREN,
            SPACE,
            EOI, // End of Input
            INVALID // Invalid token
        };
    
    TokenType type;
    int value;

    Token() : type(INVALID), value(0) {};
    Token(TokenType type, int value = 0) : type(type), value(value) {};
    
};

class Lexer {
    public:
        Lexer(std::string& input) : input_(input), position_(0) {};
        Token getNextToken();
    private:
        std::string input_;
        int position_;
};

Token Lexer::getNextToken() {
    if (position_ >= input_.length()) {
        return Token(Token::EOI);
    }

    while (position_ < input_.length() && std::isspace(input_[position_])) {
        position_ ++;
    }

    char currentChar = input_[position_];

    if (std::isdigit(currentChar)) {
        int value = 0;
        while (position_ < input_.length() && std::isdigit(input_[position_])) {
            value = value * 10 + (input_[position_] - '0');
            position_++;
        }
        return Token(Token::INTEGER, value);
    }

    switch (currentChar) {
    case '+': position_++; return { Token::PLUS };
    case '-': position_++; return { Token::MINUS };
    case '*': position_++; return { Token::MUL };
    case '/': position_++; return { Token::DIV };
    case '(': position_++; return { Token::LPAREN };
    case ')': position_++; return { Token::RPAREN };
    default: position_++; return {Token(Token::INVALID)};
    }
}

class Parser {
    public: 
        Parser(Lexer& lexer) : lexer_(lexer){
            currentToken_ = lexer_.getNextToken();
            };
        int parse();
    private:
        Lexer& lexer_;
        Token currentToken_;
        void eat(Token::TokenType tokenType);
        int factor();
        int term();
        int expr();
};


void Parser::eat(Token::TokenType tokenType) {
    if (currentToken_.type == tokenType) currentToken_ = lexer_.getNextToken();
    else {
        throw std::runtime_error("Unexpected token");
    }
}

int Parser::factor() {
    Token token = currentToken_;
    if (token.type == Token::INTEGER) {
        eat(Token::INTEGER);
        return token.value;
    } else if (token.type == Token::LPAREN) {
        eat(Token::LPAREN);
        int result = expr();
        eat(Token::RPAREN);
        return result;
    }
    throw std::runtime_error("Unexpected token");
}

int Parser::term() {
    int result = factor();
    while (currentToken_.type == Token::MUL || currentToken_.type == Token::DIV) {
        Token token = currentToken_;
        if (token.type == Token::MUL) {
            eat(Token::MUL);
            result *= factor();
        } else if (token.type == Token::DIV) {
            eat(Token::DIV);
            int divisor = currentToken_.value;
            if (divisor != 0) {
                result /= factor();
            } else {
                throw std::runtime_error("Division by zero");
            }
        } else {
            throw std::runtime_error("Unexpected error");
        }
    }
    return result;
}

int Parser::expr() {
    int result = term();
    while (currentToken_.type == Token::PLUS || currentToken_.type == Token::MINUS) {
        Token token = currentToken_;
        if (token.type == Token::PLUS) {
            eat(Token::PLUS);
            result += term();
        } else if (token.type == Token::MINUS) {
            eat(Token::MINUS);
            result -= term();
        } else {
            throw std::runtime_error("Unexpected token");
        }
    }
    return result;
}

int Parser::parse() {
    return expr();
}

class Interpreter {
    public:
        Interpreter(Parser& parser) : parser_(parser) {};
        int interpret();
    private:
        Parser& parser_;
};

int Interpreter::interpret() {
    return parser_.parse();
}

int main() {
    std::string expression;
    std::cout << ">>> ";
    std::getline(std::cin, expression);

    if (expression == "quit") return 0;

    Lexer lexer(expression);
    Parser parser(lexer);
    Interpreter interpreter(parser);

    try {
        int result = interpreter.interpret();
        std::cout << ">>> " << result << std::endl;
    } catch (const std::exception& e) {
        std::cout << ">>> Error: " << e.what() << std::endl;
    }
    main();
}