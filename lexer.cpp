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
    PR_NULL
};

static const unordered_map<TokenType, string> TOKEN_TO_STR = {
    {TokenType::L_CORCHETE, "L_CORCHETE"},
    {TokenType::R_CORCHETE, "R_CORCHETE"},
    {TokenType::L_LLAVE, "L_LLAVE"},
    {TokenType::R_LLAVE, "R_LLAVE"},
    {TokenType::COMA, "COMA"},
    {TokenType::DOS_PUNTOS, "DOS_PUNTOS"},
    {TokenType::STRING, "STRING"},
    {TokenType::NUMBER, "NUMBER"},
    {TokenType::PR_TRUE, "PR_TRUE"},
    {TokenType::PR_FALSE, "PR_FALSE"},
    {TokenType::PR_NULL, "PR_NULL"}};

struct LexResult
{
    vector<string> tokensForLine;
    string error; 
};

static inline bool isSpace(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }
static inline bool isDigit(char c) { return c >= '0' && c <= '9'; }
static inline bool isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }

bool scanString(const string &s, size_t &i)
{
    i++; 
    while (i < s.size())
    {
        if (s[i] == '"')
        {
            i++;
            return true;
        }
        
        i++;
    }
    return false; 
}

bool scanNumber(const string &s, size_t &i)
{
    size_t start = i;
    if (!isDigit(s[i]))
        return false;
    while (i < s.size() && isDigit(s[i]))
        i++;
    if (i < s.size() && s[i] == '.')
    {
        size_t j = i + 1;
        if (j < s.size() && isDigit(s[j]))
        {
            i = j + 1;
            while (i < s.size() && isDigit(s[i]))
                i++;
        }
        else
            return false;
    }
    if (i < s.size() && (s[i] == 'e' || s[i] == 'E'))
    {
        size_t j = i + 1;
        if (j < s.size() && (s[j] == '+' || s[j] == '-'))
            j++;
        if (j < s.size() && isDigit(s[j]))
        {
            j++;
            while (j < s.size() && isDigit(s[j]))
                j++;
            i = j;
        }
        else
            return false;
    }
    return i > start;
}

optional<TokenType> scanKeyword(const string &word)
{
    if (word == "true" || word == "TRUE")
        return TokenType::PR_TRUE;
    if (word == "false" || word == "FALSE")
        return TokenType::PR_FALSE;
    if (word == "null" || word == "NULL")
        return TokenType::PR_NULL;
    return nullopt;
}

LexResult tokenizeLine(const string &line)
{
    LexResult res;
    size_t i = 0, n = line.size();

    while (i < n)
    {
        while (i < n && isSpace(line[i]))
            i++;
        if (i >= n)
            break;

        char c = line[i];

        if (c == '[')
        {
            res.tokensForLine.push_back(TOKEN_TO_STR.at(TokenType::L_CORCHETE));
            i++;
            continue;
        }
        if (c == ']')
        {
            res.tokensForLine.push_back(TOKEN_TO_STR.at(TokenType::R_CORCHETE));
            i++;
            continue;
        }
        if (c == '{')
        {
            res.tokensForLine.push_back(TOKEN_TO_STR.at(TokenType::L_LLAVE));
            i++;
            continue;
        }
        if (c == '}')
        {
            res.tokensForLine.push_back(TOKEN_TO_STR.at(TokenType::R_LLAVE));
            i++;
            continue;
        }
        if (c == ',')
        {
            res.tokensForLine.push_back(TOKEN_TO_STR.at(TokenType::COMA));
            i++;
            continue;
        }
        if (c == ':')
        {
            res.tokensForLine.push_back(TOKEN_TO_STR.at(TokenType::DOS_PUNTOS));
            i++;
            continue;
        }

        if (c == '"')
        {
            size_t startCol = i;
            bool ok = scanString(line, i);
            if (!ok)
            {
                res.error = "cadena sin cierre (comilla faltante) cerca de la columna " + to_string(startCol + 1);
                return res;
            }
            res.tokensForLine.push_back(TOKEN_TO_STR.at(TokenType::STRING));
            continue;
        }

        if (isDigit(c))
        {
            size_t save = i;
            if (scanNumber(line, i))
            {
                res.tokensForLine.push_back(TOKEN_TO_STR.at(TokenType::NUMBER));
                continue;
            }
            else
            {
                while (i < n && (isDigit(line[i]) || line[i] == '.' || line[i] == 'e' || line[i] == 'E' || line[i] == '+' || line[i] == '-'))
                    i++;
                res.error = "número mal formado cerca de la columna " + to_string(save + 1);
                return res;
            }
        }

        if (isAlpha(c))
        {
            size_t j = i;
            while (j < n && isAlpha(line[j]))
                j++;
            string word = line.substr(i, j - i);
            auto kw = scanKeyword(word);
            if (kw.has_value())
            {
                res.tokensForLine.push_back(TOKEN_TO_STR.at(*kw));
                i = j;
                continue;
            }
            else
            {
                res.error = "lexema no reconocido '" + word + "' cerca de la columna " + to_string(i + 1);
                return res;
            }
        }

        res.error = string("carácter no válido '") + c + "' cerca de la columna " + to_string(i + 1);
        return res;
    }

    return res; // sin error
}

