#include <bits/stdc++.h>
#include <unistd.h>

using namespace std;

#define PROCESS_NAME_LEN 32    //进程名最大长度
#define MIN_SLICE 10           //内碎片最大大小
#define DEFAULT_MEM_SIZE 1024  //总内存大小
#define DEFAULT_MEM_START 0    //内存开始分配时的起始地址

//三种算法代号
#define FIRST_FIT 0
#define BEST_FIT 1
#define WORST_FIT 2

typedef pair<int, string> My_algo;

int mem_size = DEFAULT_MEM_SIZE;
bool flag = false;  //当内存以及被分配了之后，不允许更改总内存大小的flag
static int pid = 0;
My_algo algo[123];
int alg = FIRST_FIT;  //算法代号

struct free_block {  //空闲数据块
  int size;
  int start_addr;
  struct free_block *next;
};

struct allocated_block {  //已分配的数据块
  int pid;
  int size;
  int start_addr;
  int *data;
  char process_name[PROCESS_NAME_LEN];
  struct allocated_block *next;
};

free_block *free_block_head;                   //空闲数据块首指针
allocated_block *allocated_block_head = NULL;  //分配块首指针

allocated_block *find_process(int id);      //寻找pid为id的分配块
free_block *init_free_block(int mem_size);  //空闲块初始化
void display_menu();                        //显示选项菜单
void set_mem_size();                        //设置内存大小
int allocate_mem(allocated_block *ab);      //为制定块分配内存
void rearrange();                           // 对块进行重新分配
void create_new_process();                  //创建新的进程
void free_mem(allocated_block *ab);         //释放分配块
void swap(int *p, int *q);                  //交换地址
void dispose(allocated_block *ab);          //释放分配块结构体
void display_mem_usage();                   //显示内存情况
void kill_process();  //杀死对应进程并释放其空间与结构体
// void Usemy_algo(int id);  //使用对应的分配算法
bool (*alg_func[123])(free_block *pre, free_block *tmp);
bool first_fit(free_block *pre, free_block *tmp);
bool best_fit(free_block *pre, free_block *tmp);
bool worst_fit(free_block *pre, free_block *tmp);
int handle_bf_fit(allocated_block *ab);
int handle_worst_fit(allocated_block *ab);

//主函数
int main() {
  int op;
  pid = 0;

  //初始化一个可以使用的内存块，类似与操作系统可用的总存储空间
  free_block_head = init_free_block(mem_size);
  alg_func[FIRST_FIT] = first_fit;
  alg_func[BEST_FIT] = best_fit;
  alg_func[WORST_FIT] = worst_fit;
  for (;;) {
    sleep(1);
    display_menu();
    fflush(stdin);
    scanf("%d", &op);
    switch (op) {
      case 1: {
        set_mem_size();
        break;
      }
      case 2: {
        printf("Choose an algorithm\n");
        printf(" 0: First Fit\n 1: Best Fit\n 2: Worst Fit\n");
        scanf("%d", &alg);
        break;
      }
      case 3: {
        create_new_process();
        break;
      }
      case 4: {
        kill_process();
        break;
      }
      case 5: {
        display_mem_usage();
        break;
      }
      case 233: {
        puts("bye....");
        sleep(1);
        return 0;
      }
      defaut:
        printf("Invalid input, accept candidates in [1, 2, 3, 4, 5, 233]\n");
        break;
    }
  }
}

allocated_block *find_process(int id) {
  //循环遍历分配块链表，寻找pid=id的进程所对应的块
  if (!allocated_block_head) return NULL;
  allocated_block *itr = allocated_block_head;
  while (itr->next) {
    if (itr->pid == id) return itr;
    itr = itr->next;
  }
  return itr;
}

free_block *init_free_block(int mem_size) {
  //初始化空闲块，这里的mem_size表示允许的最大虚拟内存大小
  free_block *p;
  p = (free_block *)malloc(sizeof(free_block));
  if (p == NULL) {
    puts("No memory left");
    return NULL;
  }
  p->size = mem_size;
  p->start_addr = DEFAULT_MEM_START;
  p->next = NULL;
  return p;
}

