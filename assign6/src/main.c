#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float masterClock = 0;
static int shortestTellerLine;
int NO_OF_CUSTOMERS;
int NO_OF_TELLERS;
float SIMULATION_TIME;
float AVERAGE_SERVICE_TIME;
float totalTellerServiceTime = 0;
float totalTellerIdleTime = 0;
float maxCustomerWaitTime = -1;
int noOfCustomersServed = 0;

int interactive = 0;
int actionSummaries = 0;

FILE* pldata;

struct Node{
    void* value;
    struct Node* next;
    struct Node* prev;
};

typedef struct Node Node;

Node* createNode(void* value){
    Node* newnode = (Node*) malloc(sizeof(Node));
    newnode->value = value;
    newnode->next= NULL;
    newnode->prev= NULL;
    return newnode;
}

struct Queue{
    Node *head;
    Node *tail;
    int noOfNodes;
    void (*addAtTail)(struct Queue*, Node*);
    void (*addAppropriate)(struct Queue*, Node*);
    Node* (*pop)(struct Queue*);
};

typedef struct Queue Queue;

void addAtTail(Queue* queue, Node* node)
{
    if(queue->tail == NULL)
        queue->tail = queue->head = node;
    else
    {
        queue->tail->next = node;
        node->prev = queue->tail;
        queue->tail = node;
    }
    queue->noOfNodes++;
}

Node* pop(Queue* queue)
{
    if(queue->head == NULL) return NULL;
    Node* node = queue->head;
    queue->head = queue->head->next;
    if(queue->head!=NULL)
        queue->head->prev = NULL;
    else queue->tail = NULL;
    queue->noOfNodes--;
    return node;

}

Queue* createQueue()
{
    Queue* queue = (Queue*) malloc(sizeof(Queue));
    queue->noOfNodes=0;
    queue->addAtTail = &addAtTail;
    queue->addAppropriate = NULL;
    queue->pop = &pop;
    return queue;
}

struct Customer{
    int id;
    float arrivalTime;
    float waitingTime;
    float serviceTime;
};

typedef struct Customer Customer;

Customer* createCustomer(int id, float arrivalTime)
{
    Customer* customer = (Customer*) malloc(sizeof(Customer));
    customer->id = id;
    customer->arrivalTime = arrivalTime;
    customer-> waitingTime = -1;
    customer->serviceTime = -1;
    return customer;
}

struct Teller{
    int id;
    int currentCustomerID;
//    float (*getServiceTime)();
//    float (*getIdleTime)();
};

typedef struct Teller Teller;

float getServiceTime()
{
    float serviceTime = 2*AVERAGE_SERVICE_TIME*rand()/(float)(RAND_MAX);
    return serviceTime;
}

float getIdleTime()
{
    return (float) (1 + rand()%150);
}

Teller* createTeller(int id)
{
    Teller *teller = (Teller*) malloc(sizeof(Teller));
    teller->id = id;
    teller->currentCustomerID = -1;
    return teller;
}


struct TellerQueue{
    int noOfQueues;
    void (*add)(struct TellerQueue*, Customer*);
    Customer* (*pop)(struct TellerQueue*, int);
    Queue** line; // array of Queue*
};

typedef struct TellerQueue TellerQueue;

int getShortestTellerLine(TellerQueue* tellerQueue)
{
    // improvement: keep array of queues sorted, change pos of changed queue only
    int minline=-1;
    int minvalue=-1;
    int count=1;
    for(int i=0;i<tellerQueue->noOfQueues;i++)
    {
        int noOfCustomers = tellerQueue->line[i]->noOfNodes;
        if(minvalue==-1 || noOfCustomers < minvalue)
        {
            minvalue = noOfCustomers;
            minline = i;
            count = 1;
        }
        else if(noOfCustomers == minvalue)
            count++;
    }
    if(count==1) {
        return minline;
    }
    int shortestLines[count];
    for(int k=0,i=0;k<count && i<tellerQueue->noOfQueues;i++)
        if(tellerQueue->line[i]->noOfNodes == minvalue)
        {
            shortestLines[k] = i;
            ++k;
        }
    int value = shortestLines[rand()%count];
    return value;
}

void addCustomer(TellerQueue* tellerQueue, Customer* customer)
{
    Node* customerNode = createNode(customer);
    if(tellerQueue->noOfQueues==1)
        shortestTellerLine = 0;
    else
        shortestTellerLine = getShortestTellerLine(tellerQueue);
    Queue* queToAdd = tellerQueue->line[shortestTellerLine];
    queToAdd->addAtTail(queToAdd, customerNode);
    if(actionSummaries)
        printf("Customer %d added to line %d\n", customer->id, shortestTellerLine);
}

