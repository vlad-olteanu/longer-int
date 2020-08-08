#ifndef LONGER_INT_H
#define LONGER_INT_H
///Data structures
typedef struct _LINT
{
	union
	{
		unsigned int* x;//polynomial data
		unsigned char* byte_data;
	};
	unsigned long long size;//allocated size, measured in blocks of 32 bits
	unsigned long long used_size;//actual space occupied by the number, measured in blocks of 32 bits
	unsigned char sign;//0=positive,anything else=negative
}LINT;
///Functions
///Arithmetic
int add_lint(LINT* a, LINT* b);
int subtract_lint(LINT* a,LINT* b);
int mul_lint(LINT *a,LINT *b);
int mul_lint_abs(LINT* a,LINT *b);
int mul_lint_mod(LINT* a,LINT* b,LINT* gf);
int div_lint(LINT* a,LINT* b);
int div_lint_abs(LINT*target,LINT*divisor);
int pow_lint(LINT* target,LINT* exponent);
int pow_lint_mod(LINT*target,LINT*exponent,LINT* gf);
int add_coefficient(LINT* target, unsigned long long degree, unsigned int number);
int sub_coefficient(LINT* target, unsigned long long degree, unsigned int number);
int mod_lint(LINT* target,LINT* gf);
///Logical
int compare_lint(LINT*a, LINT* b);
int compare_lint_abs(LINT*a, LINT* b);
int divisible(LINT* a,LINT *b);
///Allocation/initialisation
LINT* new_lint_num_degree(unsigned int number,unsigned long long degree);
LINT* new_lint_str(char* string);
int init_lint_str(LINT* target,char* string);
int copy_lint(LINT*a,LINT*b);
LINT* clone_lint(LINT* target);
///Memory management
int attempt_shrink(LINT* target);
int expand_size(LINT* target);
void free_lint(LINT* target);
///Printing
void print_lint_polynomial(LINT* target);
void print_lint_polynomial_extra(LINT* target);
void print_lint(LINT* target);
char* lint_itoa(LINT* target);
///Bitwise operations
void shr_lint(LINT* target,unsigned long long amount);
int shl_lint(LINT* target,unsigned long long amount);
int shl_lint_lint(LINT* target,LINT* amount);
int shr_lint_lint(LINT* target,LINT* amount);
int or_lint(LINT* a,LINT* b);
int and_lint(LINT* a,LINT* b);
int xor_lint(LINT* a,LINT* b);
int complement_lint(LINT* a);
#endif //LONGER_INT_H
