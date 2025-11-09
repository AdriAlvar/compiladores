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


## Compilación y ejecución
### En Linux:
```bash
g++ lexer.cpp -o lexer
./lexer fuente.txt salida.txt

### En Windows:
g++ lexer.cpp -o lexer.exe
lexer.exe fuente.txt salida.txt



##  Tarea 2 – Análisis Sintáctico Descendente

###  Descripción
Implementación de un **analizador sintáctico descendente recursivo (LL(1))** para el mismo lenguaje JSON simplificado utilizado en la Tarea 1.  
Este programa valida si el contenido del archivo `fuente.txt` cumple la estructura sintáctica definida por la gramática BNF del lenguaje JSON.

###  Funcionamiento
1. El programa reutiliza un analizador léxico interno que genera una lista de tokens.
2. Aplica reglas sintácticas recursivas para reconocer:
   - Objetos `{ ... }`
   - Arreglos `[ ... ]`
   - Atributos `"clave": valor`
   - Valores válidos (`string`, `number`, `true`, `false`, `null`).
3. Si encuentra errores, los informa por consola indicando el lexema y el tipo de error.
4. Al finalizar, muestra si la fuente es **sintácticamente correcta**.

###  Archivos
| Archivo | Descripción |
|----------|--------------|
| `parser.cpp` | Analizador sintáctico descendente (Tarea 2) |
| `fuente.txt` | Archivo JSON de entrada |
| `lexer.cpp` | (opcional) Analizador léxico de la Tarea 1 usado como base |

### Compilación y ejecución
#### En Windows
```bash
g++ parser.cpp -o parser.exe
parser.exe

#### En linux
g++ parser.cpp -o parser
./parser


##  Tarea 3 – Traducción Dirigida por Sintaxis (JSON → XML)

###  Descripción
Implementación de un **traductor sintáctico** que convierte un archivo escrito en **JSON simplificado** (`fuente.txt`) a su equivalente en **XML** (`output.xml`).  
El traductor extiende el analizador sintáctico recursivo de la Tarea 2, incorporando **acciones semánticas** durante el análisis para generar el archivo de salida.

---

### Funcionamiento
1. **Lee** el archivo `fuente.txt` con la estructura JSON.
2. **Analiza léxica y sintácticamente** el contenido.
3. **Genera** el archivo `output.xml` con la traducción XML correspondiente.
4. En caso de error, muestra mensajes descriptivos y continúa el análisis (modo *Panic Mode*).

---

###  Compilación y ejecución

#### En Windows
```bash
g++ traductor.cpp -o traductor.exe
traductor.exe

#### En Linux
g++ traductor.cpp -o traductor
./traductor
