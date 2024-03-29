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

Execution occurs recursively down the `Parser::Input` data structure, with calls to `executeInput`, `executeCommandSeq`, and `executeCommand`. What I'll draw attention to is the manner in which pipes are setup and maintained across this recursive approach. I manage this by having `executeCommandSeq` pass the redirected input from one command to the next (modeling pipe behavior), while allowing `executeCommand` to take arguments for redirecting input and output accordingly.

### Catching Signals

At the moment this simple shell only deals with `SIGINT` from itself and its child processes. It does so via `signal(2)`, for which the signal handler function simply prints the `shell:` prompt again, while also preventing the main run loop from printing it a second time.