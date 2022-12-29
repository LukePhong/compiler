int a;
int func(int p){
    //int p;
	p = p - 1;
	return p;
}
int main(){
	int b;
	a = 10;
	b = func(a);
    //b = 1;
	return b;
}
