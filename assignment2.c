#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STATIC_DATA (1)
#define RODATA (2)
#define HEAP (3)
#define STACK (4)

#define MEM_LOCATION_SIZE (12)
#define TYPE_SIZE (8)

char strContains(const char* str,char c){ //works for all chars except '\0'
	int i = 0;
	while(str[i]!='\0'){
		if(str[i]==c){
			return 1;
		}
		i++;
	}
	return 0;
}
char * replaceChar(char*str,char c_1,char c_2){//replace all instances of c_1 with c_2
	int i = 0;
	while(str[i]!='\0'){
		if(str[i]==c_1){
			str[i]=c_2;
		}
		i++;
	}
	return str;
}
char* changeStrSize(char* str, int new_size){
	char* new_str = malloc(sizeof(char)*new_size);
	strcpy(new_str,str);
	free(str);
	return new_str;
}
char* removeChars(char* str,char c){ //removes all instances of char c. Used in calculateAllocSize
	int i = 0;
	int j = 0;
	char *new_str = malloc(sizeof(char)*strlen(str));
	while(str[i]!='\0'){
		if(str[i]!= c){
			new_str[j]=str[i];
			j++;
		}
		i++;
	}
	new_str[++j]='\0';
	return new_str;
}

typedef struct var {
	char *name;
	char *location;
	int mem_location;
	char *type;
	char *size;
	int line_num;
	struct var *next;
} Variable;
Variable *newVar(const char * name,const char * location,const char * type,int line_num){
	Variable *temp_var = malloc(sizeof(Variable));
	temp_var->name = malloc(sizeof(char)*strlen(name));
	strcpy(temp_var->name,name);
	temp_var->location = malloc(sizeof(char)*strlen(location));
	strcpy(temp_var->location,location);
	temp_var->mem_location = 0;
	temp_var->type = malloc(sizeof(char)*strlen(type));
	strcpy(temp_var->type,type);
	temp_var->line_num = line_num;
	temp_var->size = NULL;
	temp_var->next = NULL;
	return temp_var;
}
Variable *addVar(Variable* head, Variable* node){
	if(head==NULL){
		head = node;
	}
	else{ 
		head->next = addVar(head->next,node);
	}
	return head;
}
Variable *findVar(Variable* head,char * name, char * func){
	if(head == NULL){
		return NULL;
	}
	else if(strcmp(head->name,name)==0 && strcmp(head->location,func)==0){
		return head;
	}
	else{
		return findVar(head->next,name,func);
	}
}
void freeVar(Variable* head){
	if(head!= NULL){
		freeVar(head->next);
		free(head->next);
		free(head->name);
		free(head->location);
		free(head->type);
		free(head->size);
	}
	return;
}

