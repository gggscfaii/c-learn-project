#include <stdio.h>

int getMonDays(int year, int month)
{
    if (month == 2)//二月
    {
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))//闰年29天
        {
            return 29;
        }
        else
        {
            return 28;
        }
    }
    else if (month == 4 || month == 6 || month == 9 || month == 11)//每月有30天的月份
    {
        return 30;
    }
    else//每月有31天的月份
    {
        return 31;
    }
}

int getTotalDays(int year, int month)
{

    int totalDays = 0;
    for (int y = 1900; y < year; y++) {
        if ((y % 4 ==0 && y % 100 !=0) && y % 400 == 0) {
            totalDays += 366;
        } else {
            totalDays += 365;
        }
    }

    for(int m =1; m < month; m++){
        totalDays += getMonDays(year, m);
    }
    totalDays+=1;
    return totalDays;
}

int getWeek(int totalDays)
{
    int week= totalDays%7;
    if(week == 6){
        return 0;
    }
    return week+1;
}

void print(int totalDays, int week, int monDays)
{
    printf("日 一 二 三 四 五 六\n");
    for(int i=1; i<=week; i++){
        printf("   ");
    }

    for (int i = 1; i <= monDays; i++) {
        week= getWeek(totalDays);
        if (week == 6) {
            printf("%2d\n",i);
        } else {
            printf("%2d ",i);
        }
        totalDays++;
    }
    printf("\n");
}

int main(int argc, const char *argv[])
{
    int year, month;
    printf("please input year,month:");
    scanf("%d,%d", &year, &month);

    int totalDays = getTotalDays(year,month);
    int week = getWeek(totalDays);
    int monDays= getMonDays(year,month);

    printf("days=%d,week=%d,monDays=%d\n", totalDays, week, monDays);
    print(totalDays, week, monDays);
    return 0;
}
