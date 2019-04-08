# CS 170 Project 1: Simple Shell

Submission from: Scott Chow

Perm: 4302667

## Basic structure

Overall, the shell follows the outline of the functionality of the shell outlined in the slides for the second lecture. The interesting implementation comes in when I was figuring out how to parse the commands coming in from the shell.

### Tokenizer

Given the stick nature of tokenizing and the inconsistencies that often arise with variable white space (i.e: whitespace doesn't matter in `ls|cat` but does for `ls -a`), I decided to first tokenize the input string by hand. There are six types of tokens as listed below:

| Token | Represents |
|:------:|:----------:|
| `Word` | any alphanumeric sequence: programs, flags, input and output files|
| `LT` | `<` |
| `GT` | `>` |
| `Pipe` | `|` |
| `Amp` | `&` |
| `EOI` | end of input, for easier verification/avoiding seg-faults |

The goal of the tokenizer was to remove any of the problems with dealing with whitespaces and to make parsing (the next step) as easy as possible.

### Parser

The parser was implemented as a **recursive descent** parser, trading computational efficiency for ease of development. The grammer follows roughly as follows

| Symbol | Rule |
| ------ | ---- |
| `Input`| `CommandSeq , CommandSeq &` |
| `CommandSeq` | `Command , Command | CommandSeq` |  
| `Command` | `(Word)* (< Word)+ (Word)* (> Word)+ (Word)*` |

Note: the above rules aren't exactly the grammar I'm allowing but serve as a rough sketch as to what I'm allowing.

### Execution

