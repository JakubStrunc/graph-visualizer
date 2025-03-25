# Graph Visualizer

This project is a portable console application developed in **C** for generating visualizations of mathematical functions. It was created as a semester project at the University of West Bohemia.

## About the Project
The goal of this project is to visualize mathematical functions in PostScript format. The program reads a function from the command line and outputs a graph into a specified PostScript file. It supports basic mathematical expressions, including functions like sine, cosine, logarithm, and operations such as addition, subtraction, multiplication, division, and exponentiation. Users can optionally define the domain and range of the graph.

## How to Compile and Run
### Linux
Make sure you have `gcc` and `make` installed. Then run:
```bash
make
```

### Windows
Make sure `gcc` and `MinGW` are installed. Then run:
```bash
mingw32-make -f makefile.win
```

### Running the Program
```bash
graph.exe "sin(x ^ 2 ) * cos(x )" output.ps
```
Optionally, you can define a custom domain and range:
```bash
graph.exe "sin(x ^ 2 ) * cos(x )" output.ps -10:10:-5:5
```
If not specified, the default range is `x ∈ [-10;10]` and `y ∈ [-10;10]`.

### Example Output
Running the program with the following input:
```bash
graph.exe "sin(x ^ 2 ) * cos(x )" output.ps -10:10:-5:5
```
Generates a graph that combines oscillatory behavior from both sine and cosine functions:
![image](https://github.com/user-attachments/assets/ddfda2f7-444b-4200-8b0b-a90dc057727c)



## Supported Input
### Functions
- `sin`, `cos`, `tan`, `asin`, `acos`, `atan`
- `sinh`, `cosh`, `tanh`, `ln`, `log`, `sqrt`, `abs`, `exp`

### Operators
- `+`, `-`, `*`, `/`, `^`, unary minus

### Constants
- `pi` – π
- `e` – Euler's number
- `x` – independent variable

## Algorithm Highlights
### Reverse Polish Notation (RPN)
The program internally converts expressions to **Reverse Polish Notation** for evaluation. Tokens are parsed, categorized (number, variable, operator, function), and processed using a stack.

Example: for the expression `sin(x^2)*cos(x)`, RPN conversion and evaluation go through stages like pushing operands, resolving operator precedence, handling parentheses, and applying functions sequentially.

### Shunting Yard Algorithm
Expressions are parsed using the **Shunting Yard Algorithm** to handle operator precedence, associativity, and parentheses. Functions are stored character-by-character and ended using a special delimiter (`$`) to distinguish full function names.