Customer* popCustomer(TellerQueue* tellerQueue, int tellerNo)
{
    if(tellerQueue->noOfQueues==1)
    {
        Queue* queuePop =  tellerQueue->line[0];
        if(queuePop->head == NULL)
            return NULL;
        Customer* customer = (Customer*) queuePop->pop(queuePop)->value;
        return customer;
    }
    Queue* queuePop =  tellerQueue->line[tellerNo];
    if(queuePop->head == NULL)
    {
        int noOfQueuesToCheck = tellerQueue->noOfQueues;
        int queueIDs[noOfQueuesToCheck];
        for(int i=0;i<noOfQueuesToCheck;i++)
                queueIDs[i] = i;
        while(noOfQueuesToCheck > 0) {
            int index = rand()%noOfQueuesToCheck;
            queuePop = tellerQueue->line[queueIDs[index]];
            if(queuePop->head!=NULL) break;
            for(int i=index;i<noOfQueuesToCheck-1;i++)
                queueIDs[i] = queueIDs[i+1];
            --noOfQueuesToCheck;
        }
    }
    if(queuePop->head == NULL) return NULL;
    else {
        Customer* customer = (Customer*) queuePop->pop(queuePop)->value;
        return customer;
    }
}

TellerQueue* createTellerQueue(int noOfQueues)
{
    Queue** line = (Queue**) malloc(noOfQueues * sizeof(Queue*));
    for(int i=0;i<noOfQueues;i++)
        line[i] = createQueue();
    TellerQueue* tellerQueue = (TellerQueue*) malloc(sizeof(TellerQueue));
    tellerQueue->noOfQueues = noOfQueues;
    tellerQueue->add = &addCustomer;
    tellerQueue->pop = &popCustomer;
    tellerQueue->line = line;
    return tellerQueue;
}

struct Event{
    float eventTime;
    void* person;
    Queue* eventQueue;
    TellerQueue* tellerQueue;
    float* customerTimes;
    void (*addToQueue)(struct Event*);
    struct Event* (*popFromQueue)(Queue*);
    void (*action)(struct Event*);
};

typedef struct Event Event;

Event* createEvent(float eventTime, void* person, Queue* eventQueue, TellerQueue* tellerQueue, float* customerTimes, int actionType);

// event can be :
// 1. customer arriving: add customer to tellerqueue
// 2. teller becoming free: look for next customer in tellerqueue, or idle. Add new teller event to queue
// 3. customer leaving: collect stats about customer
void printQueue(Queue* queue)
{
    Node* node = queue->head;
    while(node!=NULL)
    {
        Customer* customer = (Customer*) node->value;
        printf("%d\t", customer->id);
        node = node->next;
    }
    printf("\n");
}

void printTellerQueue(TellerQueue* tellerQueue)
{
    for(int i=0;i<tellerQueue->noOfQueues;i++)
    {
        printf("\nTellerLine %d:\t", i);
        printQueue(tellerQueue->line[i]);
    }
}

void printTellers(Teller** tellers)
{
    for(int i=0;i<NO_OF_TELLERS;i++)
        if(tellers[i]->currentCustomerID!=-1)
            printf(":%d: %d\t\t", i, tellers[i]->currentCustomerID);
        else
            printf(":%d: ___\t\t", i);
    printf("\n");
}

void actionCustomerArrival(Event* event)
{
    // consider event popped from event queue
    masterClock = event->eventTime;
    Customer* customer = (Customer*) event->person;
    if(actionSummaries)
        printf("Customer %d has arrived\n", customer->id);
    TellerQueue* tellerQueue = event->tellerQueue;
    tellerQueue->add(tellerQueue, customer);
    free(event);
}

void actionTeller(Event* event)
{
    masterClock = event->eventTime;
    Teller* teller = (Teller*) event->person;
    TellerQueue* tellerQueue = event->tellerQueue;
    Customer* customer = tellerQueue->pop(tellerQueue, teller->id);
    if(customer!=NULL)
    {
        customer->waitingTime = masterClock - customer->arrivalTime;
        float serviceTime = getServiceTime();
        customer->serviceTime = serviceTime;
        Event* event1 = createEvent(masterClock+serviceTime, customer, event->eventQueue, event->tellerQueue, event->customerTimes, 3);
        Event* event2 = createEvent(masterClock+serviceTime, teller, event->eventQueue, event->tellerQueue, event->customerTimes, 2);
        event1->addToQueue(event1);
        event2->addToQueue(event2);
        totalTellerServiceTime += serviceTime;
        if(actionSummaries)
            printf("Teller %d will start servicing customer %d. Service time: %f\n", teller->id, customer->id, serviceTime);
        teller->currentCustomerID = customer->id;
    }
    else
    {
        float idleTime = getIdleTime();
        Event* event1 = createEvent(masterClock+idleTime, teller, event->eventQueue, event->tellerQueue, event->customerTimes, 2);
        event1->addToQueue(event1);

        totalTellerIdleTime += idleTime;
        if(actionSummaries)
            printf("Teller %d going idle for %f time\n", teller->id, idleTime);
        teller->currentCustomerID = -1;
    }
    free(event);
}

