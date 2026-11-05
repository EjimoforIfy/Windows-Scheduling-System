#include <iostream>
#include <cstdlib>

// Exception Classes
class StackException {
private:
    const char* message;
public:
    StackException(const char* msg) : message(msg) {}
    const char* what() const { return message; }
};

class QueueException {
private:
    const char* message;
public:
    QueueException(const char* msg) : message(msg) {}
    const char* what() const { return message; }
};

class SchedulerException {
private:
    const char* message;
public:
    SchedulerException(const char* msg) : message(msg) {}
    const char* what() const { return message; }
};

// Stack Implementation
class Stack {
private:
    int* elements;
    int capacity;
    int top;

public:
    Stack(int size = 100) {
        capacity = size;
        elements = new int[capacity];
        top = -1;
    }

    ~Stack() {
        delete[] elements;
    }

    void push(int value) {
        if (top >= capacity - 1) {
            throw StackException("Stack overflow");
        }
        elements[++top] = value;
    }

    int pop() {
        if (isEmpty()) {
            throw StackException("Stack underflow");
        }
        return elements[top--];
    }

    int peek() const {
        if (isEmpty()) {
            throw StackException("Stack is empty");
        }
        return elements[top];
    }

    bool isEmpty() const {
        return top == -1;
    }

    void display() const {
        if (isEmpty()) {
            std::cout << "Stack is empty" << std::endl;
            return;
        }
        std::cout << "Stack elements: ";
        for (int i = top; i >= 0; i--) {
            std::cout << elements[i] << " ";
        }
        std::cout << std::endl;
    }
};

// Process Structure
struct Process {
    int id;
    int priority;
    int cycles_waited; // For aging mechanism

    Process(int i = 0, int p = 0) : id(i), priority(p), cycles_waited(0) {}
};

// Queue Implementation
class Queue {
protected:
    static const int MAX_SIZE = 100;
    Process* elements;
    int front;
    int rear;
    int count;

public:
    Queue() {
        elements = new Process[MAX_SIZE];
        front = 0;
        rear = -1;
        count = 0;
    }

    virtual ~Queue() {
        delete[] elements;
    }

    virtual void enqueue(const Process& process) {
        if (count >= MAX_SIZE) {
            throw QueueException("Queue overflow");
        }
        rear = (rear + 1) % MAX_SIZE;
        elements[rear] = process;
        count++;
    }

    virtual Process dequeue() {
        if (isEmpty()) {
            throw QueueException("Queue underflow");
        }
        Process temp = elements[front];
        front = (front + 1) % MAX_SIZE;
        count--;
        return temp;
    }

    Process peek() const {
        if (isEmpty()) {
            throw QueueException("Queue is empty");
        }
        return elements[front];
    }

    bool isEmpty() const {
        return count == 0;
    }

    int size() const {
        return count;
    }

    virtual void display() const {
        if (isEmpty()) {
            std::cout << "Queue is empty" << std::endl;
            return;
        }
        std::cout << "Queue elements (ID, Priority): ";
        int current = front;
        for (int i = 0; i < count; i++) {
            std::cout << "(" << elements[current].id << ", " << elements[current].priority << ") ";
            current = (current + 1) % MAX_SIZE;
        }
        std::cout << std::endl;
    }
};

// Priority Scheduler Implementation (Heap-based)
class Scheduler : public Queue {
private:
    Process* heap;
    int heap_size;
    int heap_capacity;
    int agingCounter;
    static const int AGING_THRESHOLD = 3;

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[index].priority <= heap[parent].priority) break;
            Process temp = heap[index];
            heap[index] = heap[parent];
            heap[parent] = temp;
            index = parent;
        }
    }

    void heapifyDown(int index) {
        int largest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < heap_size && heap[left].priority > heap[largest].priority) {
            largest = left;
        }
        if (right < heap_size && heap[right].priority > heap[largest].priority) {
            largest = right;
        }
        if (largest != index) {
            Process temp = heap[index];
            heap[index] = heap[largest];
            heap[largest] = temp;
            heapifyDown(largest);
        }
    }

    void ageProcesses() {
        for (int i = 0; i < heap_size; i++) {
            if (heap[i].priority < 10 && heap[i].cycles_waited >= AGING_THRESHOLD) {
                heap[i].priority++;
                heap[i].cycles_waited = 0;
                heapifyUp(i);
            }
        }
        agingCounter = 0;
    }

