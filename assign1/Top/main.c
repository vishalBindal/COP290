#include <stdio.h>
extern int factorial();
extern int random_python();
extern int ruby_time();
extern int sum();
int	main(int argc, char *argv[]){
	int	a=5;
	int b= factorial(a);
	printf("Factorial of %d is %d\n", a, b);
	
	ruby_time();
	
	random_python(1,argv);

	int c=128,d=1001;
	int s=sum(c, d);
	printf("\nSum of %d and %d is %d\n", c, d, s);
}