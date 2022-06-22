Assignment 2 information
by: Kaspian Thoft-Christensen

How to compile code:
gcc assignnment2.c

Purpose: The purpose of this assignment was to create a program which analysed a C file and then printed out which section of memory each variable was assigned to. There are some C files included in the folder called "Example C files" to test with. This program assumes the code is formatted for a regular C file, and will still accept C files with errors, however there is no guarentee it will run properly in that situation.



How the problem was solved:
	The code takes in path to a file. If the file is invalid then it prints a statement and exits. It looks a the file one line at a time. Each line is then prepared so it can be tokenized. Then looks at each string in the prepared line, separated by a space, to determine if there are any patterns which match valid function and variable declarations. If it finds a variable, it assigns a node its name, which function it is in, its location in memory, its type, the line number it is on, and its size in memory. Then that variable node gets put into a linked list of all the previously assigned variables. If it finds a function, it assigns a node its name, the number of lines it takes up, and its size in memory. Then that function node gets put into a linked list of all the previous functions. If there is a string literal, then it is assigned to ROData. If there is an allocated variable, then it is added as a new variable to the heap. After is it finished reading through the file, it prints the information gathered in the linked lists out according to the assignment 2 rubric.
	
Function Documentation:
	Stuctures:
		Variable:
			A node in a linked list which stores the name of the variable, the name of the function it is located in, the location in memory it is stored in, the name of its type, the size of the variable, the line number it is stored at, and the pointer to the next node in the list.
		Function:
			A node in a linked list which stores the name of the function, the number of lines in the function, the number of variables in the function, and the pointer to the next node in the list.
	Important variables:
		In the readFile function, the variables var_list and func_list are repectively, heads to a Variable linked list and a Function linked list. They store all of the variables and functions used in the file.
	Functions: 
		char strContains(const char* str,char c):
			Returns true if string str contains char c. Used in readFile().
		char * replaceChar(char*str,char c_1,char c_2):
			Replaces all instances of of c_1 in str with c_2. Returns pointer to the string str.
		char* changeStrSize(char* str, int new_size):
			Reallocates a space in the heap for str with new_size number of characters. Returns a pointer to the newly allocated space.
		char* removeChars(char* str,char c):
			Allocates a space in the heap for a new string which is a copy of str but without any occurences of char c. Returns a pointer to the newly allocated space. Used in calculateAllocSize
		Variable *newVar(const char * name,const char * location,const char * type,int line_num):
			Allocates, assigns to memory, and returns a new pointer to a Variable.
		Variable *addVar(Variable* head, Variable* node):
			Adds the Variable node to the head of the linked list. Returns pointer to new head
		Variable *findVar(Variable* head,char * name, char * func):
			Returns the first instance of a node in the linked list at head with the matching name and function location.
		void freeVar(Variable* head):
			Frees all the values in a Variable linked list starting at head.
		Function *newFunc(Function *func,char * name):
			Allocates, assigns to memory, and returns a new pointer to a Function.
		Function *addFunc(Function* head, Function* node):
			Adds the Function node to the head of the linked list. Returns pointer to new head
		void freeFunc(Function* head):
			Frees all the values in a Function linked list starting at head.
		char* findSize(const char *type):
			Takes in string type, which can only be of value "int", "float", "char", "int*", "float*", "char*", "int[]", "float[]", "char[]" and returns a string containing the size of that data type. If it is an array with something a value within the brackets, it returns the size of a single value in the array, multiplied by the string withing the brackets.
		char * calculateAllocSize(const char *line):
			Takes in string line (which starts with either malloc or calloc), determines the size allocated and returns the string with that size. 
		int maxLineSize(FILE *current_file):
			Takes in the current file and reads it to determine the size of longest line. Returns that value.
		char recognizeType(const char* str):
			Returns 1 if string is "int", "float" or "char", else return 0.
		int determineMemLocation(char* location)
			Returns the value STATIC_DATA if the location of a var is global, STACK otherwise
		void prepareTokenize(const char* input, char * output,int input_size):
			Takes in input, and assigns all values to out except, it adds a space before and after certain characters to make it easier to parse the string. All // which appear in the input are printed as a \0 in the output. 
		void printVarMemLocation(Variable* var_head, int mem_location):
			Helper function for printInfo() that prints all the information for variables in a given mem_location.
		void printInfo(Variable* var_list, Function* func_list,int file_lines,char* filename):
			Prints out all of the Variables, Functions, and stats according to the assignment 2 rubric.
		void readFile(char* file_path):
			Reads in data from the file at file_path and accumulates all the variables and functions in it for printing later. The function relies heavily on assuming the code is properly formatted (for example, it assumes that the name string comes after the type string, unless it is '*'). More details can be found in the comments of assignnment2.c. 
		int main(int argc, char** argv):
			Reads in file name from argv[1]. If there are no arguments, then it returns an error.
Test files:
	If have decided to test using a file I created called testtext.c. testtext.c tests: in-line comments, malloc and calloc, allocating memory on a different line and the same line as the variable initialization, casting, nested square brackets, and assigning string literals using char* and char[],
	
Code for testtext.c:
----------------------------

int * x_1;
char *hello;
char * nice = "not allowed";
char what[] = "notallowed";
//hasd toto camepe;
//float y = 4;
int no = (int) 2.0;
float cannot[never];
int fun1(int input, char* output)
{
	int* i;
	int x,y,z;
	return 1;
}
int main(){
	char * never = "going t";
	int i = 5;
	int list[what[0]];
	char *c;
	float your, mine, is;
	fun1(i,&c);
	c = calloc(sizeof(char)       , 6) ; // don't look int at this
	char * malloc_test = (char*) malloc(sizeof(char)*i);
	return 0;
}

Corresponding output:
----------------------------
>>> Memory Model Layout <<<

***  exec // text ***
   testtext.c

### ROData ###       scope type  size
   nice   global   char*   11*sizeof(char)
   what   global   char[]   10*sizeof(char)
   never   main   char*   7*sizeof(char)

### static data ###
   x_1   global   int*   8
   hello   global   char*   8
   no   global   int   4
   cannot   global   float[never]   never*sizeof(float)

### heap ###
   *c   main   char   sizeof(char)*6
   *malloc_test   main   char   sizeof(char)*i

####################
### unused space ###
####################

### stack ###
   input   fun1   int   4
   output   fun1   char*   8
   i   fun1   int*   8
   x   fun1   int   4
   y   fun1   int   4
   z   fun1   int   4
   i   main   int   4
   list   main   int[what[0]]   what[0]*sizeof(int)
   c   main   char*   8
   your   main   float   4
   mine   main   float   4
   is   main   float   4
   malloc_test   main   char*   8

**** STATS ****
  - Total number of lines in the file: 25
  - Total number of functions: 2
    fun1, main
  - Total number of lines per functions:
    fun1: 3
    main: 9
  - Total number of variables per functions:
    fun1: 6
    main: 8
//////////////////////////////
