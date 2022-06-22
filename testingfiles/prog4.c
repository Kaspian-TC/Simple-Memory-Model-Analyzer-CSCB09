int x = 10;
int y;
 
void f(int *p, int q)
{
    *p = 5;
}
 
int main()
{
    int i = x;

    f(&i, i);
    return 0;
}