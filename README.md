# myOwnDB
trying to understand how DB -simple one- were implemented, so let's build one ...


note:
i'm following the Connor Stack aka cstack tutorial for building his own DB 

repo link -> https://github.com/cstack/db_tutorial?tab=readme-ov-file 

the link of tutorial -> https://cstack.github.io/db_tutorial/

## What I have learned in this series...
-- part1 ->
- The SQLite architecture consists of several components: SQL compiler (front-end), VM, backend (B-tree, pager, and OS interface).
- Each component does some work and passes it to the next layer.
- The input to the front-end is a SQL query. The output is SQLite virtual machine bytecode.
- It starts a read-execute-print loop, which takes an input buffer (an array of characters in the end), the length of that buffer, and the length of incoming input.
- The first command is ".exit", which terminates the program. 


-- part2 ->
- We start by making an SQL compiler that parses a string and outputs an internal representation called bytecode.
- Instead of comparing commands like ".exit", we separate the commands into two types: one for meta commands and the other for insertion, selection, etc.
- If the command starts with ".", it's a meta command; otherwise, we switch for insertion or selection. If the statement is correct, return the state of success for this statement; otherwise, return unrecognized.
- In the end, compilation is about filtering the commands and checking if they're typed correctly or not.
