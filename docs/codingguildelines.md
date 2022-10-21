# Coding Guideline

This guide is still under development. Coding guideline must not be applied, yet.

**0001** This style guide is mandatory for all OFS C/C++ source and header files.

## 01 - Entity Naming

### 0101 - Names shall begin with a lower case letter, shall be lower case and words shall begin with an upper case letter.
### 0102 - Constants shall be upper case. Words shall be separated by underscore.
### 0102 - Types and **Classes** shall begin with an upper case letter, words shall begin with an upper case letter.
### 0103 - Enumerators shall begin with an upper case letter.
### 0104 - Macros shall be upper case. Words shall be separated by underscore.

## 02 - Names

### 0201 - Use sensible, descriptive names

Do not use short cryptic names or names based on internal jokes. It shall be easy to type a name without looking up how it is spelt.
Exception: Loop variables and variables with a small scope (less than 20 lines) may have short names to save space if the purpose of that variable is obvious.

### 0202 - Only use english names

It is confusing when mixing languages for names. English is the preferred language because of its spread in the software market and because most libraries used already use English.

### 0203 - Variables with a large scope shall have long names, variables with a small scope can have short names

Scratch variables used for temporary storage or indices are best kept short. A programmer reading such variables shall be able to assume that its value is not used outside a few lines of code. Common scratch variables for integers are `i`, `j`, `k`, `m`, `n` and for characters `c` and `d`.

### 0204 - Only use letters, digits and the underscore

* Names should only consist of the characters `[A-Za-z0-9]`.
* Names shall always start with a letter
* The Underscore (`_`) shall only be used in names of constants and macros.

## 03 - Indentation and Spacing

### 0301 - Indentation with is 3 spaces

To have a clean structure, every level shall be indented by 3 additional spaces, compared to the previous level.

### 0302 - Braces shall follow "Exdented Style"

* Curly braces are located on lines on their own. They have the same indentation as the statement they belong to. Statements and declarations between the braces are indented relative to the braces.

Example:

```c
void f(int a)
{
  int i;
  if (a > 0)
  {
    i = a;
  }
  else
  {
    i = a;
  }
}
class A
{
};
```

### 0303 - Loop and conditional statements shall always have brace enclosed sub-statements

The code looks more consistent if all conditional and loop statements have braces. Even if there is only a single statement after the condition or loop statement today, there might be a need for more code in the future.

### 0304 - Braces without any contents may be placed on the same line

The only time when two braces can occur on the same line is when they do not contain any code.

```c
while (...)
{}
```

### 0305 - Each statement shall be placed on a line on its own

There is no need to make code compact. Putting several statements on the same line only makes the code cryptic to read.

### 0306 - Operators

* All binary arithmetic, bitwise and assignment operators and the ternary conditional operator (`?:`) shall be surrounded by spaces
* The comma operator shall be followed by a space but not preceded
* All other operators shall not be used with spaces

### 0307 - Lines shall not exceed 120 characters

Even if your editor handles long lines, other people may have set up their editors differently. Long lines in the code may also cause problems for other programs and printers.

### 0308 - Do not use tabs

Tabs make the source code difficult to read where different programs treat the tabs differently. The same code can look very differently in different views. Avoid using tabs in your source code to avoid this problem. Use spaces instead.

## 04 - Comments

### 0401 - Use JavaDoc style comments

The comment styles `///` and `/** ... */` are used by JavaDoc, Doxygen and some other code documenting tools.

### 0402 - All comments shall be placed above the line the comment describes, indented identically

Being consistent on placement of comments removes any question on what the comment refers to.

### 0403 - Every class shall have a comment that describes its purpose

### 0404 - Every function shall have a comment that describes its purpose

## 05 - Files

### 0501 - There shall only be one externally visible class defined in each header file

Having as few declarations as possible in a header file reduces header dependencies. The header file shall have the same name as the class plus extension `hpp`.

External non-member functions that belong to the class interface may also be declared in the same header file.

### 0502 - File name shall be treated as case sensitive

### 0503 - C source files shall have extension `.c`

### 0504 - C header files shall have extension `.h`

### 0505 - C++ source files shall have extension `.cpp`

### 0506 - C++ header files shall have extension `.hpp`

### 0507 - Header files must have include guards

The include guard protects against the header file being included multiple times. Example:

