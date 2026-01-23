# 42SH Project EPITA

```
$$\   $$\  $$$$$$\            $$\       
$$ |  $$ |$$  __$$\           $$ |      
$$ |  $$ |\__/  $$ | $$$$$$$\ $$$$$$$\  
$$$$$$$$ | $$$$$$  |$$  _____|$$  __$$\ 
\_____$$ |$$  ____/ \$$$$$$\  $$ |  $$ |
      $$ |$$ |       \____$$\ $$ |  $$ |
      $$ |$$$$$$$$\ $$$$$$$  |$$ |  $$ |
      \__|\________|\_______/ \__|  \__|
                                        
                                        
                                        
```

## Description
42sh is a posix shell interpreter. This project was made by a group of 4 student in EPITA.

## Usage
```
./src/42sh [--pretty-print] [ -c <string> | <filename> ]
```
Examples:
```bash
./src/42sh -c 'echo This is an example' # String
./src/42sh file.txt # File
./src/42sh # Stdin
```
## Installation
Compile 42sh:
```bash
autoreconf -vif
./configure
make
```

Install 42sh:
```bash
make install
```

## Contributors
Group members:
[Arnaud Bellicha](mailto:arnaud.bellicha@epita.fr),
[Clement Bisiaux](mailto:clement.bisiaux@epita.fr),
[Louis Moretti](mailto:louis.moretti@epita.fr),
[Yassine Tlich](mailto:yassine.tlich@epita.fr)
