float aa[2][2]= {1,2,5,6};
int aaa[1] = {1};
int glb = 27;
int main(){

  int a[2][3][4];
  float b[2]= {1,2};
  aa[0][0] = b[0] + aaa[0] + glb;

  return aa[0][0];
}