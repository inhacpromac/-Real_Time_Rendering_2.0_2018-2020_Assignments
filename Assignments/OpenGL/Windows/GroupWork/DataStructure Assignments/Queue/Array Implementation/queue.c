//Queue - Array Implementation
//Ref - Data Structures Using C - Reema Thareja

#include <stdio.h>
#include <conio.h>

#define MAX 10

int queue[MAX];
int front = -1, rear = -1;

void enqueue(void);
int dequeue(void);
int peek(void);
void display(void);

int main()
{
    int option, val;
    do
    {
        printf("\n\n ***** MAIN MENU *****");
        printf("\n 1. Insert an element");
        printf("\n 2. Delete an element");
        printf("\n 3. Peek");
        printf("\n 4. Display the queue");
        printf("\n 5. EXIT");
        printf("\n Enter your option : ");
        scanf("%d", &option);
        switch(option)
        {
            case 1:
                enqueue();
                break;
            case 2:
                val = dequeue();
                if (val != -1)
                printf("\n The number deleted is : %d", val);
                break;
            case 3:
                val = peek();
                if (val != -1)
                printf("\n The first value in queue is : %d", val);
                break;
            case 4:
                display();
                break;           
        }
    }while(option != 5);
    getch();
    return 0;
}


void enqueue()
{
    int num;
    printf("\n Enter the number to be inserted in the queue : ");
    scanf("%d", &num);
    if (rear == MAX -1)
    printf("\n OVERFLOW");
    else if (front == -1 && rear == -1)
    front = rear = 0;
    else
    rear++;
    queue[rear] = num;
}

int dequeue()
{
    int val;
    if (front == -1 || front > rear)
    {
        printf("\n UNDERFLOW");
        return -1;
    }
    else
    {
        val = queue[front];
        front++;
        if (front > rear)
        front = rear = -1;
        return val;
    }
}

int peek()
{
    if (front == -1 || front > rear)
    {
        printf("\n QUEUE IS EMPTY");
        return -1;
    }
    else
    {
        return queue[front];
    }
}

void display()
{
    int i;
    printf("\n");
    if (front == -1 || front > rear)
    printf("\n QUEUE IS EMPTY");
    else
    {
        for (i = front; i <= rear; i++)
        printf("\t %d", queue[i]);
    }
}