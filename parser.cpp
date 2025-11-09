#include <bits/stdc++.h>
using namespace std;

// --- Definición de tokens (igual que en lexer.cpp) ---
enum class TokenType
{
    L_CORCHETE,
    R_CORCHETE,
    L_LLAVE,
    R_LLAVE,
    COMA,
    DOS_PUNTOS,
    STRING,
    NUMBER,
    PR_TRUE,
    PR_FALSE,
    PR_NULL,
    EOF_TOKEN
};

// --- Estructura de token ---
struct Token
{
    TokenType type;
    string lexeme;
};

// --- Analizador Léxico simplificado (reutilizable) ---
vector<Token> tokenize(const string &path)
{
    ifstream in(path);
    if (!in)
    {
        cerr << "No se pudo abrir el archivo fuente: " << path << endl;
        exit(1);
    }

    vector<Token> tokens;
    string s;
    while (getline(in, s))
    {
        for (size_t i = 0; i < s.size(); i++)
        {
            char c = s[i];
            if (isspace(c))
                continue;
            if (c == '{')
                tokens.push_back({TokenType::L_LLAVE, "{"});
            else if (c == '}')
                tokens.push_back({TokenType::R_LLAVE, "}"});
            else if (c == '[')
                tokens.push_back({TokenType::L_CORCHETE, "["});
            else if (c == ']')
                tokens.push_back({TokenType::R_CORCHETE, "]"});
            else if (c == ',')
                tokens.push_back({TokenType::COMA, ","});
            else if (c == ':')
                tokens.push_back({TokenType::DOS_PUNTOS, ":"});
            else if (c == '"')
            {
                size_t j = i + 1;
                while (j < s.size() && s[j] != '"')
                    j++;
                if (j < s.size())
                {
                    tokens.push_back({TokenType::STRING, s.substr(i, j - i + 1)});
                    i = j;
                }
                else
                {
                    cerr << "Error léxico: cadena sin cierre." << endl;
                    exit(1);
                }
            }
            else if (isdigit(c))
            {
                size_t j = i;
                while (j < s.size() && (isdigit(s[j]) || s[j] == '.'))
                    j++;
                tokens.push_back({TokenType::NUMBER, s.substr(i, j - i)});
                i = j - 1;
            }
            else
            {
                string w;
                while (isalpha(s[i]))
                {
                    w += s[i];
                    i++;
                }
                i--;
                if (w == "true" || w == "TRUE")
                    tokens.push_back({TokenType::PR_TRUE, w});
                else if (w == "false" || w == "FALSE")
                    tokens.push_back({TokenType::PR_FALSE, w});
                else if (w == "null" || w == "NULL")
                    tokens.push_back({TokenType::PR_NULL, w});
                else
                {
                    cerr << "Error léxico: palabra no reconocida '" << w << "'" << endl;
                    exit(1);
                }
            }
        }
    }
    tokens.push_back({TokenType::EOF_TOKEN, "EOF"});
    return tokens;
}

// --- Variables globales del parser ---
vector<Token> tokens;
size_t pos = 0;

// --- Utilidades del parser ---
Token &current() { return tokens[pos]; }
void advance()
{
    if (pos < tokens.size() - 1)
        pos++;
}
bool match(TokenType t)
{
    if (current().type == t)
    {
        advance();
        return true;
    }
    return false;
}
void error(const string &msg)
{
    cerr << "Error sintáctico cerca de '" << current().lexeme << "': " << msg << endl;
}

// --- Prototipos de las funciones del parser ---
void json();
void element();
void object();
void attributes_list();
void attribute();
void parseArray();
void element_list();
void attribute_value();

// --- Implementación de las reglas (BNF del enunciado) ---
void json()
{
    element();
    if (current().type != TokenType::EOF_TOKEN)
        error("se esperaba fin de archivo");
}

void element()
{
    if (current().type == TokenType::L_LLAVE)
        object();
    else if (current().type == TokenType::L_CORCHETE)
        parseArray();
    else
        error("se esperaba objeto o arreglo");
}

void object()
{
    if (!match(TokenType::L_LLAVE))
    {
        error("se esperaba '{'");
        return;
    }
    if (match(TokenType::R_LLAVE))
        return; // objeto vacío
    attributes_list();
    if (!match(TokenType::R_LLAVE))
        error("se esperaba '}' al final del objeto");
}

void attributes_list()
{
    attribute();
    while (match(TokenType::COMA))
        attribute();
}

void attribute()
{
    if (!match(TokenType::STRING))
    {
        error("se esperaba nombre de atributo (string)");
        return;
    }
    if (!match(TokenType::DOS_PUNTOS))
    {
        error("se esperaba ':'");
        return;
    }
    attribute_value();
}

void attribute_value()
{
    if (current().type == TokenType::STRING)
        advance();
    else if (current().type == TokenType::NUMBER)
        advance();
    else if (current().type == TokenType::PR_TRUE)
        advance();
    else if (current().type == TokenType::PR_FALSE)
        advance();
    else if (current().type == TokenType::PR_NULL)
        advance();
    else if (current().type == TokenType::L_LLAVE)
        object();
    else if (current().type == TokenType::L_CORCHETE)
        parseArray();
    else
        error("valor de atributo inválido");
}

void parseArray()
{
    if (!match(TokenType::L_CORCHETE))
    {
        error("se esperaba '['");
        return;
    }
    if (match(TokenType::R_CORCHETE))
        return; // array vacío
    element_list();
    if (!match(TokenType::R_CORCHETE))
        error("se esperaba ']' al final del arreglo");
}

void element_list()
{
    element();
    while (match(TokenType::COMA))
        element();
}

// --- MAIN ---
int main()
{
    tokens = tokenize("fuente.txt");
    cout << "Analizando fuente..." << endl;
    json();
    if (current().type == TokenType::EOF_TOKEN)
        cout << " Fuente sintacticamente correcta." << endl;
    else
        cout << " Fuente con errores sintácticos." << endl;
    return 0;
}
