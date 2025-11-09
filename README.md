# Analizador Léxico - JSON Simplificado

## Descripción
Implementación de un analizador léxico en **C++** que procesa un archivo con formato **JSON simplificado**, identificando los tokens válidos y generando un archivo de salida con la secuencia de componentes léxicos por línea.

El programa lee por defecto `fuente.txt` y genera `salida.txt`.

---

## Archivos incluidos
- `lexer.cpp` → código fuente principal  
- `fuente.txt` → archivo de entrada (JSON de ejemplo)  
- `salida.txt` → salida generada con los tokens reconocidos  
- `lexer.exe` → ejecutable compilado (Windows)

---

## Tokens reconocidos
| Token | Ejemplo / Símbolo |
|-------|--------------------|
| `L_LLAVE` | `{` |
| `R_LLAVE` | `}` |
| `L_CORCHETE` | `[` |
| `R_CORCHETE` | `]` |
| `COMA` | `,` |
| `DOS_PUNTOS` | `:` |
| `STRING` | `"texto"` |
| `NUMBER` | `123`, `12.5`, `1e3` |
| `PR_TRUE` | `true` / `TRUE` |
| `PR_FALSE` | `false` / `FALSE` |
| `PR_NULL` | `null` / `NULL` |

---

## Compilación y ejecución
### En Linux:
```bash
g++ lexer.cpp -o lexer
./lexer fuente.txt salida.txt

### En Windows:
g++ lexer.cpp -o lexer.exe
lexer.exe fuente.txt salida.txt
