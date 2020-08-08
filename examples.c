#include <stdio.h>
#include <stdlib.h>
#include "longer-int.h"

int examples()
{
///Creating a new LINT
LINT*a=new_lint_str("12345");//a=12345
LINT*b=new_lint_str("-15");//b=-15
LINT*c=new_lint_num_degree(5,0);//c=5
LINT*d=new_lint_num_degree(0xFFEEDDCC,23);//d=0xFFEEDDCC*2^(32*23)
LINT*e=clone_lint(a);//e=a

///Forcing a new value
init_lint_str(d,"3333");//d=3333
copy_lint(c,a);//c=a

///Arithmetic operations
add_lint(a,b);//a=a+b
subtract_lint(a,b);//a=a-b
mul_lint(a,b);//a=a*b
mul_lint_abs(a,b);//abs(a)=abs(a)*abs(b)
div_lint(a,b);//a=a/b
div_lint_abs(a,b);//abs(a)=abs(a)/abs(b)
pow_lint(a,b);//a=pow(a,b)

///Arithmetic modulus operations
mul_lint_mod(a,b,c);//a=((a%c)*(b%c))%c
pow_lint_mod(a,b,c);//a=pow(a,b)%c
mod_lint(a,b);//a=a%b

///Logical operations
printf("%d\n",compare_lint(a,b));
printf("%d\n",compare_lint(a,a));
printf("%d\n",compare_lint_abs(a,b));
printf("%d\n",divisible(a,b));
printf("%d\n\n",divisible(a,a));

///Bitwise operations
shr_lint(a,20);//a=a>>20
shl_lint(a,15);//a=a<<15
shr_lint_lint(a,b);//a=a>>b
shl_lint_lint(a,b);//a=a<<b
or_lint(a,b);//a=a|b
and_lint(a,b);//a=a&b
xor_lint(a,b);//a=a^b
complement_lint(a);//a=~a

///Printing
print_lint(a);//prints lint as a decimal value
print_lint_polynomial(a);//prints lint as a polynomial of hexadecimal values
print_lint_polynomial_extra(a);//prints lint as a polynomial of hexadecimal values
//and specifies the allocated size and the used size
char*s=lint_itoa(a);
if(s)
{
	printf("%s\n",s);
	free(s);
}

///Memory management
expand_size(a);//doubles the allocated memory for "a"
attempt_shrink(a);//shrinks the allocated memory for "a" to a value closer to the used size without losing data
free_lint(a);//frees lint from the heap
free_lint(b);
free_lint(c);
free_lint(d);
free_lint(e);
}