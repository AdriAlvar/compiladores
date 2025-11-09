#include <bits/stdc++.h>
using namespace std;

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

struct Token
{
    TokenType type;
    string lexeme;
};

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

vector<Token> tokens;
size_t pos = 0;
ofstream out;

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
void error(const string &msg) { cerr << "Error sintáctico cerca de '" << current().lexeme << "': " << msg << endl; }

void json(int indent = 0);
void element(int indent);
void object(int indent, const string &tagName = "");
void attributes_list(int indent);
void attribute(int indent);
void parseArray(int indent, const string &tagName = "");
void element_list(int indent);
void attribute_value(int indent, const string &tagName);

void json(int indent)
{
    element(indent);
    if (current().type != TokenType::EOF_TOKEN)
        error("se esperaba fin de archivo");
}

void element(int indent)
{
    if (current().type == TokenType::L_LLAVE)
        object(indent);
    else if (current().type == TokenType::L_CORCHETE)
        parseArray(indent);
    else
        error("se esperaba objeto o arreglo");
}

void object(int indent, const string &tagName)
{
    if (!match(TokenType::L_LLAVE))
    {
        error("se esperaba '{'");
        return;
    }
    if (match(TokenType::R_LLAVE))
        return;

    if (!tagName.empty())
        out << string(indent, '\t') << "<" << tagName << ">\n";
    attributes_list(indent + 1);
    if (!match(TokenType::R_LLAVE))
        error("se esperaba '}' al final del objeto");
    if (!tagName.empty())
        out << string(indent, '\t') << "</" << tagName << ">\n";
}

void attributes_list(int indent)
{
    attribute(indent);
    while (match(TokenType::COMA))
        attribute(indent);
}

void attribute(int indent)
{
    if (current().type != TokenType::STRING)
    {
        error("se esperaba nombre de atributo");
        return;
    }

    string attrName = current().lexeme;
    attrName.erase(remove(attrName.begin(), attrName.end(), '"'), attrName.end());
    advance();

    if (!match(TokenType::DOS_PUNTOS))
    {
        error("se esperaba ':'");
        return;
    }

    attribute_value(indent, attrName);
}

void attribute_value(int indent, const string &tagName)
{
    if (current().type == TokenType::STRING ||
        current().type == TokenType::NUMBER ||
        current().type == TokenType::PR_TRUE ||
        current().type == TokenType::PR_FALSE ||
        current().type == TokenType::PR_NULL)
    {

        out << string(indent, '\t') << "<" << tagName << ">"
            << current().lexeme << "</" << tagName << ">\n";
        advance();
    }
    else if (current().type == TokenType::L_LLAVE)
    {
        out << string(indent, '\t') << "<" << tagName << ">\n";
        object(indent + 1);
        out << string(indent, '\t') << "</" << tagName << ">\n";
    }
    else if (current().type == TokenType::L_CORCHETE)
    {
        out << string(indent, '\t') << "<" << tagName << ">\n";
        parseArray(indent + 1);
        out << string(indent, '\t') << "</" << tagName << ">\n";
    }
    else
        error("valor de atributo inválido");
}

void parseArray(int indent, const string &tagName)
{
    if (!match(TokenType::L_CORCHETE))
    {
        error("se esperaba '['");
        return;
    }
    if (match(TokenType::R_CORCHETE))
        return;

    while (true)
    {
        out << string(indent, '\t') << "<item>\n";
        element(indent + 1);
        out << string(indent, '\t') << "</item>\n";
        if (!match(TokenType::COMA))
            break;
    }

    if (!match(TokenType::R_CORCHETE))
        error("se esperaba ']' al final del arreglo");
}

int main()
{
    tokens = tokenize("fuente.txt");

    out.open("output.xml", ios::out | ios::trunc);
    if (!out.is_open())
    {
        cerr << "No se pudo crear el archivo de salida output.xml" << endl;
        return 1;
    }

    cout << "Traduciendo fuente..." << endl;
    json();
    out.close();

    if (current().type == TokenType::EOF_TOKEN)
        cout << " Traduccion completada. Archivo generado: output.xml" << endl;
    else
        cout << "  Traduccion finalizada con errores." << endl;

    return 0;
}