void actionCustomerExit(Event* event)
{
    masterClock = event->eventTime;
    Customer* customer = (Customer*) event->person;
    if(customer->waitingTime > maxCustomerWaitTime)
        maxCustomerWaitTime = customer->waitingTime;
    event->customerTimes[customer->id] = customer->waitingTime + customer->serviceTime;
    ++noOfCustomersServed;

    if(actionSummaries)
        printf("Customer %d has finished service and is exiting\n", customer->id);
    free(event);
    free(customer);
}
void addEventToQueue(Event* event)
{
    Queue* queue = event->eventQueue;
    Node* newnode = createNode(event);
    queue->addAppropriate(queue, newnode);
}

Event* popEventFromQueue(Queue* queue)
{
    Node* node = queue->pop(queue);
    if(node==NULL) return NULL;
    Event* event = (Event*) node->value;
    free(node);
    return event;
}

Event* createEvent(float eventTime, void* person, Queue* eventQueue, TellerQueue* tellerQueue, float* customerTimes, int actionType)
{
    Event* event = (Event*) malloc(sizeof(Event));
    event->eventTime = eventTime;
    event->person = person;
    event->eventQueue = eventQueue;
    event->tellerQueue = tellerQueue;
    event->customerTimes = customerTimes;
    event->addToQueue = &addEventToQueue;
    event->popFromQueue = &popEventFromQueue;
    if(actionType==1) event->action = &actionCustomerArrival;
    else if(actionType==2)event->action =  &actionTeller;
    else  event->action = &actionCustomerExit;
    return event;
}

int compareEvents(Event* event1, Event* event2)
{
    float diff = event1->eventTime - event2->eventTime;
    if(diff>0) return 1;
    if(diff<0) return -1;
    return 0;
}



void addEventNode(Queue* queue, Node* node)
{
    if(queue->tail == NULL)
    {
        queue->tail = queue->head = node;
        queue->noOfNodes++;
        return;
    }
    Node* current = queue->tail;
    Event* nodeEvent = (Event*) node->value;
    Event* curEvent = (Event*) current->value;
    while(nodeEvent->eventTime < curEvent->eventTime)
    {
        current = current->prev;
        if(current == NULL) break;
        curEvent = (Event*) current->value;
    }
    if(current!=NULL)
    {
        node->next = current->next;
        current->next = node;
        node->prev = current;
        if(node->next!=NULL) node->next->prev = node;
        if(current == queue->tail) queue->tail = node; // godaaaamn bug took hours to resolve :(
    }
    else
    {
        if(queue->head!=NULL) queue->head->prev = node;
        node->next = queue->head;
        queue->head = node;
    }
    queue->noOfNodes++;
}

Queue* createEventQueue()
{
    Queue* queue = createQueue();
    queue->addAppropriate = &addEventNode;
    return queue;
}

