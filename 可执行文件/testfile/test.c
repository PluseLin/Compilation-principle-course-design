//#include <stdio.h> 
int program(int a,int b,int c)
{
	int i;
	int j;
	i=0; 	
	if(a>(b+c))
	{
		j=a+(b*c+1);
	}
	else
	{
		j=a;
	}
	while(i<=100)
	{
		i=i+j*2;
	}
	return i;
}

int demo(int a)
{
	a=a+2;
	return a*2;
}

void main(void)
{
	int a[2][2];
	int b;
	a[0][0]=3;
	a[0][1]=a[0][0]+1;
	a[1][0]=a[0][0]+a[0][1];
	a[1][1]=program(a[0][0],a[0][1],demo(a[1][0]));
//	printf("a[0][0]=%d\n",a[0][0]);
//	printf("a[0][1]=%d\n",a[0][1]);
//	printf("a[1][0]=%d\n",a[1][0]);
//	printf("a[1][1]=%d\n",a[1][1]);
	return;
}