```c
#ifndef FILE_H
#define FILE_H
...
#endif
```

### 0508 - The name of the macro used in the include guard shall have the same name as the file (excluding the extension) followed by the suffix `_H`

### 0509 - Header files shall be self-contained

When a header is included, there shall not be a need to include any other headers first. A simple way to make sure that a header file does not have any dependencies is to include it first in the corresponding source file.

Example:

```c
/* foobar.h */

#include "foobar.h"
#include <stdio.h>

...
```

### 0510 - System header files shall be included with `<>` and project headers with `""`

### 0511 - Put #include directives at the top of files

Having all `#include` directives in one place makes it easy to find them.

### 0512 - Do not use absolute directory names in #include directives.

The directory structure may be different on other systems.

### 0513 - Each file must start with a copyright notice

### 0514 - Each file must contain a revision marker

## 06 - Declarations

### 0601 - Provide names of parameters in function declarations

Parameter names are useful to document what the parameter is used for. The parameter names shall be the same in all declarations and definitions of the function.

### 0602 - Always provide the return type explicitly

### 0603 - Use a typedef to define a pointer to a function

Pointers to functions have a strange syntax. The code becomes much clearer if you use a typedef for the pointer to function type. This typedef name can then be used to declare variables etc.

```c
double sin(double arg);
typedef double (*TrigFunc)(double arg);

/* Usage examples */
TrigFunc myFunc = sin;
void callFunc(TrigFunc callback);
TrigFunc funcTable[10];
```

### 0604 - Do not use exception specifications

Exception specifications in C++ are not as useful as they look. The compiler does not make the code more efficient. On the contrary, the compiler has to insert code to check that called functions do not violate the specified exception specification at runtime.

### 0605 - Declare inherited functions virtual

An inherited function is implicitly virtual if it is declared virtual in the base class. Repeat the virtual keyword when declaring an inherited function in a derived class to make it clear that this function is virtual.

### 0606 - Do not use global variables

Use singleton objects instead. Global variables are initialised when the program starts whether it will be used or not. A singleton object is only initialised when the object is used the first time.

If global variables are using other global variables for their initialisation there may be a problem if the dependent variables are not initialised yet. The initialisation order of global variables in
different object files is not defined. Singleton objects do not have this problem as the dependent object will be initialised when it is used. However, watch out for cyclic dependencies in singleton object initialisations.

### 0607 - Do not use global using declarations and using directives in headers

Bringing in names from a namespace to the global namespace may cause conflicts with other headers. The author of a header does not know in which context the header is used and should avoid polluting the global namespace. Instead, only use using declarations in the source files.

### 0608 - The parts of a class definition must be in this order

* `public`
* `protected`
* `private`

This makes it easy to read the class definition as the public interface is of interest to most readers.

### 0609 - The contents of each part must be in this order

* constants (static const) 
* type definitions 
* instance fields
* Constructors
* destructors
* operators
* getters/setters
* methods
* static fields
* static methods

### 0610 - Declare class data private

Classes shall encapsulate their data and only provide access to this data by member functions to ensure that data in class objects are consistent. The exception to the rule is C type struct that only contains data members.

### 0611 - Only use the keyword struct for C-style structs

### 0612 - Initialize all fields in the initialization lists of all constructors

Exceptions:

* Classes with a default constructor
* Structs and Arrays

### 0613 - In the destructor free all memory allocated in the constructors

## 07 - Statements

### 0701 - Never use gotos

Gotos break structured coding.

### 0702 - All switch statements shall have a default label

Even if there is no action for the default label, it shall be included to show that the programmer has considered values not covered by case labels. If the case labels cover all possibilities, it is useful to put an assertion there to document the fact that it is impossible to get here. An assertion also protects from a future situation where a new possibility is introduced by mistake.

## 08 - Other Typographical Issues

### 0801 - Do not use literal numbers other than `0` and `1`

Use constants instead of literal numbers to make the code consistent and easy to maintain. The name of the constant is also used to document the purpose of the number.

### 0802 - Do not rely on implicit conversion to bool in conditions

```c
if (ptr)         // wrong
if (ptr != NULL) // ok
```

### 0803 - Use the new cast operators

Use `dynamic_cast`, `const_cast`, `reinterpret_cast` and `static_cast` instead of the traditional C cast notation. These document better what is being performed.
