int x = 10;
int y;
 
int f(int p, int q)
{
    int j = 5;

    x = 5;
    return p * q + j;
}
 
int main()
{
   int i = x;
 
   y = f(i, i);
   return 0;
}