int main()  
{  
    int i = '7';  
    int j = 9;  
    int c[5] =   
  
{2,10,10,19,3};

    for (i=0;i<20;i++)  
    {  
        for(j=i+1;j<20;j--)  
        {  
            if(j == 19)  
            {  
                c[i] = j;  
            }
        }  
    }  
    printf("Hello world");  
    return 0;  
}  