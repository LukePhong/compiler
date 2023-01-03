/*float aa[2][2]= {1,2,5,6};
int aaa[1] = {1};
int xbb[2][2] = {{1,2},{5,6}};
int omg[1][1][2] = {{{1,2}}};
int glb = 27;
int main(){

  int a[2][3][4] = {1,2,3,4,5,6};//,7,8};
  float b[2]= {1,2};
  int tmp;
  tmp = a[1][2][3] + aaa[0];

  return 0;
}*/
/*int main(){
    const int a[4][2] = {{1, 2}, {3, 4}, {}, 7};
    const int N = 3;
    int b[4][2] = {};
    int c[4][2] = {1, 2, 3, 4, 5, 6, 7, 8};
    int d[N + 1][2] = {1, 2, {3}, {5}, a[3][0], 8};
    int e[4][2][1] = {{d[2][1], {c[2][1]}}, {3, 4}, {5, 6}, {7, 8}};
    return e[3][1][0] + e[0][0][0] + e[0][1][0] + d[3][0];
}*/
int maxArea(int height[], int n) {
    int i;
    int j;
    i = 0;
    j = n - 1;
    int max_val;
    max_val = -1;
    while(i < j){
        int area;
        if(height[i] < height[j])
            area = (j - i) * height[i];
        else
            area = (j - i) * height[j];
        if(area > max_val){
            max_val = area;
        }
        if(height[i] > height[j])
            j = j - 1;
        else
            i = i + 1;
    }
    return max_val;
}

int main(){
    int res;
    int a[10];
    a[0]=3;a[1]=3;a[2]=9;a[3]=0;a[4]=0;
    a[5]=1;a[6]=1;a[7]=5;a[8]=7;a[9]=8;
    res = 10;
    res = maxArea(a, res);
    return res;
}
