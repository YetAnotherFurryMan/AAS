# Asset Assembler Script (AAS) Specification

In this document I will describe how Asset Assembler Script (later AAS) is processed and present basic instructions (the built-in ones).
Also, I will mention `use` and `import` instructions, but these are special extensions implemented in the executable (aas), not in the language library (aaslib).
For documentation of specific modules (like `console`) see other documents in this directory.

## Parser

There is no actual parser for AAS, the syntax is made in such way that makes it possible to use lexer only approach.
That means, there is no AST, only stream of tokens. The code is executed from top to bottom and the index of currently processed token is stored in a variable (Program Counter - PC).
The PC can be modified by commands, so the control-flow can be non-linear (loops, etc.) and some tokens can be considered data.

These are currently possible token types: number, string, formatted string, identifier, stack reference and label.
1. A number is a token that is a valid integer and satisfies the following regex: `-?[0-9]+`
2. A string is a collection of any bytes between two single quotes, it does not support escaping.
3. A formatted string is same as a string but surrounded by double quotes instead. It supports escaping with the slash `\`. The sequences are similar to the C's ones, but it supports only one-byte hex-encoded sequences (e.g. `\xAA` or `\x0a`).
4. An identifier is a sequence of alphanumeric characters, dots, and underscores. Every identifier is translated into a unique number, that will be important later.
5. A stack reference is a positive integer `n` that refers to `n`th element on the stack (counting from the top), it satisfies the following regex: `$[0-9]*`, where in case of `$` it is equivalent to `$0` (the top).
6. A label is actually an identifier prefixed by `@`, it is automatically mapped to its PC.

## Execution process

So, firstly, the code is tokenized and the commands are assigned to corresponding identifiers (by its index).
These two actions can be performed in any order, because if an identifier does not have an index yet, it will be created at when needed.
Then the program can be executed, the PC is created (with the value of 0) and the flow is passed to the command assigned to the first identifier.
The command can modify the state of the program (including the stack) and the PC if needed.
When the command returns, the PC is incremented and the cycle goes on. Only the identifiers can be executed, so when PC points at string, formatted string or number the program fails.
The same happens when PC is pointing at an identifier with no command assigned, for obvious reasons.

## Stack datatypes

The data types are simpler than token types, for now there are only: integer, text, and object.

| Token Type       | Data Type |
| :--------------: | :-------: |
| Number           | Integer   |
| String           | Text      |
| Formatted String | Text      |
| Identifier       | Integer   |
| Stack Reference  | _depends_ |
| Label            | Integer   |

## Built-in commands

The commands are divided into categories: stack, variables, types, math, and flow. They can take arguments from code (as following tokens) or from the stack.
The functions of some commands (like `push` or `pop`) can be easily guessed along with what data they operate on, however some have variants (one that operates with data on the stack and one that operate on both stack and source).
For this reason, a command with no suffix (like `add`) operates only on stack and a command with suffix `v` (like `addv`) takes a following token as the second operand.

Example: These two peaces of code are equivalent.

``` AAS
push 2
push 1
add
```

``` AAS
push 1
addv 2
```

Also, some flow commands may depend on the stack, so the `z` suffix means that the action will be performed only when the top of the stack is integer equal to 0.

Example: The first sample will print nothing, while the second will print "Hello World!" and a new line character.

``` AAS
use console

push 1
goto Label
printv "Hello World!\n"
@Label
```

``` AAS
use console

push 1
gotoz Label
printv "Hello World\n"
@Label
```

### Stack

`push [ANY]`

The `push` command takes one argument from code and pushes it as data to the stack.

`pop`

The `pop` command removes top-most element from the stack.

`dup`

The `dup` duplicates the top-most element on the stack.

`swap`

The `swap` changes the order of the two top-most elements on the stack.

`rot`

The `rot` rotates the three top-most elements (moves the top-most under the third).

### Variables

`set [ID] [ANY]`
`set [TXT] [ANY]`

Sets the variable (first argument) to the given value (the second one).

`get [ID]`
`get [TXT]`

Pushes the value of the given variable on the stack.

### Types

`text`

Converts whatever lies on the stack to text.

`int`

Converts whatever lies on the stack to integer (with value of 0 on error).

`isText`

Pushes 1 if the top of the stack is of type text, 0 otherwise.

`isInt`

Pushes 1 if the top-most element of the stack is of thupe integer, 0 otherwise.

`isObj`

Pushes 1 when the top of the stack is an object or 0.

`ofType [TXT]`

Pushes 1 if the top-most element of the stack is of type object ant its name is equal to given text, 0 otherwise.

### Math

`add`
`addv [INT]`

The `add` adds two operands.

`sub`
`subv [INT]`

The `sub` subtracts two operands.

`mul`
`mulv [INT]`

The `mul` multiplies two operands.

`div`
`divv [INT]`

The `div` divides two operands.

`mod`
`modv [INT]`

The `mod` returns the reminder of division of two operands on the stack.

`cat`
`catv [INT]`
`catv [TXT]`

The `cat` command concatenates operands. The supported types are text and integer, but it always returns text on the stack.

`gt`
`gtv [INT]`

Pushes 1 if the first operand is grater than the second, 0 otherwise.

`gtq`
`gtqv [INT]`

Same as `gt`, but covers the equal case.

`lt`
`ltv [INT]`

Pushes 1 if the first operand is less than the second, 0 otherwise.

`ltq`
`ltqv [INT]`

Same as `lt`, but covers the equal case.

`eq`
`eqv [ANY]`

Pushes 1 if the operands are equal to eachother, note that objects cannot be equal to anything.

`neq`
`newv [ANY]`

The very negation of `eq`.

`not`

Takes only one operand, pushes 1 if it is an integer with value 0, pushes 0 in any other case.

### Flow

`label [ID]`

Pushes the PC assigned to the label with the given identifier.

`jump [INT]`
`jumpz [INT]`

Sets the PC to given value.

`goto [ID]`
`gotoz [ID]`

Sets the PC to the one assigned to the label with the given identifier.

`call [ID]`
`callz [ID]`

Same as `goto`, but also pushes the current PC on the stack.

`ret`

Changes PC to the one on the stack (pops the value).

### Extension

`use [ID]`
`use [STR]`

The `use` command loads an external module and assigns its commands with aliases.
For example `use console` will assign both `console.print` and `print`.

`import [ID]`
`import [STR]`

The `import` command does the same as `use`, but does not assign aliases (in the example above the `print` would not be assigned).
