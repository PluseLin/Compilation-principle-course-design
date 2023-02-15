//#include<stdio.h>
//ц╟ещеепР 
int a[5];
int main(){
	int i;
	int j;
	int temp;
	
	a[0]=1;
	a[1]=2;
	a[2]=4;
	a[3]=5;
	a[4]=2;
	while(i<5){
		j=i+1;
		while(j<5){
			if(a[i]>a[j]){
				temp=a[i];a[i]=a[j];a[j]=temp;
			}
			j=j+1;
		}
		
		i=i+1;
	}
	//printf("%d\n",a[4]);
	return 0;
}
