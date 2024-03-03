# Simple Shell

## Description
Simple Shell is a basic UNIX command-line interpreter written in C. It provides a minimalistic shell environment where users can execute commands and built-in functionalities.

## Features
- Prompt for commands
- Execute simple commands
- Handle built-in commands (exit, setenv, unsetenv, cd, alias)
- Handle command line arguments
- Handle environment variables
- Handle logical operators (;, &&, ||)

## Requirements
- Ubuntu 20.04 LTS
- GCC compiler

## Installation
1. Clone the repository:
git clone https://github.com/Tshego07/simple_shell.git
2. Navigate to the project directory:
cd simple_shell

## Usage
Run the shell executable:
./hsh
Enter commands at the prompt and press Enter to execute.

Example:
$ ./hsh
$ ls -l
total 100
-rwxr-xr-x 1 user user 10794 Jan 25 10:08 hsh
-rw-r--r-- 1 user user 1234 Jan 25 10:08 shell.c
...
$ exit

## Built-in Commands
- `exit`: Exit the shell
- `setenv`: Set or modify an environment variable
- `unsetenv`: Remove an environment variable
- `cd`: Change the current directory
- `alias`: Define or display aliases

## Authors
- Tshegofatso Moloi

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
