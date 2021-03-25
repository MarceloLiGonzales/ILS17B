//Includes


//MACROS
#define TASK_1_STACK_SIZE 1024
#define TASK_2_STACK_SIZE 1024
#define TASK_3_STACK_SIZE 1024
#define TASK_4_STACK_SIZE 1024
#define TASK_5_STACK_SIZE 1024

#define TASK_1_PRIORITY 1
#define TASK_2_PRIORITY 1
#define TASK_3_PRIORITY 1
#define TASK_4_PRIORITY 1
#define TASK_5_PRIORITY 1

#define TASK_1_CORE 0
#define TASK_2_CORE 0
#define TASK_3_CORE 0
#define TASK_4_CORE 0
#define TASK_5_CORE 0


//Initialize
void generalSetup(void);

//Global variables


//TaskHandles
static TaskHandle_t task1Handle = NULL;
static TaskHandle_t task2Handle = NULL;
static TaskHandle_t task3Handle = NULL;
static TaskHandle_t task4Handle = NULL;
static TaskHandle_t task5Handle = NULL;

//Pins


void setup() {

  //Functions called once upon start up
  generalSetup();

  //Start tasks
  //Start Task 1
  xTaskCreatePinnedToCore(
    Task1,
    "Task 1 name",
    TASK_1_STACK_SIZE,
    NULL,
    TASK_1_PRIORITY,
    &task1Handle,
    TASK_1_CORE);

  //Start task 2
  xTaskCreatePinnedToCore(
    Task2,
    "Task 2 name",
    TASK_2_STACK_SIZE,
    NULL,
    TASK_2_PRIORITY,
    &task2Handle,
    TASK_2_CORE);

   //Start update LCD task
  xTaskCreatePinnedToCore(
    Task3,
    "Task 3 name",
    TASK_3_STACK_SIZE,
    NULL,
    TASK_3_PRIORITY,
    &task3Handle,
    TASK_3_CORE);

   //Start Task 4
   xTaskCreatePinnedToCore(
    Task4,
    "Task 4 name",
    TASK_4_STACK_SIZE,
    NULL,
    TASK_4_PRIORITY,
    &task4Handle,
    TASK_4_CORE);

   //Start Task 5
   xTaskCreatePinnedToCore(
    Task5,
    "Task 5 name",
    TASK_5_STACK_SIZE,
    NULL,
    TASK_5_PRIORITY,
    &task5Handle,
    TASK_5_CORE);

    vTaskDelete(NULL); //I dont believe we will need setup or loop, but I may be wrong
}

void loop() {

}
