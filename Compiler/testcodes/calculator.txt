//输入数据num1,num2,op，根据op确定操作进行运算，最后输出运算结果ans
int num1,num2,op,ans;
get(num1,num2,op);
if(op==0)
{
    ans = num1 + num2;
};
if(op==1)
{
    ans = num1 - num2;
};
if(op==2)
{
    ans = num1 & num2;
};
if(op==3)
{
    ans = num1 | num2;
};
put(ans);