public:
    Scheduler() : Queue(), heap_size(0), heap_capacity(100), agingCounter(0) {
        heap = new Process[heap_capacity];
    }

    ~Scheduler() {
        delete[] heap;
    }

    void enqueue(const Process& process) override {
        if (process.priority < 1 || process.priority > 10) {
            throw SchedulerException("Invalid priority level (must be 1-10)");
        }
        if (heap_size >= heap_capacity) {
            throw SchedulerException("Scheduler overflow");
        }
        Process p = process;
        p.cycles_waited = 0;
        heap[heap_size] = p;
        heapifyUp(heap_size);
        heap_size++;
        Queue::enqueue(process); // Maintain base Queue for display
    }

    Process dequeue() override {
        if (heap_size == 0) {
            throw SchedulerException("Scheduler underflow");
        }
        Process result = heap[0];
        heap[0] = heap[heap_size - 1];
        heap_size--;
        if (heap_size > 0) {
            heapifyDown(0);
        }
        Queue::dequeue();
        for (int i = 0; i < heap_size; i++) {
            heap[i].cycles_waited++;
        }
        agingCounter++;
        if (agingCounter >= AGING_THRESHOLD) {
            ageProcesses();
        }
        return result;
    }

    void display() const override {
        if (heap_size == 0) {
            std::cout << "Scheduler is empty" << std::endl;
            return;
        }
        std::cout << "Scheduler processes (ID, Priority): ";
        for (int i = 0; i < heap_size; i++) {
            std::cout << "(" << heap[i].id << ", " << heap[i].priority << ") ";
        }
        std::cout << std::endl;
    }
};

// Test function for demonstration and video
void runTests() {
    std::cout << "\n=== TESTING STACK IMPLEMENTATION ===\n";
    Stack stack;
    try {
        std::cout << "Pushing elements to stack: 10, 20, 30, 40, 50" << std::endl;
        stack.push(10);
        stack.push(20);
        stack.push(30);
        stack.push(40);
        stack.push(50);
        stack.display();

        std::cout << "Popping two elements" << std::endl;
        std::cout << "Popped: " << stack.pop() << std::endl;
        std::cout << "Popped: " << stack.pop() << std::endl;
        stack.display();

        std::cout << "Peek at top element: " << stack.peek() << std::endl;
    }
    catch (const StackException& e) {
        std::cout << "Stack Exception: " << e.what() << std::endl;
    }

    std::cout << "\n=== TESTING QUEUE IMPLEMENTATION ===\n";
    Queue queue;
    try {
        std::cout << "Enqueuing processes to queue" << std::endl;
        queue.enqueue(Process(101, 3));
        queue.enqueue(Process(102, 5));
        queue.enqueue(Process(103, 1));
        queue.enqueue(Process(104, 7));
        queue.display();

        std::cout << "Dequeuing two processes" << std::endl;
        Process p1 = queue.dequeue();
        std::cout << "Dequeued: (ID: " << p1.id << ", Priority: " << p1.priority << ")" << std::endl;
        Process p2 = queue.dequeue();
        std::cout << "Dequeued: (ID: " << p2.id << ", Priority: " << p2.priority << ")" << std::endl;
        queue.display();
    }
    catch (const QueueException& e) {
        std::cout << "Queue Exception: " << e.what() << std::endl;
    }

    std::cout << "\n=== TESTING PRIORITY SCHEDULER IMPLEMENTATION ===\n";
    Scheduler scheduler;
    try {
        std::cout << "Enqueuing processes with different priorities" << std::endl;
        scheduler.enqueue(Process(201, 3));
        scheduler.enqueue(Process(202, 7));
        scheduler.enqueue(Process(203, 1));
        scheduler.enqueue(Process(204, 5));
        scheduler.enqueue(Process(205, 9));
        scheduler.display();

        std::cout << "Dequeuing processes (in priority order)" << std::endl;
        for (int i = 0; i < 3; i++) {
            Process p = scheduler.dequeue();
            std::cout << "Dequeued: (ID: " << p.id << ", Priority: " << p.priority << ")" << std::endl;
            scheduler.display();
        }

        std::cout << "\nTesting priority aging (prevents starvation)" << std::endl;
        scheduler.enqueue(Process(206, 8));
        scheduler.enqueue(Process(207, 2));
        scheduler.display();

        std::cout << "Dequeuing to show aging mechanism" << std::endl;
        for (int i = 0; i < 4; i++) {
            Process p = scheduler.dequeue();
            std::cout << "Dequeued: (ID: " << p.id << ", Priority: " << p.priority << ")" << std::endl;
            if (!scheduler.isEmpty()) {
                scheduler.display();
            }
        }
    }
    catch (const SchedulerException& e) {
        std::cout << "Scheduler Exception: " << e.what() << std::endl;
    }

    std::cout << "\n=== TESTING EXCEPTION HANDLING ===\n";
    try {
        Stack s;
        std::cout << "Attempting to pop from empty stack..." << std::endl;
        s.pop();
    }
    catch (const StackException& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    try {
        Queue q;
        std::cout << "Attempting to dequeue from empty queue..." << std::endl;
        q.dequeue();
    }
    catch (const QueueException& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    try {
        Scheduler s;
        std::cout << "Attempting to enqueue process with invalid priority (11)..." << std::endl;
        s.enqueue(Process(301, 11));
    }
    catch (const SchedulerException& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "*** Windowless 9 Process Scheduling System ***\n";
    std::cout << "   Developed by Operating Systems Ltd.\n";
    std::cout << "   Student ID: 30108586\n\n";

    runTests();
    return 0;
}