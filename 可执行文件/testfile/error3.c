//变量重命名
//int func1(){
//	int a;
//	int b;
//	int a;
//	return 0;
//} 


//变量未命名 
//int func2(){
//	a=0;
//	return 0;
//}

//数组维数错误 
//int func3(){
//	int a[1][1];
//	a[1]=1;
//	return 0;
//}

//函数未声明
int func4(){
        return decl();
}

//函数返回值错误
//void decl1(){
//	int a;
//	a=0;
//} 
//
//int func5(){
//	int a;
//	a=decl1();
//}

//函数重命名
//void func6(){
//	
//} 
//
//void func6(){
//	
//}

//调用函数参数错误
//int decl2(int a){
//	return a+1;
//} 
//
//int func7(){
//	int a;
//	a=0;
//	a=decl2(a,1);
//	return 0;
//}

//函数返回值错误
//void func8(){
//	return 0;
//}

//int func9(){
//	return;
//}
