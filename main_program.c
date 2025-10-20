#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 10

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    int start_time;
} Process;

typedef struct {
    int time;
    int pid;
    char state[20];
} Event;

Event events[1000];
int event_count = 0;

void add_event(int time, int pid, const char* state) {
    events[event_count].time = time;
    events[event_count].pid = pid;
    strcpy(events[event_count].state, state);
    event_count++;
}

void fcfs(Process proc[], int n) {
    int current_time = 0;
    event_count = 0;
    
    // Sort by arrival time
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (proc[j].arrival_time > proc[j + 1].arrival_time) {
                Process temp = proc[j];
                proc[j] = proc[j + 1];
                proc[j + 1] = temp;
            }
        }
    }
    
    for (int i = 0; i < n; i++) {
        if (current_time < proc[i].arrival_time) {
            add_event(current_time, -1, "idle");
            current_time = proc[i].arrival_time;
        }
        
        proc[i].start_time = current_time;
        add_event(current_time, proc[i].pid, "running");
        
        current_time += proc[i].burst_time;
        proc[i].completion_time = current_time;
        proc[i].turnaround_time = proc[i].completion_time - proc[i].arrival_time;
        proc[i].waiting_time = proc[i].turnaround_time - proc[i].burst_time;
        
        add_event(current_time, proc[i].pid, "completed");
    }
}

void sjf_non_preemptive(Process proc[], int n) {
    int current_time = 0, completed = 0;
    int is_completed[MAX_PROCESSES] = {0};
    event_count = 0;
    
    while (completed < n) {
        int idx = -1, min_burst = 999999;
        
        for (int i = 0; i < n; i++) {
            if (proc[i].arrival_time <= current_time && !is_completed[i]) {
                if (proc[i].burst_time < min_burst) {
                    min_burst = proc[i].burst_time;
                    idx = i;
                }
            }
        }
        
        if (idx != -1) {
            proc[idx].start_time = current_time;
            add_event(current_time, proc[idx].pid, "running");
            
            current_time += proc[idx].burst_time;
            proc[idx].completion_time = current_time;
            proc[idx].turnaround_time = proc[idx].completion_time - proc[idx].arrival_time;
            proc[idx].waiting_time = proc[idx].turnaround_time - proc[idx].burst_time;
            
            add_event(current_time, proc[idx].pid, "completed");
            is_completed[idx] = 1;
            completed++;
        } else {
            add_event(current_time, -1, "idle");
            current_time++;
        }
    }
}

void sjf_preemptive(Process proc[], int n) {
    int current_time = 0, completed = 0;
    int is_completed[MAX_PROCESSES] = {0};
    Process temp[MAX_PROCESSES];
    event_count = 0;
    
    for (int i = 0; i < n; i++) {
        temp[i] = proc[i];
        temp[i].remaining_time = proc[i].burst_time;
        temp[i].start_time = -1;
    }
    
    int prev_pid = -1;
    
    while (completed < n) {
        int idx = -1, min_remaining = 999999;
        
        for (int i = 0; i < n; i++) {
            if (temp[i].arrival_time <= current_time && !is_completed[i]) {
                if (temp[i].remaining_time < min_remaining) {
                    min_remaining = temp[i].remaining_time;
                    idx = i;
                }
            }
        }
        
        if (idx != -1) {
            if (temp[idx].start_time == -1) {
                temp[idx].start_time = current_time;
            }
            
            if (prev_pid != temp[idx].pid) {
                add_event(current_time, temp[idx].pid, "running");
                prev_pid = temp[idx].pid;
            }
            
            temp[idx].remaining_time--;
            current_time++;
            
            if (temp[idx].remaining_time == 0) {
                temp[idx].completion_time = current_time;
                temp[idx].turnaround_time = temp[idx].completion_time - temp[idx].arrival_time;
                temp[idx].waiting_time = temp[idx].turnaround_time - temp[idx].burst_time;
                add_event(current_time, temp[idx].pid, "completed");
                is_completed[idx] = 1;
                completed++;
                prev_pid = -1;
            }
        } else {
            if (prev_pid != -1) {
                add_event(current_time, -1, "idle");
                prev_pid = -1;
            }
            current_time++;
        }
    }
    
    for (int i = 0; i < n; i++) {
        proc[i] = temp[i];
    }
}

