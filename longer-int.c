#include "longer-int.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///=================Data Structures=======================
typedef union _SPLIT_LL
{
	unsigned long long value;
	struct
	{
		unsigned int lowpart,highpart;
	};
}SPLIT_LL;

///=======================Functions=======================

///=======================Arithmetic======================
//Adds the values of "a" and "b" and saves the result in "a"
//a=a+b
//success => return 1
//failure => return 0
int add_lint(LINT*a,LINT*b)
{
	if(!a||!b)
	{
		return 0;
	}
	int res=0,retcode=0;
	LINT*result=0;
	///if b==0
	if(b->used_size==1&&b->x[0]==0)
	{
		return 1;
	}
	///if a==0
	if(a->used_size==1&&a->x[0]==0)
	{
		res=copy_lint(a,b);
		if(res==0)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	///if a->sign == b->sign
	if((a->sign==0&&b->sign==0)||(a->sign!=0&&b->sign!=0))
	{
		//result=a
		result=clone_lint(a);
		if(!result)
		{
			printf("Error: memory allocation failure in add_lint\n");
			return 0;
		}
		for(unsigned long long i=0;i<b->used_size;i++)
		{
			res=add_coefficient(result,i,b->x[i]);
			if(res==0)
			{
				retcode=0;
				goto end;
			}
		}
		retcode=1;
		goto end;
	}
	///if a>0 and b<0
	if(a->sign==0&&b->sign!=0)
	{
		//a+(-b)=a-b
		LINT positive_b=*b;
		positive_b.sign=0;
		return subtract_lint(a,&positive_b);
	}
	else///if a<0 and b>0
	{
		//-a+b=-a-(-b)
		LINT negative_b=*b;
		negative_b.sign=1;
		return subtract_lint(a,&negative_b);
	}

	end:
	if(retcode==0)
	{
		if(result)
		{
			free_lint(result);
		}
	}
	if(retcode==1)
	{
		free(a->x);
		*a=*result;
		free(result);
	}
	return retcode;
}

//Subtracts the value of "b" from the value of "a" and saves the result in "a"
//a=a-b
//success => return 1
//failure => return 0
int subtract_lint(LINT*a,LINT*b)
{
	int retcode=0,res=0;
	unsigned char sign=a->sign;
	LINT* result=0;
	if(!a||!b)
	{
		return 0;
	}

	///if b==0
	if(b->used_size==1&&b->x[0]==0)
	{
		return 1;
	}

	///if a==0
	if(a->used_size==1&&a->x[0]==0)
	{
		//a=-b
		res=copy_lint(a,b);
		if(res==1)
		{
			a->sign=!sign;
			return 1;
		}
		else
		{
			return 0;
		}
	}

	res=compare_lint(a,b);
	if(res==-2)
	{
		return 0;
	}
	///if a==b => a=0
	if(res==0)
	{
		res=init_lint_str(a,"0");
		if(res==0)
		{
			retcode=0;
		}
		else
		{
			retcode=1;
		}
		goto end;
	}

	///if a->sign == b->sign
	if((a->sign==0&&b->sign==0)||(a->sign!=0&&b->sign!=0))
	{
		res=compare_lint_abs(a,b);
		if(res==-2)
		{
			return 0;
		}
		///if(abs(a)>abs(b))
		if(res==1)
		{
			//result=a
			result=clone_lint(a);
			if(!result)
			{
				printf("Error: memory allocation failure in subtract_lint\n");
				retcode=0;
				goto end;
			}
			for(unsigned long long i=0;i<b->used_size;i++)
			{
				//result[i]-=b[i]
				if(b->x[i])
				{
					res=sub_coefficient(result,i,b->x[i]);
					if(res==0)
					{
						retcode=0;
						goto end;
					}
				}
			}
			retcode=1;
			goto end;
		}
		else///if(abs(b)>abs(a))
		{
			sign=!sign;
			//result=b
			result=clone_lint(b);
			if(!result)
			{
				printf("Error: memory allocation failure in subtract_lint\n");
				retcode=0;
				goto end;
			}
			for(unsigned long long i=0;i<a->used_size;i++)
			{
				res=sub_coefficient(result,i,a->x[i]);
				if(res==0)
				{
					retcode=0;
					goto end;
				}
			}
			retcode=1;
			goto end;
		}
	}
	///if a>0 and b<0
	if(a->sign==0&&b->sign!=0)
	{
		//a-(-b)=a+b
		LINT positive_b=*b;
		positive_b.sign=0;
		return add_lint(a,&positive_b);
	}
	else///if a<0 and b>0
	{
		//-a-b=-(a+b)
		LINT positive_a=*a;
		positive_a.sign=0;
		res=add_lint(&positive_a,b);
		if(res==0)
		{
			return 0;
		}
		*a=positive_a;
		a->sign=1;
		return 1;
	}

	end:
	if(retcode==0)
	{
		if(result)
		{
			free_lint(result);
		}
	}
	if(retcode==1)
	{
		if(result)
		{
			free(a->x);
			*a=*result;
			free(result);
		}
		a->sign=sign;
	}
	return retcode;
}

//Multiplies the value of "a" with the value of "b"
//using the discrete fourier transform and saves the result in "a"
//a=a*b
//success => return 1
//failure => return 0
int mul_lint(LINT*a,LINT*b)
{
	if(!a||!b)
	{
		return 0;
	}

	unsigned char sign=a->sign^b->sign;

	if(mul_lint_abs(a,b)==0)
	{
		return 0;
	}

	a->sign=sign;
	return 1;
}

//Multiplies the absolute value of "a" with the absolute value of "b"
//using the discrete fourier transform and saves the result in "a", keeping "a"'s old sign
//abs(a)=abs(a)*abs(b)
//success => return 1
//failure => return 0
int mul_lint_abs(LINT*a,LINT*b)
{
	if(!a||!b)
	{
		return 0;
	}
	int retcode=1;
	unsigned long long d=a->used_size+b->used_size;
	LINT*c=new_lint_num_degree(0,0);
	if(!c)
	{
		printf("Error: memory allocation failure in mul_lint\n");
		return 0;
	}
	int ok=1;
	for(unsigned long long n=0;n<d;n++)
	{
		for(unsigned long long m=0;m<=n;m++)
		{
			SPLIT_LL aux;
			aux.value=(m>=a->used_size?0:a->x[m]);
			aux.value*=(n-m>=b->used_size?0:b->x[n-m]);
			if(aux.lowpart)
			{
				if(add_coefficient(c,n,aux.lowpart)==0)
				{
					retcode=0;
					goto end;
				}
			}
			if(aux.highpart)
			{
				if(add_coefficient(c,n+1,aux.highpart)==0)
				{
					retcode=0;
					goto end;
				}
			}
		}
	}
	end:
	if(retcode==1)
	{
		attempt_shrink(c);
		free(a->x);
		*a=*c;
		free(c);
		return 1;
	}
	if(retcode==0)
	{
		free_lint(c);
	}
	return retcode;
}

//Multiplies the value of "a" with the value of "b"
//using the discrete fourier transform inside a galois/finite field
//and saves the result in "a"
//a=((a%gf)*(b%gf))%gf
//success => return 1
//failure => return 0
int mul_lint_mod(LINT*a,LINT*b,LINT*gf)
{
	if(!a||!b||!gf)
	{
		return 0;
	}
	int retcode=1;
	LINT*aux_a=0,*aux_b=0;
	aux_a=clone_lint(a);
	if(!aux_a)
	{
		printf("Error: memory allocation failure in mul_lint_abs_mod\n");
		return 0;
	}
	aux_b=clone_lint(b);
	if(!aux_b)
	{
		printf("Error: memory allocation failure in mul_lint_abs_mod\n");
		retcode=0;
		goto end;
	}
	if(mod_lint(aux_a,gf)==0)
	{
		retcode=0;
		goto end;
	}
	if(mod_lint(aux_b,gf)==0)
	{
		retcode=0;
		goto end;
	}
	if(mul_lint_abs(aux_a,aux_b)==0)
	{
		retcode=0;
		goto end;
	}
	if(mod_lint(aux_a,gf)==0)
	{
		retcode=0;
		goto end;
	}
	end:
	if(retcode==1)
	{
		free_lint(aux_b);
		free(a->x);
		*a=*aux_a;
		free(aux_a);
	}
	if(retcode==0)
	{
		if(aux_a)
		{
			free_lint(aux_a);
		}
		if(aux_b)
		{
			free_lint(aux_b);
		}
	}
	return retcode;
}

//Divides the value of "target" by the value of "divisor" and
//saves the result in "target". Implemented with long division.
//target=target/divisor
//success => return 1
//failure => return 0
int div_lint(LINT*target,LINT*divisor)
{
	if(!target||!divisor)
	{
		return 0;
	}

	unsigned char sign=target->sign^divisor->sign;
	target->sign=divisor->sign;

	int res=div_lint_abs(target,divisor);
	if(res==0)
	{
		return 0;
	}
	target->sign=sign;
	return 1;
}
//Divides the absolute value of "target" by the absolute value of "divisor" and
//saves the result in "target", keeping "target"'s old sign. Implemented with long division.
//abs(target)=abs(target)/abs(divisor)
//success => return 1
//failure => return 0
int div_lint_abs(LINT*target,LINT*divisor)
{
	if(!target||!divisor)
	{
		return 0;
	}
	int res=0,retcode=1;
	///aux_target=a copy of target
	///sh_divisor=the shifted divisor
	///sh_one=the number 1 shifted as many times as sh_divisor
	LINT*result=0,*aux_target=0,*sh_divisor=0,*sh_one=0;

	///if divisor==0
	if(divisor->used_size==1&&divisor->x[0]==0)
	{
		return 0;
	}

	///if divisor==1 or -1
	if(divisor->used_size==1&&divisor->x[0]==1)
	{
		return 1;
	}

	res=compare_lint_abs(target,divisor);
	if(res==-2)
	{
		return 0;
	}
	///if target is smaller than the divisor => result=0
	if(res==-1)
	{
		res=init_lint_str(target,"0");
		if(res==1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	///if target==divisor => result=1
	if(res==0)
	{
		res=init_lint_str(target,"1");
		if(res==1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	result=new_lint_num_degree(0,0);
	if(!result)
	{
		printf("Error: Memory allocation failure in div_lint\n");
		return 0;
	}

	aux_target=clone_lint(target);
	if(!aux_target)
	{
		printf("Error: Memory allocation failure in div_lint\n");
		retcode=0;
		goto end;
	}

	sh_divisor=clone_lint(divisor);
	if(!sh_divisor)
	{
		printf("Error: Memory allocation failure in div_lint\n");
		retcode=0;
		goto end;
	}

	sh_one=new_lint_num_degree(1,0);
	if(!sh_one)
	{
		printf("Error: Memory allocation failure in div_lint\n");
		retcode=0;
		goto end;
	}

	///while sh_divisor<target
	do
	{
		res=shl_lint(sh_divisor,1);
		if(!res)
		{
			retcode=0;
			goto end;
		}

		res=shl_lint(sh_one,1);
		if(!res)
		{
			retcode=0;
			goto end;
		}

		res=compare_lint_abs(sh_divisor,target);
		if(res==-2)
		{
			retcode=0;
			goto end;
		}
	}while(res==-1);

	///while aux_target>divisor
	do
	{
		///while sh_divisor>aux_target
		res=compare_lint_abs(sh_divisor,aux_target);
		while(res==1)
		{
			shr_lint(sh_divisor,1);
			shr_lint(sh_one,1);

			res=compare_lint_abs(sh_divisor,aux_target);
			if(res==-2)
			{
				retcode=0;
				goto end;
			}
		}

		res=subtract_lint(aux_target,sh_divisor);
		if(!res)
		{
			retcode=0;
			goto end;
		}

		res=add_lint(result,sh_one);
		if(!res)
		{
			retcode=0;
			goto end;
		}
		res=compare_lint_abs(aux_target,divisor);
	}while(res!=-1);

	end:
	if(retcode==1)
	{
		free(target->x);
		*target=*result;
		free(result);
		attempt_shrink(target);
	}
	if(retcode==0)
	{
		free_lint(result);
	}
	if(aux_target)
		free_lint(aux_target);
	if(sh_divisor)
		free_lint(sh_divisor);
	if(sh_one)
		free_lint(sh_one);
	return retcode;
}

//Exponentiates the value of "target" by the value of "exponent" using
//exponentiation by squaring and saves the result in "target"
//target=pow(target,exponent)
//success => return 1
//failure => return 0
int pow_lint(LINT*target,LINT*exponent)
{
	if(!target||!exponent)
	{
		return 0;
	}

	int retcode=1,res=0;
	LINT* result=0,*aux_target=0,*aux_exponent=0;

	///if exponent==0 => result=1
	if(exponent->used_size==1&&exponent->x[0]==0)
	{
		res=init_lint_str(target,"1");
		if(res==1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	///if the exponent<0 => result=0
	if(exponent->sign!=0)
	{
		res=init_lint_str(target,"0");
		if(res==1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	///if exponent==1
	if(exponent->used_size==1&&exponent->x[0]==1)
	{
		return 1;
	}

	result=new_lint_num_degree(1,0);
	if(!result)
	{
		printf("Error: memory allocation failure in mul_lint\n");
		return 0;
	}

	aux_target=clone_lint(target);
	if(!aux_target)
	{
		printf("Error: memory allocation failure in mul_lint\n");
		retcode=0;
		goto end;
	}

	aux_exponent=clone_lint(exponent);
	if(!aux_exponent)
	{
		printf("Error: memory allocation failure in mul_lint\n");
		retcode=0;
		goto end;
	}

	unsigned char sign;
	if(target->sign==0)
	{
		sign=0;
	}
	else
	{
		if(aux_exponent->x[0]%2==0)
		{
			sign=0;
		}
		else
		{
			sign=1;
		}
	}

	while(aux_exponent->used_size>1||aux_exponent->x[0]>=1)
	{
		if(aux_exponent->x[0]%2==1)
		{
			res=mul_lint_abs(result,aux_target);
			if(res==0)
			{
				retcode=0;
				goto end;
			}
			aux_exponent->x[0]--;
		}
		else
		{
			res=mul_lint_abs(aux_target,aux_target);
			if(res==0)
			{
				retcode=0;
				goto end;
			}
			shr_lint(aux_exponent,1);
		}
	}

	end:
	if(retcode==1)
	{
		free(target->x);
		*target=*result;
		target->sign=sign;
		free(result);
	}
	if(retcode==0)
	{
		if(result)
			free_lint(result);
	}
	if(aux_target)
		free_lint(aux_target);
	if(aux_exponent)
		free_lint(aux_exponent);
	return retcode;
}

//Exponentiates the value of "target" by the value of "exponent" using
//exponentiation by squaring in a galois/finite field and saves the result in "target"
//target=pow(target,exponent)%gf
//success => return 1
//failure => return 0
int pow_lint_mod(LINT*target,LINT*exponent,LINT* gf)
{
	if(!target||!exponent||!gf)
	{
		return 0;
	}

	int retcode=1,res=0;
	LINT*result=0,*aux_target=0;

	///if exponent==0 => result=1 mod gf
	if(exponent->used_size==1&&exponent->x[0]==0)
	{
		result=new_lint_num_degree(1,0);
		if(!result)
		{
			return 0;
		}
		res=mod_lint(result,gf);
		if(res==0)
		{
			retcode=0;
		}
		goto end;
	}

	///if exponent<0 => result=0
	if(exponent->sign!=0)
	{
		res=init_lint_str(target,"0");
		if(res==0)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}

	///if exponent==1
	if(exponent->used_size==1&&exponent->x[0]==1)
	{
		return mod_lint(target,gf);
	}

	result=new_lint_num_degree(1,0);
	if(!result)
	{
		printf("Error: memory allocation failure in mul_lint\n");
		return 0;
	}

	aux_target=clone_lint(target);
	if(!aux_target)
	{
		printf("Error: memory allocation failure in mul_lint\n");
		retcode=0;
		goto end;
	}

	LINT*aux_exponent=clone_lint(exponent);
	if(!aux_exponent)
	{
		printf("Error: memory allocation failure in mul_lint\n");
		retcode=0;
		goto end;
	}

	while(aux_exponent->used_size>1||aux_exponent->x[0]>=1)
	{
		if(aux_exponent->x[0]%2==1)
		{
			res=mul_lint_mod(result,aux_target,gf);
			if(res==0)
			{
				retcode=0;
				goto end;
			}
			aux_exponent->x[0]--;
		}
		else
		{
			res=mul_lint_mod(aux_target,aux_target,gf);
			if(res==0)
			{
				retcode=0;
				goto end;
			}
			shr_lint(aux_exponent,1);
		}
	}
	end:
	if(retcode==1)
	{
		free(target->x);
		*target=*result;
		free(result);
	}
	if(retcode==0)
	{
		if(result)
			free_lint(result);
	}
	if(aux_target)
		free_lint(aux_target);
	if(aux_exponent)
		free_lint(aux_exponent);
	return retcode;
}

//Adds number to the specified degree in the polynomial that represents the LINT
//This function works with absolute values, to add number*((2^32)^degree) to target
//use new_lint_num_degree() and add_lint()
//success => return 1
//failure => return 0
int add_coefficient(LINT*target,unsigned long long degree,unsigned int number)
{
	if(!target)
		return 0;
	if(number==0)
		return 1;
	//if we need more space we allocate more space
	if(degree>=target->size)
	{
		unsigned long long i=2;
		while(degree>=target->size*i)
		{
			i*=2;
		}
		unsigned int*aux=(unsigned int*)realloc(target->x,i*(target->size)*sizeof(unsigned int));
		if(!aux)
		{
			printf("Error: realloc failed in add_coefficient\n");
			return 0;
		}
		target->x=aux;
		memset(target->x+target->size,0,(i*target->size-target->size)*sizeof(unsigned int));
		target->size=i*target->size;
	}
	if(degree>=target->used_size)
	{
		target->used_size=degree+1;
	}
	SPLIT_LL x;
	x.highpart=0;
	x.value=target->x[degree];
	x.value+=(unsigned long long)number;
	target->x[degree]=x.lowpart;
	//if we have arithmetic overflow we add the remainder to the next degree
	if(x.highpart)
	{
		return add_coefficient(target,degree+1,1);
	}
	return 1;
}

//Subtracts number from the specified degree in the polynomial that represents the LINT
//This function works with absolute values, to subtract number*((2^32)^degree) from target
//use new_lint_num_degree() and subtract_lint()
//success => return 1
//failure => return 0
int sub_coefficient(LINT*target,unsigned long long degree,unsigned int number)
{
	if(!target)
		return 0;
	if(number==0)
	{
		return 1;
	}
	//If the degree we want to subtract from exceeds the used size of the number it means
	//that we would have an arithmetic underflow.
	//To solve this we create a new LINT with the value of number*((2^32)^degree) and subtract
	//our current LINT from it then negate the LINT's sign.
	if(degree>=target->used_size)
	{
		LINT*aux=new_lint_num_degree(number,degree);
		if(!aux)
		{
			printf("Error: memory allocation failure in sub_coefficient\n");
			return 0;
		}
		//we make sure the target and aux have the same sign so subtract_lint actually subtracts them
		aux->sign=target->sign;
		if(subtract_lint(aux,target))
		{
			free(target->x);
			*target=*aux;
			free(aux);
			//we negate the target's sign
			target->sign=!target->sign;
			return 1;
		}
		free_lint(aux);
		return 0;
	}
	else
	{
		SPLIT_LL x;
		x.highpart=1;
		x.lowpart=target->x[degree];
		x.value=x.value-(unsigned long long)number;
		target->x[degree]=x.lowpart;
		attempt_shrink(target);
		//if there's underflow we borrow from the next coefficient
		if(x.highpart!=1)
		{
			return sub_coefficient(target,degree+1,1);
		}
		attempt_shrink(target);
		return 1;
	}
}

//Applies the modulus operation on "target"
//target=target%gf
//success => return 1
//failure => return 0
int mod_lint(LINT*target,LINT*gf)
{
	if(!target||!gf)
	{
		return 0;
	}
	int res=0,retcode=1;
	LINT*result=0,*gf_mul=0;

	res=compare_lint_abs(target,gf);
	if(res==-2)
	{
		return 0;
	}
	///if target is included in the galois field (target<gf)
	if(res==-1)
	{
		return 1;
	}
	///if target is just outside of the galois field (target=gf)
	if(res==0)
	{
		result=new_lint_num_degree(0,0);
		if(!result)
		{
			printf("Error: Memory allocation failure in mod_lint\n");
			return 0;
		}
		free(target->x);
		*target=*result;
		free(result);
		return 1;
	}
	result=clone_lint(target);
	if(!result)
	{
		printf("Error: Memory allocation failure in mod_lint\n");
		return 0;
	}
	result->sign=gf->sign;

	///gf_mul=gf * the smallest power of 2 possible so that gf_mul>result
	gf_mul=clone_lint(gf);
	if(!gf_mul)
	{
		printf("Error: Memory allocation failure in mod_lint\n");
		retcode=0;
		goto end;
	}
	///while gf_mul<result
	do
	{
		res=shl_lint(gf_mul,1);
		if(!res)
		{
			retcode=0;
			goto end;
		}
		res=compare_lint_abs(result,gf_mul);
		if(res==-2)
		{
			retcode=0;
			goto end;
		}
	}while(res!=-1);

	///while result>gf
	do
	{
		res=compare_lint_abs(gf_mul,result);
		if(res==-2)
		{
			retcode=0;
			goto end;
		}
		///while gf_mul>result shift right
		while(res==1)
		{
			shr_lint(gf_mul,1);
			res=compare_lint_abs(gf_mul,result);
			if(res==-2)
			{
				retcode=0;
				goto end;
			}
		}
		///result-=gf_mul
		res=subtract_lint(result,gf_mul);
		if(res==0)
		{
			retcode=0;
			goto end;
		}
		res=compare_lint_abs(result,gf);
		if(res==-2)
		{
			retcode=0;
			goto end;
		}
	}while(res!=-1);
	end:
	if(retcode==1)
	{
		attempt_shrink(result);
		free(target->x);
		result->sign=target->sign;
		*target=*result;
		free(result);
		if(gf_mul)
		{
			free_lint(gf_mul);
		}
	}
	if(retcode==0)
	{
		if(result)
		{
			free_lint(result);
		}
		if(gf_mul)
		{
			free_lint(gf_mul);
		}
	}
	return retcode;
}

///=======================Logical=========================

//Compares the value of a and b
//a==b => return 0
//a>b => return 1
//a<b => return -1
//data validation error => return -2
int compare_lint(LINT*a,LINT*b)
{
	if(!a||!b)
	{
		return -2;
	}
	///if a==0 and b==0
	if((a->used_size==1&&a->x[0]==0)&&(b->used_size==1&&b->x[0]==0))
	{
		return 0;
	}
	///if a>=0 and b<=0, (a,b)!=(0,0)
	if(a->sign==0&&b->sign!=0)
	{
		return 1;
	}
	///if a<=0 and b>=0, (a,b)!=(0,0)
	if(b->sign==0&&a->sign!=0)
	{
		return -1;
	}
	///if a>=0 and b>=0, (a,b)!=(0,0)
	if(a->sign==0&&b->sign==0)
	{
		return compare_lint_abs(a,b);
	}
	else///if a<=0 and b<=0, (a,b)!=(0,0)
	{
		int res=-compare_lint_abs(a,b);
		//if res==2 => return -2, else return res
		return res==2?-2:res;
	}
}

//Compares the absolute value of a and b
//a==b => return 0
//a>b => return 1
//a<b => return -1
//data validation error => return -2
int compare_lint_abs(LINT*a,LINT*b)
{
	if(!a||!b)
	{
		return -2;
	}
	if(a->used_size>b->used_size)
	{
		return 1;
	}
	if(b->used_size>a->used_size)
	{
		return -1;
	}
	///if a and b have the same degree check each degree individually
	for(unsigned long long i=a->used_size-1;i<a->used_size;i--)
	{
		if((a->x[i])!=(b->x[i]))
		{
			if(a->x[i]>b->x[i])
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
	}
	return 0;
}

//Checks if a is divisible by b
//a%b==0 => return 1
//a%b!=0 => return 0
//unexpected error => return -1
int divisible(LINT* a,LINT *b)
{
	if(!a||!b)
	{
		return -1;
	}

	int retcode=0,res;
	LINT* aux_a;

	aux_a=clone_lint(a);
	if(!aux_a)
	{
		printf("Error: memory allocation failure in divisible\n");
		return -1;
	}

	res=mod_lint(aux_a,b);
	if(res==0)
	{
		retcode=-1;
		goto end;
	}

	///if a%b==0
	if(aux_a->used_size==1&&aux_a->x[0]==0)
	{
		retcode=1;
		goto end;
	}
	else
	{
		retcode=0;
		goto end;
	}

	end:
	if(aux_a)
	{
		free_lint(aux_a);
	}
	return retcode;
}

///==============Allocation/initialisation================
//Creates a new LINT with the value number*((2^32)^degree)
LINT*new_lint_num_degree(unsigned int number,unsigned long long degree)
{
	if(number==0&&degree!=0)
	{
		return new_lint_num_degree(0,0);
	}

	LINT*target=(LINT*)calloc(1,sizeof(LINT));
	if(!target)
	{
		printf("Error: memory allocation failure in new_lint_num_degree\n");
		return 0;
	}
	target->size=1;
	//we find the smallest power of 2 larger than degree
	while(target->size<=degree)
	{
		target->size*=2;
	}
	target->x=(unsigned int*)calloc(target->size,sizeof(unsigned int));
	if(!target->x)
	{
		free(target);
		return 0;
	}
	target->used_size=degree+1;
	target->x[degree]=number;
	target->sign=0;
	return target;
}

//Allocates and initialises a LINT using a string
LINT*new_lint_str(char*string)
{
	if(!string)
	{
		return 0;
	}

	LINT*target=new_lint_num_degree(0,0);
	if(!target)
	{
		printf("Error: memory allocation failure in new_lint_str\n");
		return 0;
	}

	if(init_lint_str(target,string))
	{
		return target;
	}
	else
	{
		free_lint(target);
		return 0;
	}
}

//Initialises a LINT using a string
//success => return 1
//failure => return 0
int init_lint_str(LINT*target,char*string)
{
	if(!target||!string)
	{
		return 0;
	}
	unsigned char sign=0;
	int retcode=1,res=0;
	LINT* aux=0,*result,*ten;

	///ten=10
	ten=new_lint_num_degree(10,0);
	if(!ten)
	{
		retcode=0;
		goto end;
	}

	///result=0
	result=new_lint_num_degree(0,0);
	if(!result)
	{
		retcode=0;
		goto end;
	}

	while(string[0])
	{
		if(string[0]>='0'&&string[0]<='9')
		{
			///result*=10
			res=mul_lint(result,ten);
			if(!res)
			{
				retcode=0;
				goto end;
			}

			///aux=string[i]
			if(aux)
				free_lint(aux);
			aux=new_lint_num_degree(string[0]-'0',0);
			if(!aux)
			{
				retcode=0;
				goto end;
			}

			///result+=aux
			res=add_lint(result,aux);
			if(!res)
			{
				retcode=0;
				goto end;
			}
		}
		if(string[0]=='-')
		{
			sign=!sign;
		}
		///pointer incrementation
		string++;
	}
	end:
	if(retcode==1)
	{
		free(target->x);
		*target=*result;
		target->sign=sign;
		free(result);
	}
	if(retcode==0)
	{
		if(result)
		{
			free_lint(result);
		}
	}
	if(aux)
		free_lint(aux);
	if(ten)
		free_lint(ten);
	return retcode;
}

//Copies the value of "b" to "a"
//a=b
//success => return 1
//failure => return 0
int copy_lint(LINT*a,LINT*b)
{
	unsigned int*v=(unsigned int*)calloc(b->size,sizeof(unsigned int));
	if(!v)
	{
		return 0;
	}
	for(unsigned long long i=0;i<b->used_size;i++)
	{
		v[i]=b->x[i];
	}
	free(a->x);
	*a=*b;
	a->x=v;
	return 1;
}

//Creates a copy of the specified LINT
LINT* clone_lint(LINT*target)
{
	if(!target)
	{
		return 0;
	}
	LINT* aux=(LINT*)calloc(1,sizeof(LINT));
	if(!aux)
	{
		printf("Error: memory allocation failure in clone_lint");
		return 0;
	}
	aux->x=(unsigned int*)calloc(target->size,sizeof(unsigned int));
	if(!(aux->x))
	{
		printf("Error: memory allocation failure in clone_lint");
		free(aux);
		return 0;
	}
	aux->used_size=target->used_size;
	aux->size=target->size;
	aux->sign=target->sign;
	for(unsigned long long i=0;i<aux->used_size;i++)
	{
		aux->x[i]=target->x[i];
	}
	return aux;
}

///==================Memory management====================
//Attempts shrinking the size of the LINT to a number closer to the used size if necessary
//success => return 1
//failure => return 0
int attempt_shrink(LINT*target)
{
	if(!target)
	{
		return 0;
	}
	while(target->x[target->used_size-1]==0&&target->used_size!=1)
	{
		target->used_size--;
	}
	unsigned long long new_size=target->size;
	while(target->used_size<=new_size/2)
	{
		new_size/=2;
	}
	if(new_size!=target->size)
	{
		unsigned int*aux=realloc(target->x,new_size*sizeof(unsigned int));
		if(!aux)
		{
			printf("Error: realloc failed in attempt_shrink\n");
			return 0;
		}
		else
		{
			target->size=new_size;
			target->x=aux;
			return 1;
		}
	}
	else
	{
		return 1;
	}
}

//Frees the LINT and its contents from the heap
void free_lint(LINT*target)
{
	if(!target)
	{
		return;
	}
	free(target->x);
	free(target);
}

//Doubles the size of the LINT
//success => return 1
//failure => return 0
int expand_size(LINT*target)
{
	if(!target)
	{
		return 0;
	}
	unsigned int*aux=(unsigned int*)realloc(target->x,target->size*2*sizeof(unsigned int));
	if(!aux)
	{
		return 0;
	}
	target->x=aux;
	memset(target->x+target->size,0,target->size*sizeof(unsigned int));
	target->size*=2;
	return 1;
}

///=======================Printing========================
//Prints the size, used size and hexadecimal polynomial values of the LINT
void print_lint_polynomial_extra(LINT*target)
{
	if(!target)
	{
		return;
	}
	printf("Size:%llu Used size:%llu Data:",target->size,target->used_size);
	print_lint_polynomial(target);
}

//Prints the hexadecimal polynomial values of the LINT
void print_lint_polynomial(LINT*target)
{
	if(!target)
	{
		printf("Error: invalid target in print_lint_polynomial");
		return;
	}
	if(target->sign!=0)
	{
		printf("-");
	}
	for(unsigned long long i=target->used_size-1;i<target->used_size;i--)
	{
		printf("%x ",target->x[i]);
	}
	printf("\n");
}

//Prints the value of the LINT as a decimal value
void print_lint(LINT* target)
{
	char* string=0;
	string=lint_itoa(target);
	if(!string)
	{
		return;
	}
	printf("%s\n",string);
	free(string);
}

//Returns a dynamically allocated string with the value of target
char* lint_itoa(LINT* target)
{
	int ok=1,res;
	char*result=0;
	unsigned long long result_size=2,result_iterator=0;
	LINT*aux_target=0,*ten=0,*aux_mod=0;

	result=(char*)calloc(result_size,sizeof(char));
	if(!result)
	{
		return 0;
	}

	aux_target=clone_lint(target);
	if(!aux_target)
	{
		ok=0;
		goto end;
	}

	ten=new_lint_num_degree(10,0);
	if(!ten)
	{
		ok=0;
		goto end;
	}

	aux_mod=new_lint_num_degree(0,0);
	if(!aux_mod)
	{
		ok=0;
		goto end;
	}
	///while aux_target!=0
	do
	{
		result_iterator++;
		if(result_iterator>=result_size-1)
		{
			char* aux=(char*)realloc(result,2*result_size*sizeof(char));
			if(!aux)
			{
				printf("Error: realloc failed in lint_itoa\n");
				ok=0;
				goto end;
			}
			result=aux;
			memset(result+result_size,0,result_size);
			result_size*=2;
		}

		///result[i]=x%10
		//aux_mod=aux_target
		res=copy_lint(aux_mod,aux_target);
		if(!res)
		{
			ok=0;
			goto end;
		}
		//aux_mod=aux_target%10
		res=mod_lint(aux_mod,ten);
		if(!res)
		{
			ok=0;
			goto end;
		}
		result[result_iterator]=aux_mod->x[0]+'0';
		//aux_target/=10
		res=div_lint(aux_target,ten);
		if(res==0)
		{
			ok=0;
			goto end;
		}
	}while(!(aux_target->used_size==1&&aux_target->x[0]==0));

	if(aux_target->sign)
	{
		result_iterator++;
		result[result_iterator]='-';
	}

	char*aux=realloc(result,result_iterator+1);
	if(aux)
	{
		result=aux;
	}

	//reversing the string
	for(unsigned long long i=0;i<=result_iterator/2;i++)
	{
		char aux=result[i];
		result[i]=result[result_iterator-i];
		result[result_iterator-i]=aux;
	}

	end:
	if(ok==0)
	{
		free(result);
		result=0;
	}
	if(aux_target)
		free_lint(aux_target);
	if(ten)
		free_lint(ten);
	if(aux_mod)
		free_lint(aux_mod);
	return result;
}

///==================Bitwise Operations===================
//Shift left, only works with positive amounts
int shl_lint(LINT*target,unsigned long long amount)
{
	if(!target)
	{
		return 0;
	}

	LINT* aux_target;
	aux_target=clone_lint(target);
	if(!aux_target)
	{
		printf("Error: Memory allocation failure in shl_lint\n");
		return 0;
	}

	while(amount)
	{
		amount--;
		//if the most significant bit of the last degree of
		//target is "1" add another degree to shift it into
		if(aux_target->x[aux_target->used_size-1]&0x80000000)
		{
			if(aux_target->used_size==aux_target->size)
			{
				if(expand_size(aux_target)==0)
				{
					printf("Error: memory allocation failure in shl_lint\n");
					return 0;
				}
			}
			aux_target->used_size++;
		}
		for(unsigned long long i=target->used_size;i>0;i--)
		{
			aux_target->x[i]<<=1;
			//if the following degree has a "1" as the most significant bit
			//we shift it into the current degree
			if(aux_target->x[i-1]&0x80000000)
			{
				aux_target->x[i]++;
			}
		}
		aux_target->x[0]<<=1;
	}

	free(target->x);
	*target=*aux_target;
	free(aux_target);
	return 1;
}

//Shift right, only works with positive amounts
void shr_lint(LINT*target,unsigned long long amount)
{
	if(!target)
	{
		return;
	}
	while(amount)
	{
		amount--;
		for(unsigned long long i=0;i<target->used_size-1;i++)
		{
			target->x[i]>>=1;
			//if the next coefficient is odd we shift that bit into
			//the current coefficient
			if(target->x[i+1]%2==1)
			{
				target->x[i]|=0x80000000;
			}
		}
		target->x[target->used_size-1]>>=1;
	}
	attempt_shrink(target);
}

//Shift left where amount is also a LINT, keeps track of the amount's sign
int shl_lint_lint(LINT*target,LINT*amount)
{
	if(!target||!amount)
	{
		return 0;
	}

	///if amount==0
	if(amount->used_size==1&&amount->x[0]==0)
	{
		return 1;
	}

	///if amount<0
	if(amount->sign!=0)
	{
		LINT positive_amount=*amount;
		positive_amount.sign=0;
		return shr_lint_lint(target,&positive_amount);
	}

	LINT*aux_amount=clone_lint(amount);
	if(!aux_amount)
	{
		printf("Error: memory allocation failure in shr_lint_lint\n");
		return 0;
	}
	LINT*one=new_lint_num_degree(1,0);
	if(!one)
	{
		printf("Error: memory allocation failure in shr_lint_lint\n");
		return 0;
	}
	LINT*result=clone_lint(target);
	if(!result)
	{
		printf("Error: memory allocation failure in shr_lint_lint\n");
		return 0;
	}
	while(aux_amount->used_size>1||aux_amount->x[0]>0)
	{
		if(subtract_lint(aux_amount,one)==0)
		{
			free_lint(aux_amount);
			free_lint(result);
			free_lint(one);
			return 0;
		}
		if(shl_lint(result,1)==0)
		{
			free_lint(aux_amount);
			free_lint(result);
			free_lint(one);
			return 0;
		}
	}
	free_lint(aux_amount);
	free_lint(one);
	free(target->x);
	*target=*result;
	free(result);
	return 1;
}

//Shift left where amount is also a LINT, keeps track of the amount's sign
int shr_lint_lint(LINT*target,LINT*amount)
{
	if(!target)
	{
		return 0;
	}
	if(amount->sign!=0)
	{
		LINT positive_amount=*amount;
		positive_amount.sign=0;
		return shl_lint_lint(target,&positive_amount);
	}
	LINT*aux_amount=clone_lint(amount);
	if(!aux_amount)
	{
		printf("Error: memory allocation failure in shr_lint_lint\n");
		return 0;
	}
	LINT*one=new_lint_num_degree(1,0);
	if(!one)
	{
		printf("Error: memory allocation failure in shr_lint_lint\n");
		return 0;
	}
	LINT*result=clone_lint(target);
	if(!result)
	{
		printf("Error: memory allocation failure in shr_lint_lint\n");
		return 0;
	}
	while(aux_amount->used_size>1||aux_amount->x[0]>0)
	{
		if(subtract_lint(aux_amount,one)==0)
		{
			free_lint(aux_amount);
			free_lint(result);
			free_lint(one);
			return 0;
		}
		shr_lint(result,1);
	}
	free_lint(aux_amount);
	free_lint(one);
	free(target->x);
	*target=*result;
	free(result);
	return 1;
}

//Bitwise or operation between "a" and "b", the result is saved in "a"
//a=a|b
//success => return 1
//failure => return 0
int or_lint(LINT*a,LINT*b)
{
	if(!a||!b)
	{
		return 0;
	}
	//op1=the longer between "a" and "b"
	LINT*op1,*op2;
	if(a->used_size>b->used_size)
	{
		op1=a;
		op2=b;
	}
	else
	{
		op1=b;
		op2=a;
	}

	LINT*result=clone_lint(op1);
	if(!result)
	{
		return 0;
	}

	for(unsigned long long i=0;i<result->used_size;i++)
	{
		if(i<op2->used_size)
		{
			result->x[i]|=op2->x[i];
		}
	}

	attempt_shrink(result);
	free(a->x);
	*a=*result;
	free(result);
	return 1;
}

//Bitwise and operation between "a" and "b", the result is saved in "a"
//a=a&b
//success => return 1
//failure => return 0
int and_lint(LINT*a,LINT*b)
{
	if(!a||!b)
	{
		return 0;
	}
	//op1=the longer between "a" and "b"
	LINT*op1,*op2;
	if(a->used_size>b->used_size)
	{
		op1=a;
		op2=b;
	}
	else
	{
		op1=b;
		op2=a;
	}

	LINT*result=clone_lint(op1);
	if(!result)
	{
		return 0;
	}

	for(unsigned long long i=0;i<result->used_size;i++)
	{
		if(i<op2->used_size)
		{
			result->x[i]&=op2->x[i];
		}
		else
		{
			result->x[i]=0;
		}
	}

	attempt_shrink(result);
	free(a->x);
	*a=*result;
	free(result);
	return 1;
}

//Bitwise xor operation between "a" and "b", the result is saved in "a"
//a=a^b
//success => return 1
//failure => return 0
int xor_lint(LINT*a,LINT*b)
{
	if(!a||!b)
	{
		return 0;
	}
	//op1=the longer between "a" and "b"
	LINT*op1,*op2;
	if(a->used_size>b->used_size)
	{
		op1=a;
		op2=b;
	}
	else
	{
		op1=b;
		op2=a;
	}

	LINT*result=clone_lint(op1);
	if(!result)
	{
		return 0;
	}

	for(unsigned long long i=0;i<result->used_size;i++)
	{
		if(i<op2->used_size)
		{
			result->x[i]^=op2->x[i];
		}
	}

	attempt_shrink(result);
	free(a->x);
	*a=*result;
	free(result);
	return 1;
}


//Transforms "a" into it's own complement
//a=~a
//success => return 1
//failure => return 0
int complement_lint(LINT*a)
{
	if(!a)
	{
		return 0;
	}
	for(unsigned long long i=0;i<a->used_size;i++)
	{
		a->x[i]=~a->x[i];
	}
	return 1;
}