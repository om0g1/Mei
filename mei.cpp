#include <iostream>

class Token {
    public:
        enum TokenType{
            INTEGER,
            PLUS,
            MINUS,
            MUL,
            DIV,
            SPACE,
            LPAREN,
            RPAREN,
            EOI,
            INVALID
        };

        TokenType m_type;
        int m_value;

        Token() : m_type(INVALID), m_value(0) {};
        Token(TokenType type, int value = 0) : m_type(type), m_value(value) {};
};

class Lexer {
    public:
        Lexer(const std::string& input) : m_input(input), m_position(0) {};
        Token getNextToken();
    private:
        const std::string& m_input;
        std::size_t m_position;
};

Token Lexer::getNextToken() {
    if (m_position >= m_input.length()) {
        return Token(Token::EOI);
    }

    while (std::isspace(m_input[m_position])) {
        m_position++;
    }

    char currentChar = m_input[m_position];

    if (std::isdigit(currentChar)) {
        int value = 0;
        while (m_position < m_input.length() && std::isdigit(m_input[m_position])) {
            value = value * 10 + (m_input[m_position] - '0');
            m_position ++;
        }
        return Token(Token::INTEGER, value);
    }


    switch (currentChar) {
    case '+': m_position++; return Token(Token::PLUS);
    case '-': m_position++; return Token(Token::MINUS);
    case '*': m_position++; return Token(Token::MUL);
    case '/': m_position++; return Token(Token::DIV);
    case '(': m_position++; return Token(Token::LPAREN);
    case ')': m_position++; return Token(Token::RPAREN);
    default: m_position++; return Token(Token::INVALID);
    }

};

class Parser {
    public:
        Parser(Lexer& lexer) : m_lexer(lexer) {
            m_currentToken = m_lexer.getNextToken();
        };
        int parse();

    private:
        Lexer& m_lexer;
        Token m_currentToken;

        void eat(Token::TokenType);
        int factor();
        int term();
        int expr();
};

void Parser::eat(Token::TokenType tokenType) {
    if (m_currentToken.m_type == tokenType) {
        m_currentToken = m_lexer.getNextToken();
    } else {
        throw std::runtime_error("Unexpected token");
    }
}

int Parser::factor() {
    Token token = m_currentToken;
    if (token.m_type == Token::INTEGER) {
        eat(Token::INTEGER);
        return token.m_value;
    } else if (token.m_type == Token::LPAREN) {
        eat(Token::LPAREN);
        int value = expr();
        eat(Token::RPAREN);
        return value;
    }
    throw std::runtime_error("Unexpected token");
}

int Parser::term() {
    int result = factor();
    while (m_currentToken.m_type == Token::MUL || m_currentToken.m_type == Token::DIV) {
        if (m_currentToken.m_type == Token::MUL) {
            eat(Token::MUL);
            result *= factor();
        } else if (m_currentToken.m_type == Token::DIV) {
            eat(Token::DIV);
            result /= factor();
        } else {
            throw std::runtime_error("Unexpected token");
        }
    }
    return result;
}

int Parser::expr() {
    int result = term();
    while (m_currentToken.m_type == Token::PLUS || m_currentToken.m_type == Token::MINUS) {
        if (m_currentToken.m_type == Token::PLUS) {
            eat(Token::PLUS);
            result += term();
        } else if (m_currentToken.m_type == Token::DIV) {
            eat(Token::MINUS);
            result += term();
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
        Interpreter(Parser& parser) : m_parser(parser) {};
        int interprete();
    private:
        Parser& m_parser;
};

int Interpreter::interprete() {
    return m_parser.parse();
}

int main () {
    std::cout << "Mei 0.0.1 (tags 7/28/2023 1:00PM)" << std::endl;
    std::cout << "type 'quit()' to quit " << std::endl;

    while (true) {
        std::string expression;
        std::cout << ">>> ";
        std::getline(std::cin, expression);
        
        if (expression == "quit()") return 0;

        Lexer lexer(expression);
        Parser Parser(lexer);
        Interpreter interpreter(Parser);

        try {
            std::cout << ">>> " << interpreter.interprete() << std::endl;
        } catch (std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
};