void priority_preemptive(Process proc[], int n) {
    int current_time = 0, completed = 0;
    int is_completed[MAX_PROCESSES] = {0};
    Process temp[MAX_PROCESSES];
    event_count = 0;
    
    for (int i = 0; i < n; i++) {
        temp[i] = proc[i];
        temp[i].remaining_time = proc[i].burst_time;
        temp[i].start_time = -1;
    }
    
    int prev_pid = -1;
    
    while (completed < n) {
        int idx = -1, highest_priority = 999999;
        
        for (int i = 0; i < n; i++) {
            if (temp[i].arrival_time <= current_time && !is_completed[i]) {
                if (temp[i].priority < highest_priority) {
                    highest_priority = temp[i].priority;
                    idx = i;
                }
            }
        }
        
        if (idx != -1) {
            if (temp[idx].start_time == -1) {
                temp[idx].start_time = current_time;
            }
            
            if (prev_pid != temp[idx].pid) {
                add_event(current_time, temp[idx].pid, "running");
                prev_pid = temp[idx].pid;
            }
            
            temp[idx].remaining_time--;
            current_time++;
            
            if (temp[idx].remaining_time == 0) {
                temp[idx].completion_time = current_time;
                temp[idx].turnaround_time = temp[idx].completion_time - temp[idx].arrival_time;
                temp[idx].waiting_time = temp[idx].turnaround_time - temp[idx].burst_time;
                add_event(current_time, temp[idx].pid, "completed");
                is_completed[idx] = 1;
                completed++;
                prev_pid = -1;
            }
        } else {
            if (prev_pid != -1) {
                add_event(current_time, -1, "idle");
                prev_pid = -1;
            }
            current_time++;
        }
    }
    
    for (int i = 0; i < n; i++) {
        proc[i] = temp[i];
    }
}

void round_robin(Process proc[], int n, int quantum) {
    int current_time = 0, completed = 0;
    int is_completed[MAX_PROCESSES] = {0};
    Process temp[MAX_PROCESSES];
    int queue[MAX_PROCESSES * 10], front = 0, rear = 0;
    int in_queue[MAX_PROCESSES] = {0};
    event_count = 0;
    
    for (int i = 0; i < n; i++) {
        temp[i] = proc[i];
        temp[i].remaining_time = proc[i].burst_time;
        temp[i].start_time = -1;
    }
    
    for (int i = 0; i < n; i++) {
        if (temp[i].arrival_time == 0) {
            queue[rear++] = i;
            in_queue[i] = 1;
        }
    }
    
    while (completed < n) {
        if (front == rear) {
            add_event(current_time, -1, "idle");
            current_time++;
            for (int i = 0; i < n; i++) {
                if (temp[i].arrival_time <= current_time && !is_completed[i] && !in_queue[i]) {
                    queue[rear++] = i;
                    in_queue[i] = 1;
                }
            }
            continue;
        }
        
        int idx = queue[front++];
        in_queue[idx] = 0;
        
        if (temp[idx].start_time == -1) {
            temp[idx].start_time = current_time;
        }
        
        add_event(current_time, temp[idx].pid, "running");
        
        int exec_time = (temp[idx].remaining_time < quantum) ? temp[idx].remaining_time : quantum;
        temp[idx].remaining_time -= exec_time;
        current_time += exec_time;
        
        for (int i = 0; i < n; i++) {
            if (temp[i].arrival_time <= current_time && !is_completed[i] && !in_queue[i] && i != idx) {
                queue[rear++] = i;
                in_queue[i] = 1;
            }
        }
        
        if (temp[idx].remaining_time == 0) {
            temp[idx].completion_time = current_time;
            temp[idx].turnaround_time = temp[idx].completion_time - temp[idx].arrival_time;
            temp[idx].waiting_time = temp[idx].turnaround_time - temp[idx].burst_time;
            add_event(current_time, temp[idx].pid, "completed");
            is_completed[idx] = 1;
            completed++;
        } else {
            queue[rear++] = idx;
            in_queue[idx] = 1;
        }
    }
    
    for (int i = 0; i < n; i++) {
        proc[i] = temp[i];
    }
}

void write_json(Process proc[], int n, const char* algo) {
    FILE *fp = fopen("output.json", "w");
    if (!fp) {
        printf("Error opening file!\n");
        return;
    }
    
    fprintf(fp, "{\n");
    fprintf(fp, "  \"algorithm\": \"%s\",\n", algo);
    fprintf(fp, "  \"processes\": [\n");
    
    for (int i = 0; i < n; i++) {
        fprintf(fp, "    {\n");
        fprintf(fp, "      \"pid\": %d,\n", proc[i].pid);
        fprintf(fp, "      \"arrival_time\": %d,\n", proc[i].arrival_time);
        fprintf(fp, "      \"burst_time\": %d,\n", proc[i].burst_time);
        fprintf(fp, "      \"priority\": %d,\n", proc[i].priority);
        fprintf(fp, "      \"completion_time\": %d,\n", proc[i].completion_time);
        fprintf(fp, "      \"turnaround_time\": %d,\n", proc[i].turnaround_time);
        fprintf(fp, "      \"waiting_time\": %d\n", proc[i].waiting_time);
        fprintf(fp, "    }%s\n", (i < n - 1) ? "," : "");
    }
    
    fprintf(fp, "  ],\n");
    fprintf(fp, "  \"events\": [\n");
    
    for (int i = 0; i < event_count; i++) {
        fprintf(fp, "    {\n");
        fprintf(fp, "      \"time\": %d,\n", events[i].time);
        fprintf(fp, "      \"pid\": %d,\n", events[i].pid);
        fprintf(fp, "      \"state\": \"%s\"\n", events[i].state);
        fprintf(fp, "    }%s\n", (i < event_count - 1) ? "," : "");
    }
    
    fprintf(fp, "  ]\n");
    fprintf(fp, "}\n");
    
    fclose(fp);
}

