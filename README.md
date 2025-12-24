ELEC2645 Unit 2 Project - Password Strength Checker

This is the basic code for a command line application which I used for the Unit 2 project.



### 1 Run code

We can build the code as we have been using
`gcc main.c funcs.c -o main.out -lm` (the `-lm` is required to link the math library). 
We can also use `make -B` to force a rebuild using the provided `Makefile`.
Then run the code with `./main.out`



### What the Password checker essencially does
- User Enters password
- It Computes score and feedback (`compute_password_score`, `print_password_feedback`)
- Show rule checks:
  - minimum length (`MIN_LENGTH`)
  - character types checks is the passowrd inserted includes lower/upper/digit/special
  - checks if it is a top-20 common password 
  - checks the simple sequence 
- Clears the password then exits

### Files
- `main.c` — menu loop, input handling, calls project functions
- `funcs.c` — password logic and scoring
- `funcs.h` — prototypes and constants (`MAX_LENGTH`, `MIN_LENGTH`)

### Build & run
The `test.sh` script is provided to check that your code compiles correctly. This is what the autograder will use to check the submission. 
It can run with `bash test.sh` or `./test.sh` or just `make test`. 