void run(int mode, float arrivalTimes[], int plot)
{
    char* typeOfQueueing = (mode==1) ? "one line per teller" :"common line";
    if(!plot)
        printf("Queue Type : %s\n", typeOfQueueing);
    masterClock = 0;
    totalTellerServiceTime = 0;
    totalTellerIdleTime = 0;
    maxCustomerWaitTime = -1;
    noOfCustomersServed = 0;
    shortestTellerLine = 0;
    Queue* eventQueue = createEventQueue();
    eventQueue->head = eventQueue->tail = NULL;
    int noOfTellerLines = (mode==1)? NO_OF_TELLERS : 1;
    TellerQueue* tellerQueue = createTellerQueue(noOfTellerLines);
    for(int i=0;i<noOfTellerLines;i++)
    {
        Queue* queue = tellerQueue->line[i];
        queue->noOfNodes = 0;
        queue->head = NULL;
        queue->tail = NULL;
    }
    float* customerTimes = (float*) malloc(NO_OF_CUSTOMERS* sizeof(float));
    for(int i=0;i<NO_OF_CUSTOMERS;i++)
        customerTimes[i] = -1;
    for(int i=0;i<NO_OF_CUSTOMERS;i++)
    {
        Customer* customer = createCustomer(i, arrivalTimes[i]);
        Event* event = createEvent(customer->arrivalTime, customer, eventQueue, tellerQueue, customerTimes, 1);
        event->addToQueue(event);
        if(actionSummaries)
            printf("Customer %d added to event queue, set to arrive at %f time\n", customer->id, event->eventTime);
    }
    Teller** tellers = (Teller**) malloc(NO_OF_TELLERS*sizeof(Teller*));
    for(int i=0;i<NO_OF_TELLERS;i++)
    {
        tellers[i] = createTeller(i);
        Event* event = createEvent((float)(1 + rand()%600), tellers[i], eventQueue, tellerQueue, customerTimes, 2);
        event->addToQueue(event);
        if(actionSummaries)
            printf("Teller %d added to event queue, idle till %f time\n", tellers[i]->id, event->eventTime);
    }

    Node* eventNode = eventQueue->pop(eventQueue);
    while(eventNode!=NULL && noOfCustomersServed < NO_OF_CUSTOMERS)
    {
        if(interactive) printf("\e[1;1H\e[2J");
        Event* event = (Event*) eventNode->value;
        free(eventNode);
        if(actionSummaries)
            printf("\n-- Current time: %f seconds\n", event->eventTime);
        event->action(event);
        eventNode = eventQueue->pop(eventQueue);
        if(interactive) {
            printTellers(tellers);
            printTellerQueue(tellerQueue);
            getchar();
        }
    }

    float average = 0;
    for(int i=0;i<NO_OF_CUSTOMERS;i++)
        if(customerTimes[i]!=-1)
            average+=customerTimes[i];
    average /= (float)noOfCustomersServed;

    if(plot) fprintf(pldata, "%d %f\n", NO_OF_TELLERS, average);
    else {
        float standardDeviation = 0;
        for (int i = 0; i < NO_OF_CUSTOMERS; i++)
            if (customerTimes[i] != -1)
                standardDeviation += (customerTimes[i] - average) * (customerTimes[i] - average);
        standardDeviation /= (float) noOfCustomersServed;
        standardDeviation = (float) sqrt((double) standardDeviation);

        printf("Number of customers = %d ; Total time = %f seconds\n", noOfCustomersServed, masterClock);
        printf("Number of tellers = %d ; Type of queueing = %s\n", NO_OF_TELLERS, typeOfQueueing);
        printf("Average time spent by customers in bank = %f seconds; Standard deviation = %f seconds\n", average, standardDeviation);
        printf("Maximum wait time = %f seconds\n", maxCustomerWaitTime);
        printf("Total teller service time = %f seconds; Total teller idle time = %f seconds\n\n", totalTellerServiceTime,
               totalTellerIdleTime);
    }

    free(customerTimes);
    for(int i=0;i<eventQueue->noOfNodes;i++)
    {
        Node* node = eventQueue->pop(eventQueue);
        Event* event = (Event*) node->value;
        free(node);
        free(event);
    }
    free(eventQueue);
    for(int i=0;i<noOfTellerLines;i++)
        free(tellerQueue->line[i]);
    free(tellerQueue->line);
    free(tellerQueue);
    for(int i=0;i<NO_OF_TELLERS;i++)
        free(tellers[i]);
    free(tellers);
}

int main(int argc, char* argv[]) {
    if(argc!=5)
    {
        perror("Insufficient arguments!");
        return EXIT_FAILURE;
    }
    NO_OF_CUSTOMERS = atoi(argv[1]);
    NO_OF_TELLERS = atoi(argv[2]);
    SIMULATION_TIME = atof(argv[3])*60;
    AVERAGE_SERVICE_TIME = atof(argv[4])*60;

    float arrivalTimes[NO_OF_CUSTOMERS];
    for(int i=0;i<NO_OF_CUSTOMERS;i++)
        arrivalTimes[i] = SIMULATION_TIME * rand()/(float)(RAND_MAX);

    run(1, arrivalTimes, 0);
    run(2, arrivalTimes, 0);

    if(interactive==1 || actionSummaries==1)
        return 0;
    pldata = fopen("../output/plotdata.txt", "w");
    int k=0;
    NO_OF_TELLERS = 2;
    while(k<50)
    {
        run(2, arrivalTimes, 1);
        k+=1;
        NO_OF_TELLERS+=1;
    }
    fclose(pldata);
    FILE* gnup;
    gnup = popen("gnuplot -persistent", "w");
    fprintf(gnup, "set terminal png\n"
                  "set output \"../output/TimeSpent.png\"\n"
                  "set title \"Average time spent in bank\"\n"
                  "set xlabel \"No of tellers\"\n"
                  "set ylabel \"Average time spent in bank\"\n"
                  "set style line 1 \\\n"
                  "linecolor rgb '#0060ad' \\\n"
                  "linetype 1 linewidth 1 \\\n"
                  "pointtype 7 pointsize 1\n"
                  "plot '../output/plotdata.txt' with linespoints linestyle 1");
    fclose(gnup);
    return 0;
}