int main() {
    Process proc[MAX_PROCESSES];
    int n, choice;
    
    printf("=================================================\n");
    printf("   CPU SCHEDULING ALGORITHM SIMULATOR\n");
    printf("=================================================\n\n");
    
    printf("Enter the number of processes (2-%d): ", MAX_PROCESSES);
    scanf("%d", &n);
    
    if (n < 2 || n > MAX_PROCESSES) {
        printf("Invalid number of processes!\n");
        return 1;
    }
    
    printf("\n--- Enter Process Details ---\n\n");
    
    for (int i = 0; i < n; i++) {
        proc[i].pid = i + 1;
        printf("Process P%d:\n", proc[i].pid);
        printf("  Arrival Time: ");
        scanf("%d", &proc[i].arrival_time);
        printf("  Burst Time: ");
        scanf("%d", &proc[i].burst_time);
        proc[i].priority = 0;
        proc[i].remaining_time = proc[i].burst_time;
        proc[i].completion_time = 0;
        proc[i].turnaround_time = 0;
        proc[i].waiting_time = 0;
        proc[i].start_time = -1;
        printf("\n");
    }
    
    printf("\n=================================================\n");
    printf("Select Scheduling Algorithm:\n");
    printf("=================================================\n");
    printf("1. First Come First Serve (FCFS)\n");
    printf("2. Shortest Job First - Non-Preemptive (SJF)\n");
    printf("3. Shortest Job First - Preemptive (SRTF)\n");
    printf("4. Priority Scheduling (Preemptive)\n");
    printf("5. Round Robin (RR)\n");
    printf("=================================================\n");
    printf("Enter your choice (1-5): ");
    
    scanf("%d", &choice);
    
    switch(choice) {
        case 1:
            fcfs(proc, n);
            write_json(proc, n, "FCFS");
            printf("\n✓ FCFS scheduling completed!\n");
            break;
            
        case 2:
            sjf_non_preemptive(proc, n);
            write_json(proc, n, "SJF_Non_Preemptive");
            printf("\n✓ SJF (Non-Preemptive) scheduling completed!\n");
            break;
            
        case 3:
            sjf_preemptive(proc, n);
            write_json(proc, n, "SJF_Preemptive");
            printf("\n✓ SJF (Preemptive/SRTF) scheduling completed!\n");
            break;
            
        case 4:
            printf("\n--- Enter Priority for each process (lower number = higher priority) ---\n");
            for (int i = 0; i < n; i++) {
                printf("Priority for Process P%d: ", proc[i].pid);
                scanf("%d", &proc[i].priority);
            }
            priority_preemptive(proc, n);
            write_json(proc, n, "Priority_Preemptive");
            printf("\n✓ Priority (Preemptive) scheduling completed!\n");
            break;
            
        case 5:
            printf("\nEnter Time Quantum: ");
            int quantum;
            scanf("%d", &quantum);
            round_robin(proc, n, quantum);
            write_json(proc, n, "Round_Robin");
            printf("\n✓ Round Robin scheduling completed!\n");
            break;
            
        default:
            printf("\n✗ Invalid choice!\n");
            return 1;
    }
    
    printf("\n=================================================\n");
    printf("  Results Summary:\n");
    printf("=================================================\n");
    printf("PID\tAT\tBT\tCT\tTAT\tWT\n");
    printf("-------------------------------------------------\n");
    
    float total_tat = 0, total_wt = 0;
    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n", 
               proc[i].pid, 
               proc[i].arrival_time, 
               proc[i].burst_time,
               proc[i].completion_time,
               proc[i].turnaround_time,
               proc[i].waiting_time);
        total_tat += proc[i].turnaround_time;
        total_wt += proc[i].waiting_time;
    }
    
    printf("-------------------------------------------------\n");
    printf("Average Turnaround Time: %.2f\n", total_tat / n);
    printf("Average Waiting Time: %.2f\n", total_wt / n);
    printf("=================================================\n\n");
    
    printf("✓ output.json file has been generated successfully!\n");
    printf("✓ Now open index.html and upload the output.json file to visualize.\n\n");
    
    return 0;
}