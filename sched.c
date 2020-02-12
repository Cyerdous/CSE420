#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define null NULL

typedef struct Process
{
    unsigned int Pid;
    unsigned int ArrivalTime;
    unsigned int BurstTime;
    unsigned int Priority;
    unsigned int FinishTime;
    unsigned int WaitingTime;
} Process;

typedef struct Node
{
    Process* process;
    struct Node* prev;
    struct Node* next;
} Node;

typedef struct LL
{
    Node* head;
    Node* tail;
} LL;

Process* ReadLine(FILE* file)
{
    Process* new = malloc(sizeof(Process));
    unsigned int pid, at, bt, p;
    if(fscanf(file, "%d %d %d %d", &pid, &at, &bt, &p) != 4)
    {
        return null;
    }
    new->Pid = pid;
    new->ArrivalTime = at;
    new->BurstTime = bt;
    new->Priority = p;
    new->FinishTime = 0;
    new->WaitingTime = 0;
    return new;
    
}

void AddToLL(LL* list, Node* node)
{
    if(list->head == null && list->tail == null)
    {
        list->head = node;
        list->tail = node;
		node->next = null;
		node->prev = null;
    }
    else 
    {
        Node* add = list->tail;
        node->prev = add;
        node->next = null;
        add->next = node;
        list->tail = node;
    }
}

LL* ReadFile(FILE* file, int limit)
{
    LL* processes = (LL*)malloc(sizeof(LL));
    processes->head = null;
    processes->tail = null;
    int i;
    for(i = 0; i < limit || limit == 0; i++)
    { 
        Process* procToAdd = ReadLine(file);
        if(procToAdd == null)
        {
            break;
        }
        Node* nodeToAdd = malloc(sizeof(Node));
        nodeToAdd->process = procToAdd;
        AddToLL(processes, nodeToAdd);
    }
    return processes;
}

void WriteProccessToFile(Process* proc, FILE* out)
{
    fprintf(out, "%d %d %d %d \n", proc->Pid, proc->ArrivalTime, proc->FinishTime, proc->WaitingTime);
}

void FirstComeFirstServe(LL* list, FILE* out)
{
    int i;
    for(i = 0; list->head != null; i++)
    {
        if(list->head == null)
        {
            break;
        }
        //decrement burst time of first arrival, decrement wait time
        Node* process = list->head;
        
        while(process != null && process->process != null)
        {
            if(process->process->ArrivalTime <= i)
            {
                process->process->WaitingTime++;
            }            
            process = process->next;
        }
        Node* priority = list->head;
        if(priority != null && priority->process != null)
        {
            priority->process->WaitingTime--;
            priority->process->BurstTime--;

            //if bursttime becomes 0, remove it and write info to file
            if(priority->process->BurstTime == 0)
            {
                priority->process->FinishTime = i+1;
                WriteProccessToFile(priority->process, out);
                if(priority->next != null && priority->prev != null)
                {
                    priority->next->prev = priority->prev;
                    priority->prev->next = priority->next;
                }
                else if(priority->next == null && priority->prev != null)
                {
                    priority->prev->next = null;
                    list->tail = priority->prev;
                }
                else if(priority->prev == null && priority->next != null)
                {
                    priority->next->prev = null;
                    list->head = priority->next;
                }
                else
                {
                    list->head = null;
                    list->tail = null;
                }
                free(priority->process);
                free(priority);
            }
        }
    }
}

void PreemptivePriority(LL* list, FILE* out)
{
    int i;
    for(i = 0; 1; i++)
    {
        if(list->head == null)
        {
            break;
        }
        //decrement burst time of best priority, decrement wait time
        Node* process = list->head;
        Node* priority = list->head;
        while(process != null)
        {
            if(priority->process->Priority > process->process->Priority)
            {
                priority = process;
            }
            process = process->next;
        }
        priority->process->BurstTime--;

        //if bursttime becomes 0, remove it and write info to file
        if(priority->process->BurstTime == 0)
        {
            priority->process->FinishTime = i;
            WriteProccessToFile(priority->process, out);
            free(priority->process);
            if(priority->next != null && priority->prev != null)
            {
                priority->next->prev = priority->prev;
                priority->prev->next = priority->next;
            }
            else if(priority->next == null && priority->prev != null)
            {
                priority->prev->next = null;
            }
            else if(priority->prev == null && priority->next != null)
            {
                priority->next->prev = null;
            }
            free(priority);
        }
        
        //increment wait time of all processes
        process = list->head;
        while(process != null)
        {
            if(process->process->ArrivalTime >= i)
            {
                process->process->WaitingTime++;
            }            
            process = process->next;
        }
        if(priority != null)
        {
            priority->process->WaitingTime--;
        }
    }
}

int main(int argc, char* argv[])
{
    // Open Files
    FILE* in = fopen(argv[1], "r");
    FILE* out = fopen(argv[2], "w");

    // Create data structure
    
    LL* processes;
    int limit = 0;
    if(argc == 5)
    {
        sscanf(argv[4], "%d", &limit);
    }
        
    processes = ReadFile(in, limit);   

    // Run process and write to file
    if(strcmp(argv[3], "FCFS") == 0)
    {
        FirstComeFirstServe(processes, out);
    }
    else if(strcmp(argv[3], "PP") == 0)
    {
        PreemptivePriority(processes, out);
    }
    fclose(in);
    fclose(out);
    free(processes);
    return 0;
}