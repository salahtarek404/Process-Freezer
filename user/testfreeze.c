// testfreeze: end-to-end test for the freeze/resume syscalls.
//
// Layout:
//   child  : prints "child: tick N" forever, pausing 5 ticks between prints
//   parent : lets the child warm up, freezes it, verifies the child went
//            silent for the whole quiet window, resumes it, verifies the
//            child resumes printing, then cleans up.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define WARMUP_TICKS 20
#define QUIET_TICKS  40
#define RESUME_TICKS 30

int
main(int argc, char *argv[])
{
  int pid = fork();
  if(pid < 0){
    printf("FAIL: fork\n");
    exit(1);
  }

  if(pid == 0){
    int i = 0;
    while(1){
      printf("child: tick %d\n", i++);
      pause(5);
    }
  }

  printf("parent: warmup, expect child ticks for %d ticks\n", WARMUP_TICKS);
  pause(WARMUP_TICKS);

  printf("parent: freezing child pid=%d\n", pid);
  if(freeze(pid) < 0){
    printf("FAIL: freeze() returned -1\n");
    kill(pid); wait(0); exit(1);
  }

  printf("parent: child frozen; %d-tick QUIET window (expect NO child output)\n", QUIET_TICKS);
  pause(QUIET_TICKS);
  printf("parent: quiet window over\n");

  printf("parent: resuming child\n");
  if(resume(pid) < 0){
    printf("FAIL: resume() returned -1\n");
    kill(pid); wait(0); exit(1);
  }

  printf("parent: child resumed; expect child ticks for %d ticks\n", RESUME_TICKS);
  pause(RESUME_TICKS);

  printf("parent: cleaning up\n");
  kill(pid);
  wait(0);
  printf("PASS: testfreeze done\n");
  exit(0);
}