void display_menu() {
  puts("\n\n******************menu*******************");
  printf("1) Set memory size (default = %d)\n", DEFAULT_MEM_SIZE);
  printf("2) Set memory allocation algorithm\n");
  printf("3) Create a new process\n");
  printf("4) Kill a process\n");
  printf("5) Display memory usage\n");
  printf("233) Exit\n");
}

void set_mem_size() {  //更改最大内存大小
  if (flag) {
    printf("Fail to set memory size: memory size already fixed\n");
    return;
  }
  printf("Enter memory size: (byte) ");
  if (scanf("%d", &mem_size) <= 0) {
    printf("Fail to set memory size: invalid size input\n");
    mem_size = DEFAULT_MEM_SIZE;
    return;
  }
  flag = true;
  free_block_head->size = mem_size;
  printf("Successfully changed memory size to: %d\n", mem_size);
}

int allocate_mem(allocated_block *ab) {
  //为块分配内存，真正的操作系统会在这里进行置换等操作
  if (!free_block_head) {
    printf("Fail to allocate memory: running out of free memory\n");
    return 0;
  }
  rearrange();
  switch (alg) {
    case FIRST_FIT:
      if (!handle_bf_fit(ab)) return 0;
      break;
    case BEST_FIT:
      if (!handle_bf_fit(ab)) return 0;
      break;
    case WORST_FIT:
      if (!handle_worst_fit(ab)) return 0;
      break;
    default:
      printf("Fail to allocate memory: no matching algorithms\n");
      break;
  }
  return 1;
}

void create_new_process() {  //创建新进程
  int mem_sz = 0;
  printf("Please input memory size\n");
  scanf("%d", &mem_sz);
  if (mem_sz <= 0) {
    printf("Fail to create process: memory size must be a positive integer\n");
    return;
  }
  allocated_block *ab = (allocated_block *)malloc(sizeof(allocated_block));
  if (!ab) exit(EXIT_FAILURE);
  ab->size = mem_sz;
  ab->pid = ++pid;
  ab->next = NULL;
  sprintf(ab->process_name, "process%d", pid);
  if (!allocate_mem(ab)) {
    printf("Fail to create new process: fail to allocate memory\n");
    return;
  }
  printf("Successfully created new process **%s**\n", ab->process_name);
  allocated_block *tmp = allocated_block_head;
  if (!tmp) {
    allocated_block_head = ab;
  } else {
    while (tmp->next) tmp = tmp->next;
    tmp->next = ab;
  }
}

void swap(int *p, int *q) {
  int tmp = *p;
  *p = *q;
  *q = tmp;
}

void rearrange() {  //将块按照地址大小进行排序
  free_block *tmp, *tmpx;
  puts("Rearrange begins...");
  puts("Rearrange by address...");
  tmp = free_block_head;
  while (tmp) {  //冒泡排序
    tmpx = tmp->next;
    while (tmpx) {
      if (tmpx->start_addr < tmp->start_addr) {
        swap(&tmp->start_addr, &tmpx->start_addr);
        swap(&tmp->size, &tmpx->size);
      }
      tmpx = tmpx->next;
    }
    tmp = tmp->next;
  }
  tmp = free_block_head;
  while (tmp != NULL) {  //合并相邻空闲数据块
    tmpx = tmp->next;
    while (tmpx != NULL) {
      int offset = tmp->start_addr + tmp->size;
      if (offset == tmpx->start_addr) {
        tmp->size += tmpx->size;
        tmp->next = tmpx->next;
        free(tmpx);
      }
      tmpx = tmpx->next;
    }
    tmp = tmp->next;
  }
  tmp = free_block_head;
  while (tmp != NULL) {  //冒泡排序
    tmpx = tmp->next;
    while (tmpx != NULL) {
      if (!alg_func[alg](tmp, tmpx)) {
        swap(&tmp->start_addr, &tmpx->start_addr);
        swap(&tmp->size, &tmpx->size);
      }
      tmpx = tmpx->next;
    }
    tmp = tmp->next;
  }
  usleep(500);
  puts("Rearrange Done.");
}