int main(int argc, char **argv)
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string inPath = (argc >= 2 ? argv[1] : "fuente.txt");
    string outPath = (argc >= 3 ? argv[2] : "salida.txt");

    ifstream in(inPath);
    if (!in)
    {
        cerr << "No se pudo abrir el archivo de entrada: " << inPath << "\n";
        cerr << "Uso: " << (argc ? argv[0] : "lexer") << " <fuente.txt> <salida.txt>\n";
        return 1;
    }
    ofstream out(outPath);
    if (!out)
    {
        cerr << "No se pudo abrir el archivo de salida: " << outPath << "\n";
        return 1;
    }

    string line;
    size_t lineNo = 0;

    int indentLevel = 0;      
    vector<string> prevTokens;

    auto printIndent = [&](int lvl, bool hanging)
    {
        for (int i = 0; i < lvl; ++i)
            out << '\t'; 
        if (hanging)
            out << "      "; 
    };

    while (getline(in, line))
    {
        lineNo++;
        LexResult r = tokenizeLine(line);

        
        bool beginsWithCloser = false;
        if (!r.tokensForLine.empty())
        {
            const string &first = r.tokensForLine.front();
            if (first == "R_LLAVE" || first == "R_CORCHETE")
                beginsWithCloser = true;
        }
        if (beginsWithCloser && indentLevel > 0)
            indentLevel--;

       
        bool prevWasAttrArrayOpen = false;
        if (prevTokens.size() >= 3)
        {
            if (prevTokens[0] == "STRING" &&
                prevTokens[1] == "DOS_PUNTOS" &&
                prevTokens.back() == "L_CORCHETE")
            {
                prevWasAttrArrayOpen = true;
            }
        }


        bool hanging = prevWasAttrArrayOpen && !beginsWithCloser &&
                       !r.tokensForLine.empty() &&
                       r.tokensForLine.front() == "L_LLAVE";

        if (!r.error.empty())
        {
            printIndent(indentLevel, /*hanging=*/false);
            out << "ERROR en línea " << lineNo << ": " << r.error << "\n";
            prevTokens.clear();
            continue;
        }

        
        printIndent(indentLevel, hanging);

        if (r.tokensForLine.empty())
        {
            out << "\n";
        }
        else
        {
            for (size_t k = 0; k < r.tokensForLine.size(); ++k)
            {
                if (k)
                    out << ' ';
                out << r.tokensForLine[k];
            }
            out << "\n";
        }

      
        int delta = 0;
        for (const auto &tk : r.tokensForLine)
        {
            if (tk == "L_LLAVE" || tk == "L_CORCHETE")
                delta++;
            else if (tk == "R_LLAVE" || tk == "R_CORCHETE")
                delta--;
        }
        indentLevel += delta;
        if (indentLevel < 0)
            indentLevel = 0;

        prevTokens = r.tokensForLine; 
    }

    return 0;
}
