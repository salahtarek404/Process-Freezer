// user/testfreeze.c
// Stable xv6 Freeze/Resume Demo & Validation
// Designed specifically for current project implementation

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static int passed = 0;
static int failed = 0;

// --------------------------------------------------
// Helper
// --------------------------------------------------

void
check(char *msg, int condition)
{
  if(condition){
    printf("[PASS] %s\n", msg);
    passed++;
  } else {
    printf("[FAIL] %s\n", msg);
    failed++;
  }
}

// --------------------------------------------------
// Busy Delay
// --------------------------------------------------

void
delay(void)
{
  volatile int i;

  for(i = 0; i < 100000000; i++);
}

// --------------------------------------------------
// Test 1
// Invalid PID
// --------------------------------------------------

void
test_invalid_pid(void)
{
  printf("\n=== Test 1: Invalid PID ===\n");

  check("freeze(-1)", freeze(-1) == -1);

  check("freeze(9999)", freeze(9999) == -1);

  check("resume(-1)", resume(-1) == -1);

  check("resume(9999)", resume(9999) == -1);
}

// --------------------------------------------------
// Test 2
// Basic Freeze/Resume
// --------------------------------------------------

void
test_basic_freeze_resume(void)
{
  printf("\n=== Test 2: Basic Freeze/Resume ===\n");

  int pid = fork();

  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  // Child
  if(pid == 0){
    while(1){
      printf("Child running...\n");
      delay();
    }
  }

  // Parent
  delay();

  check("freeze(child)", freeze(pid) == 0);

  printf("Child should stop printing now...\n");

  delay();

  check("resume(child)", resume(pid) == 0);

  printf("Child should resume printing now...\n");

  delay();

  kill(pid);

  wait(0);
}

// --------------------------------------------------
// Test 3
// Double Freeze
// --------------------------------------------------

void
test_double_freeze(void)
{
  printf("\n=== Test 3: Double Freeze ===\n");

  int pid = fork();

  if(pid == 0){
    while(1){
      delay();
    }
  }

  delay();

  check("first freeze()", freeze(pid) == 0);

  check("second freeze()", freeze(pid) == -1);

  resume(pid);

  kill(pid);

  wait(0);
}

// --------------------------------------------------
// Test 4
// Double Resume
// --------------------------------------------------

void
test_double_resume(void)
{
  printf("\n=== Test 4: Double Resume ===\n");

  int pid = fork();

  if(pid == 0){
    while(1){
      delay();
    }
  }

  delay();

  freeze(pid);

  check("first resume()", resume(pid) == 0);

  check("second resume()", resume(pid) == -1);

  kill(pid);

  wait(0);
}

// --------------------------------------------------
// Test 5
// Multiple Freeze/Resume Cycles
// --------------------------------------------------

void
test_multiple_cycles(void)
{
  printf("\n=== Test 5: Multiple Freeze/Resume Cycles ===\n");

  int pid = fork();

  if(pid == 0){
    while(1){
      delay();
    }
  }

  delay();

  int ok = 1;

  for(int i = 0; i < 3; i++){
    if(freeze(pid) != 0){
      ok = 0;
      break;
    }

    delay();

    if(resume(pid) != 0){
      ok = 0;
      break;
    }

    delay();
  }

  check("3 freeze/resume cycles", ok);

  kill(pid);

  wait(0);
}

// --------------------------------------------------
// Test 6
// Many Processes
// --------------------------------------------------

void
test_multiple_processes(void)
{
  printf("\n=== Test 6: Multiple Processes ===\n");

#define NPROCS 4

  int pids[NPROCS];
  int ok1 = 1;
  int ok2 = 1;

  for(int i = 0; i < NPROCS; i++){
    pids[i] = fork();
    if(pids[i] == 0){
      while(1){
        delay();
      }
    }
  }

  delay();

  for(int i = 0; i < NPROCS; i++){
    if(freeze(pids[i]) != 0)
      ok1 = 0;
  }

  check("freeze multiple processes", ok1);

  delay();

  for(int i = 0; i < NPROCS; i++){
    if(resume(pids[i]) != 0)
      ok2 = 0;
  }

  check("resume multiple processes", ok2);
  
  for(int i = 0; i < NPROCS; i++){
    kill(pids[i]);
    wait(0);
  }
}

// --------------------------------------------------
// Summary
// --------------------------------------------------

void
summary(void)
{
  printf("\n====================================\n");
  printf("Tests Passed: %d\n", passed);
  printf("Tests Failed: %d\n", failed);
  printf("====================================\n");
}

// --------------------------------------------------
// Main
// --------------------------------------------------

int
main(void)
{
  printf("\n====================================\n");
  printf("      xv6 Freeze/Resume Tests\n");
  printf("====================================\n");

  test_invalid_pid();

  test_basic_freeze_resume();

  test_double_freeze();

  test_double_resume();

  test_multiple_cycles();

  test_multiple_processes();

  summary();

  exit(0);
}
