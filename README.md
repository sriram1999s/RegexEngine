# RegexEngine

Matches texts with regular expressions. Similar to re.search() in python.

## Description

  - Takes text and a regular expression as input
  - If matched, returns ```<1, start_index, end_index>```
  - If not matched, returns ```<0>```

## Usage

### Compilation

```sh
gcc re_engine.c -o re_engine
```

### Execution
Example of an input file is given below:
```txt
Hello there. This is a sample text for the input. abcd9
12
the
abc[a-z][0-9]
9+a+
.*e.*e
.*
.*?
\w+e.*e
\w+e\w*e
\w*s.*\d
[a-zA-zBBCD9]*?\w*x.*\d
3?\d?
Hello.*is\w*?
```
```sh
./re_engine < input_files/sample_inp
```