typedef struct func {
	char *name;
	int lines;
	int vars;
	struct func *next;
} Function;
Function *newFunc(Function *func,char * name){
	Function *temp_func = malloc(sizeof(Function));
	temp_func->name = malloc(sizeof(char)*strlen(name));
	strcpy(temp_func->name,name);
	temp_func->lines = 0; //set as default
	temp_func->vars = 0;
	temp_func->next = NULL;
	func = temp_func;
	return func;
}
Function *addFunc(Function* head, Function* node){
	if(head==NULL){
		head = node;
	}
	else{ 
		head->next = addFunc(head->next,node);
	}
	return head;
}
void freeFunc(Function* head){
	if(head!= NULL){
		freeFunc(head->next);
		free(head->next);
		free(head->name);
	}
	return;
}
char* findSize(const char *type){ 
	/*
	type can only be of the value, int, float, char, int*, float*, char*, int[], float[], char[].
	they will be formatted this way before findSize is called.
	Pointers to pointers and multidimentional arrays will just be considered pointers (they have the same value)
	*/
	char *output = malloc(sizeof(char)*5);
	if(strcmp("float",type)==0){
		sprintf(output,"%ld",sizeof(float));
	}
	else if(strcmp("char",type)==0){
		sprintf(output,"%ld",sizeof(char));
	}
	else if(strcmp("int",type)==0){
		sprintf(output,"%ld",sizeof(int));
	}
	else if(strncmp("int*",type,4)==0||strncmp("float*",type,6)==0||strncmp("char*",type,5)==0||strncmp("int[]",type,5)==0||strncmp("float[]",type,7)==0||strncmp("char[]",type,6)==0)
		sprintf(output,"%ld",sizeof(int*)); //these are all identical since they are pointers
	else if((strncmp("int[",type,4)==0 && type[4]!=']')||(strncmp("float[",type,6)==0 && type[6]!=']')||(strncmp("char[",type,5)==0 && type[5]!=']')){		
		char * first_value_pointer = (strchr(type,'[')+1);
		int first = strlen(first_value_pointer);
		int last = strlen(strrchr(type,']'));
		char * pre_string;
		if(strncmp("int[",type,4)==0)
			pre_string = "*sizeof(int)";
		else if(strncmp("float[",type,6)==0)
			pre_string = "*sizeof(float)";
		else if(strncmp("char[",type,5)==0)
			pre_string = "*sizeof(char)";
		free(output);
		output = malloc(sizeof(char)*(first-last+strlen(pre_string)+1));
		strncpy(output,first_value_pointer,first-last);
		strcat(output,pre_string);
	}
	return output;
}
char * calculateAllocSize(const char *line){// helper function in readFile used to determine the size of an allocated  
	char * output;
	char * first_value_pointer = (strchr(line,'(')+1);
	char* no_space_first_value_pointer = removeChars(first_value_pointer,' ');	
	int first = strlen(no_space_first_value_pointer);
	int last = strlen(strrchr(no_space_first_value_pointer,')'));
	output = malloc(sizeof(char)*(first-last+1));
	
	strncpy(output,no_space_first_value_pointer,first-last);
	free(no_space_first_value_pointer);
	output = replaceChar(output,',','*');
	return output;
}
int maxLineSize(FILE *current_file){ //DONE
	//used in readFile to find the length of the longest line
	int max = 0;
	int line_count = 0;
	char c;
	c = (char)fgetc(current_file);
	while(c!=EOF){ //not sure what this does but according to documentation it's a character which signifies the end of file
		if(c=='\n'){
			if(line_count>max)
				max=line_count;
			line_count=0;
		}
		else
			line_count++;
		c = (char)fgetc(current_file);
	}
	if(line_count>max)
		max=line_count;
	return max;
}
char recognizeType(const char* str){
	if((str!=NULL && strcmp(str,"int")==0)||strcmp(str,"float")==0||strcmp(str,"char")==0||strcmp(str,"void")==0)
		return 1;
	else 
		return 0;
}
int determineMemLocation(char* location){
	if(strcmp(location,"global")==0){
		return STATIC_DATA;
	}
	return STACK;
}
void prepareTokenize(const char* input, char * output,int input_size){
	//goes through input and adds a space between certain characters helper function for read file 
	int output_counter = 0;
	int open_square_brackets = 0;
	int ignore = 0;
	for(int i = 0;i<input_size;i++){
		if(input[i] == '/' && input[i+1] == '*'){
			ignore = 1;
		}
		else if (input[i] == '*' && input[i+1] == '/'){
			ignore = 0;
		}
		else if(input[i] == '/' && input[i+1] == '/'){
			output[output_counter] = '\0';
		}
		else if(ignore==0)
		{
			if(input[i] == '['){
				if(open_square_brackets==0){
					output[output_counter++] = ' ';
					output[output_counter++] = input[i];
					output[output_counter] = ' ';	
				}
				else
					output[output_counter] = input[i];
				open_square_brackets++; //TODO look at case where there is an array in the []
			}
			else if(input[i] == ']'){
				open_square_brackets--;
				if(open_square_brackets==0){
					output[output_counter++] = ' ';
					output[output_counter++] = input[i];
					output[output_counter] = ' ';	
				}
				else
					output[output_counter] = input[i];
			}
			
			else if(open_square_brackets==0 && (input[i] == '*' || input[i] == '(' || input[i] == ')' || input[i] == ',' || input[i] == ';' || input[i] == '\t' || (input[i] == '=' && input[i+1] != '='))){
				output[output_counter++] = ' ';
				output[output_counter++] = input[i];
				output[output_counter] = ' ';
			}
			else if(open_square_brackets==0 || input[i]!=' '){
						output[output_counter] = input[i];
			}
		}
		output_counter++;
	}
	return;
}
void printVarMemLocation(Variable* var_head, int mem_location){ //helper function for printInfo
	while(var_head!=NULL){
		if(var_head->mem_location==mem_location)
			printf("   %s   %s   %s   %s\n",var_head->name,var_head->location,var_head->type, var_head->size);
		var_head = var_head->next;
	}
	return;
}
void printInfo(Variable* var_list, Function* func_list,int file_lines,char* filename){
	//prints all the info
	Function* func_head;
	func_head = func_list;
	
	printf(">>> Memory Model Layout <<<\n\n");
	printf("***  exec // text ***\n");
	printf("   %s\n",filename);
	printf("\n### ROData ###       scope type  size\n");
	printVarMemLocation(var_list,RODATA);
	printf("\n### static data ###\n");
	printVarMemLocation(var_list,STATIC_DATA);
	printf("\n### heap ###\n");
	printVarMemLocation(var_list,HEAP);
	printf("\n####################\n### unused space ###\n####################\n");
	printf("\n### stack ###\n");
	printVarMemLocation(var_list,STACK);
	printf("\n**** STATS ****\n");
	printf("  - Total number of lines in the file: %d\n", file_lines);
	int num_func = 0; // number of functions
	while(func_head!=NULL){
		num_func++;
		func_head = func_head->next;
	}
	func_head = func_list;
	printf("  - Total number of functions: %d\n    ",num_func);
	
	while(func_head!=NULL){
		printf("%s",func_head->name);
		if(func_head->next!=NULL)
			printf(", ");
		func_head = func_head->next;
	}
	func_head = func_list;
	printf("\n  - Total number of lines per functions:\n");	
	while(func_head!=NULL){
		printf("    %s: %d\n",func_head->name,func_head->lines);
		func_head = func_head->next;
	}
	func_head = func_list;
	printf("  - Total number of variables per functions:\n");	
	while(func_head!=NULL){
		printf("    %s: %d\n",func_head->name,func_head->vars);
		func_head = func_head->next;
	} 
	printf("//////////////////////////////\n");
	//free everything
	freeFunc(func_list);
	free(func_list);
	freeVar(var_list);
	free(var_list);
	
	return;
}
void readFile(char* file_path){
	FILE *current_file;
	current_file = fopen(file_path,"r");
	if(current_file==NULL){
		fprintf(stderr,"Error opening file, try putting \" \" around the path or replacing all \\ 's with / 's\n");
		exit(1);
	}
	Variable* var_list =NULL;
	Function* func_list = NULL;
	int line_size= maxLineSize(current_file)+1;
	char line[line_size];
	char token_ready_line[line_size*3]; // worst case scenario size
	
	rewind(current_file); //moves pointer back to the beginning
	
	//variables used in the reading process
	char* var_type = NULL;
	char* var_name= NULL;
	char* var_location= NULL;
	
	Function *current_func = NULL;
	char* func_name;
	int func_size = 0;
	
	int in_var = 0;
	int in_func = 0; //0 if not in function, 1 if in function
	
	int line_num = 1;//always at least 1 line
	int open_curly_brakets = 0;//the number of currently open curly brakets
	char * token;
	char * start_alloc;
	char * last_token;
	//starts the reading process
	while(fgets(line,line_size,current_file)!=NULL){
		if(var_type!=NULL)
			free(var_type);
		var_type = NULL;
		if(var_name!=NULL)
			free(var_name);
		var_name = NULL;
		if(var_location!=NULL)
			free(var_location);
		var_location = NULL;
		in_var = 0;
		prepareTokenize(line,token_ready_line,line_size);
		token = strtok(token_ready_line, " ");
		while(token != NULL) {
			if(recognizeType(token)){
				if(var_type!=NULL)
					free(var_type);
				var_type = malloc(sizeof(char)*TYPE_SIZE);
				strcpy(var_type,token);
				in_var = 1;
			}
			else if(in_var && var_name==NULL && strcmp(token,")")==0)
			{
					//not actually a var but just a cast
					free(var_type);
					var_type = NULL;
					//if(strcmp(token,"global")==0)
					free(var_location);
					var_location = NULL;
					in_var=0;
			}
			else if(in_var){
				if(strcmp(token,"*")==0){
					//adds * if it is a pointer
					strcat(var_type,token);
				}
				else if(var_name == NULL){
					//adds the name. Token will be the name since it comes right after the type.
					var_name = malloc(sizeof(char)*strlen(token)+1);
					strcpy(var_name,token);
				}
				if(in_func && var_location==NULL){
					//assigns location if it is in a function
					var_location = malloc(sizeof(char)*strlen(current_func->name));
					strcpy(var_location ,current_func->name);
				}
				else if(var_location==NULL)
				{
					//assigns location if it is not in a function
					var_location = malloc(sizeof(char)*5);
					strcpy(var_location,"global");
				}
				else if(var_type!=NULL &&strcmp(token,"*")!=0 && strcmp(token,var_name)!=0){ //starts the var adding process. Does not start until token is the word after the name
					if(strcmp(token,"[")==0)//case where it is an array
					{
						strcat(var_type,token);
						token = strtok(NULL, " ");
						
						if(strcmp(token,"]")!=0){
							var_type = changeStrSize(var_type,TYPE_SIZE+strlen(token)+1); //if is an array of a defined size, include the size of array
							strcat(var_type,token);
							token = strtok(NULL, " ");
							
						}
						strcat(var_type,token);
						token = strtok(NULL, " ");
						
					}
					
					if(!in_func && (strcmp(token,"(")==0)){ 
						//deals with the case where variable is actually a function (not very efficient but easy for me to understand)
						func_name = var_name;
						current_func = newFunc(current_func,func_name);
						func_list = addFunc(func_list,current_func);
						in_func = 1;
						var_name = NULL;
						free(var_type);
						var_type = NULL;
						free(var_location);
						var_location = NULL;
						in_var = 0;
					}
					else{
						if(in_func){ // case where it is a var
							(current_func->vars)++;
						}
						Variable* current_var = newVar(var_name,var_location,var_type,line_num);
						if(strcmp(token,"=")==0){ //case where is string literal and malloc/calloc
							char * temp_last_token = token;
							token = strtok(NULL, " ");
							
							if(token[0] == '\"'){ //it is a string literal
								token[strlen(token)] = ' ';
								current_var->mem_location = RODATA;
								token = strtok(token,"\"");
								char * size = malloc(sizeof(char)*10); //anything more than 10 digits is > 10^10 bytes 
								sprintf(size,"%ld",strlen(token));
								strcat(size,"*sizeof(char)");
								current_var->size = size; //this is okay since size is not used outside of this context
							}
							else{
								//if not a string literal then it goes back to the '=' and continues
								token[strlen(token)] = ' ';
								token = temp_last_token;
								token[strlen(token)] = ' ';
								token = strtok(token, " ");
								current_var->mem_location = determineMemLocation(current_var->location);
								current_var->size = findSize(current_var->type);
							}
						}
						else{
							//assign mem_location to the location in memory
							current_var->mem_location = determineMemLocation(current_var->location);
							current_var->size = findSize(current_var->type);
						}
						//adds the var name to the var_list
						var_list = addVar(var_list,current_var);
						//var_type = NULL;
						free(var_name);
						var_name = NULL;
						free(var_location);
						var_location = NULL;
						if(strcmp(token,",")!=0) // continues assigning vars if there is a comma
							in_var = 0;
					}
				}
			}
			else if(strContains(token,'{')){
				if(open_curly_brakets==0)
					func_size=-1;
				open_curly_brakets++;
			}
			else if(strContains(token,'}')){
				open_curly_brakets--;
				if(open_curly_brakets==0){
					in_func=0;
					current_func->lines = func_size;
					current_func = NULL;
					free(func_name);
					func_name = NULL;
					func_name = "global";
					func_size = 0;
				}
			}
			if(strcmp(token,"=")==0 &&((start_alloc = strstr((token+strlen(token)+1)," malloc "))!=NULL||(start_alloc = strstr((token+strlen(token)+1)," calloc "))!=NULL)){ //case where there is an allocation (either calloc or malloc)
				char * alloc_size = calculateAllocSize(start_alloc);
				char * alloc_name = malloc(sizeof(char)*(strlen(last_token)+1));
				alloc_name[0]='*';
				alloc_name[1]='\0';
				strcat(alloc_name,last_token);
				char * alloc_type = malloc(sizeof(char)*TYPE_SIZE);
				Variable * unalloced_var;
				unalloced_var = findVar(var_list,last_token,func_name);
				if(unalloced_var==NULL){
					fprintf(stderr,"file contains invalid syntax\n");
					exit(1);
				}
				strcpy(alloc_type,unalloced_var->type);
				replaceChar(alloc_type,'*','\0');
				
				Variable* alloc_var; 
				alloc_var = newVar(alloc_name,func_name,alloc_type,line_num);
				free(alloc_name);
				free(alloc_type);
				alloc_var->size = alloc_size;
				alloc_var->mem_location = HEAP;
				var_list = addVar(var_list,alloc_var);
				start_alloc = NULL;
			}
			last_token = token;
			token = strtok(NULL, " ");
		}
		
		if(strContains(line,'\n')){
			line_num++;
			if(in_func)
				func_size++;
		}
	}
	char * filename;
	if((filename = strrchr(file_path,'/'))==NULL)
		filename = file_path;
	else
		filename++;
	fclose(current_file);
	printInfo(var_list,func_list,line_num,filename);
}
int main(int argc, char** argv){ //DONE
	if(argc==1){
		fprintf(stderr,"Please include a path to a C file\n");
		return 1;
	}
	readFile(argv[1]); //takes command line argument
	return 0;
}