void free_mem(allocated_block *ab) {  //释放某一块的内存
  free_block *tmp = (free_block *)malloc(sizeof(free_block));
  if (!tmp) {
    printf("Fail to free memory: no free block\n");
    return;
  }
  tmp->size = ab->size;
  tmp->start_addr = ab->start_addr;
  tmp->next = NULL;
  if (!free_block_head) {
    free_block_head = tmp;
  } else {
    tmp->next = free_block_head->next;
    free_block_head->next = tmp;
  }
}

void dispose(allocated_block *fab) {  //释放结构体所占的内存
  allocated_block *pre, *ab;
  if (fab == allocated_block_head) {
    allocated_block_head = allocated_block_head->next;
    free(fab);
    return;
  }
  pre = allocated_block_head;
  ab = allocated_block_head->next;
  while (ab != fab) {
    pre = ab;
    ab = ab->next;
  }
  pre->next = ab->next;
  free(ab);
}

void display_mem_usage() {
  free_block *fb = free_block_head;
  allocated_block *ab = allocated_block_head;
  puts("*********************Free Memory*********************");
  printf("%20s %20s\n", "start_addr", "size");
  int cnt = 0;
  while (fb) {
    cnt++;
    printf("%20d %20d\n", fb->start_addr, fb->size);
    fb = fb->next;
  }
  if (!cnt)
    puts("No Free Memory");
  else
    printf("Totaly %d free blocks\n", cnt);
  puts("");
  puts("*******************Used Memory*********************");
  printf("%10s %20s %20s %10s\n", "PID", "process_name", "start_addr", "size");
  cnt = 0;
  while (ab) {
    cnt++;
    printf("%10d %20s %20d %10d\n", ab->pid, ab->process_name, ab->start_addr,
           ab->size);
    ab = ab->next;
  }
  if (!cnt)
    puts("No allocated block");
  else
    printf("Totaly %d allocated blocks\n", cnt);
  return;
}

void kill_process() {  //杀死某个进程
  allocated_block *ab;
  int pid;
  puts("Please input the pid of Killed process");
  scanf("%d", &pid);
  ab = find_process(pid);
  if (ab) {
    free_mem(ab);
    dispose(ab);
  } else {
    printf("Fail to kill process: process of pid %d not found\n", pid);
  }
}

bool first_fit(free_block *pre, free_block *tmp) {
  return pre->start_addr < tmp->start_addr;
}

bool best_fit(free_block *pre, free_block *tmp) {
  return pre->size < tmp->size;
}

bool worst_fit(free_block *pre, free_block *tmp) {
  return pre->size > tmp->size;
}

int handle_bf_fit(allocated_block *ab) {
  free_block *pre = NULL;
  free_block *tmp = free_block_head;
  while (tmp && tmp->size < ab->size) {
    pre = tmp;
    tmp = tmp->next;
  }
  if (!tmp) return 0;
  if (tmp->size - ab->size > MIN_SLICE) {
    ab->start_addr = tmp->start_addr;
    tmp->size -= ab->size;
    tmp->start_addr += ab->size;
  } else {
    ab->size = tmp->size;
    ab->start_addr = tmp->start_addr;
    if (tmp == free_block_head) {
      free_block_head = free_block_head->next;
    } else {
      pre->next = tmp->next;
    }
  }
  return 1;
}

int handle_worst_fit(allocated_block *ab) {
  free_block *tmp = free_block_head;
  if (tmp->size < ab->size) return 0;
  if (tmp->size - ab->size > MIN_SLICE) {
    ab->start_addr = tmp->start_addr;
    tmp->size -= ab->size;
    tmp->start_addr += ab->size;
  } else {
    ab->size = tmp->size;
    ab->start_addr = tmp->start_addr;
    free_block_head = free_block_head->next;
  }
  return 1;
}
