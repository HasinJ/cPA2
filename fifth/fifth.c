#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>

struct Vertex{
  char *letter;
  int distance, degree, visited;
  struct Edge* next;
  struct Edge* prev;
};

struct Edge{
  int weight;
  char* letter;
  struct Edge* next;
  struct Edge* prev;
  struct Vertex* vertex; //vertex it corresponds with, to make traversal easier
};

struct TopSet{
  struct Vertex **items;
  int count;
};

struct Stack{
  int top;
  int max;
  struct Vertex **items;
};

struct Stack* IndegreeStack;
struct TopSet* set;

struct Vertex* Pop(struct Stack* stack){
  if(stack->top==0) exit(EXIT_FAILURE);
  return stack->items[stack->top--];
}

void Push(struct Stack* stack,struct Vertex* vertex){
  if (stack->top+1>stack->max){
    printf("stack overflow (visited node probably in stack or repitition)\n");
    exit(EXIT_FAILURE);
  }
  stack->items[++stack->top]=vertex;
}

struct Vertex* Peek(struct Stack* stack){
  return stack->items[stack->top];
}

void RemoveTop(struct Stack* stack){
  if(stack->top==0) exit(EXIT_FAILURE);
  stack->top--;
  return;
}

struct Stack* createStack(struct Stack* stack, int vertices){
  stack = malloc(sizeof(struct Stack));
  stack->top=0;
  stack->max=vertices;
  stack->items=malloc(vertices*sizeof(struct Vertex));
  return stack;
}

struct TopSet* createTopSet(struct TopSet* topset, int vertices){
  topset = malloc(sizeof(struct TopSet));
  topset->items=calloc(vertices,sizeof(struct Vertex));
  topset->count=0;
  return topset;
}

void insertHere(struct Edge* current, struct Vertex* correspondingVertex, char* character, int weight){
  struct Edge* new = malloc(sizeof(struct Edge));
  new->letter=malloc(20*sizeof(char*));
  strcpy(new->letter,character);
  new->weight=weight;
  new->vertex=correspondingVertex;
  new->next = current->next;
  current->next=new;
}

void insertHead(struct Vertex* current, struct Vertex* correspondingVertex, char* character, int weight){
  struct Edge* new = malloc(sizeof(struct Edge));
  new->letter=malloc(20*sizeof(char*));
  strcpy(new->letter,character);
  new->weight=weight;
  new->vertex=correspondingVertex;
  new->next = current->next;
  current->next=new;
}

void addNext(struct Vertex* vertex, struct Vertex* correspondingVertex, char* to, int weight) {
  if(vertex->next==0){ //head
    vertex->next=malloc(sizeof(struct Edge));
    vertex->next->letter=malloc(20*sizeof(char*));
    strcpy(vertex->next->letter,to);
    vertex->next->vertex=correspondingVertex;
    vertex->next->weight=weight;
    vertex->next->next=0;
    return;
  } else{ if(strcmp(vertex->next->letter,to)>0){
    if (strcmp(vertex->next->letter,to)==0) return;
    insertHead(vertex,correspondingVertex,to,weight);
    return;
  }}

  struct Edge* current=vertex->next; //set current as head
  if (strcmp(current->letter,to)==0) return;
  while(current->next!=0) {
    if (strcmp(current->next->letter,to)==0) return;
    if (strcmp(current->next->letter,to)>0) { //flipping this swaps the list around
      insertHere(current,correspondingVertex,to,weight);
      return;
    }
    current = current->next;
  }
  current->next=malloc(sizeof(struct Edge));
  current->next->letter=malloc(20*sizeof(char*));
  strcpy(current->next->letter,to);
  current->next->vertex=correspondingVertex;
  current->next->weight=weight;
  current->next->next=0;
}

void addPrev(struct Vertex* vertex,struct Vertex* correspondingVertex,char* to,int weight) {
  if(vertex->prev==0){ //head
    vertex->prev=malloc(sizeof(struct Edge));
    vertex->prev->letter=malloc(20*sizeof(char*));
    strcpy(vertex->prev->letter,to);
    vertex->prev->weight=weight;
    vertex->prev->vertex=correspondingVertex;
    vertex->prev->prev=0;
    return;
  }

  struct Edge* current=vertex->prev; //set current as head
  while(current->prev!=0) {
    current = current->prev;
  }
  current->prev=malloc(sizeof(struct Edge));
  current->prev->letter=malloc(20*sizeof(char*));
  strcpy(current->prev->letter,to);
  current->prev->weight=weight;
  current->prev->vertex=correspondingVertex;
  current->prev->prev=0;
}

void degree(struct Vertex** graph,int vertices){
  for (size_t i = 0; i < vertices; i++) {
    //in degree
    int count=0;
    for (struct Edge* current = graph[i]->prev; current!=0; current=current->prev) {
      count++;
    }
    graph[i]->degree=count;
  }
}

void freeNextEdges(struct Edge* x){
  if (x==0) return;
  freeNextEdges(x->next);
  free(x->letter);
  free(x);
  return;
}

void freePrevEdges(struct Edge* x){
  if (x==0) return;
  freePrevEdges(x->prev);
  free(x->letter);
  free(x);
  return;
}

void freeVertex(struct Vertex* x){
  if (x==0) return;
  freeNextEdges(x->next);
  freePrevEdges(x->prev);
  free(x->letter);
  free(x);
  return;
}

void freeEverything(struct Vertex** graph, int vertices){
  for (size_t i = 0; i < vertices; i++) {
    freeVertex(graph[i]);
  }
  free(set->items);
  free(set);
  while (IndegreeStack->top!=0) IndegreeStack->top--;
  free(IndegreeStack->items);
  free(IndegreeStack);
  free(graph);
}

void read(struct Vertex **graph, int vertices){
  printf("\n");
  for (size_t i = 0; i < vertices; i++) {
    printf("%s", graph[i]->letter);
    printf("\n");
  }
  return;
}

void readTopSet(struct TopSet* set, int count){
  for (size_t i = 0; i < count; i++) {
    printf("%s-->",set->items[i]->letter);
    if(set->items[i]->distance==INT_MAX) {
      printf("INF\n");
      continue;}
    printf("%d\n", set->items[i]->distance);
  }
  printf("\n");
}

void readAll(struct Vertex **graph, int vertices){
  degree(graph,vertices);

  for (size_t i = 0; i < vertices; i++) {
    printf("%s: ", graph[i]->letter);
    for (struct Edge* current = graph[i]->next; current!=0; current=current->next) {
      printf("%s%d ", current->letter,current->weight);
    }
    printf("\n");
  }

  for (size_t i = 0; i < vertices; i++) {
    for (struct Edge* current = graph[i]->prev; current!=0; current=current->prev) {
      printf(" %s<--", current->letter);
    }
    printf("  %s  ", graph[i]->letter);
    for (struct Edge* current = graph[i]->next; current!=0; current=current->next) {
      printf("-->%s ", current->letter);
    }
    printf("degree: %d\n", graph[i]->degree);
  }
  printf("Top Order: ");
  for (size_t i = 0; i < set->count; i++) {
    printf("%s ", set->items[i]->letter);
  }
  printf("\n");
}

void addVertex(struct Vertex **graph, int vertices, char* content, int position){
  struct Vertex* vertex=calloc(1, sizeof(struct Vertex));
  vertex->letter=malloc(20*sizeof(char*));
  strcpy(vertex->letter,content);
  vertex->next=0;
  vertex->degree=0;
  vertex->distance=INT_MAX;
  if (graph[0]==0){
    graph[position]=vertex;
    return;
  }else if(strcmp(graph[position-1]->letter,content)>0){
    for (size_t i = position; i!=-1; i--) {
      if(i==0) {
        graph[0]=vertex;
        return;
      }
      if(strcmp(graph[i-1]->letter,content)<0){
        graph[i]=vertex;
        return;
      }
      graph[i]=graph[i-1];
    }
  }

  graph[position]=vertex;
}

void fill(struct Vertex **graph, int vertices, char* source, char* to, int weight){

  //add edges
  struct Vertex* corr=0; //corresponding vertex
  for (size_t i = 0; i < vertices; i++) {

    if (strcmp(to,graph[i]->letter)==0) corr = graph[i];
    if (strcmp(source,graph[i]->letter)==0) {
      if(corr==0)  {
        for (size_t j = i; j < vertices; j++) if (strcmp(to,graph[j]->letter)==0) corr = graph[j];
      }
      addNext(graph[i],corr,to,weight);

      //undirected (does the same for the "to" vertex)
      for (size_t j = 0; j < vertices; j++){
        if (strcmp(to,graph[j]->letter)==0) {
          addPrev(graph[j],graph[i],source,weight);
          return;}}
    }
  }
}

void dfsDAG(struct TopSet* topset, struct Stack* stack, struct Vertex* current){
  while(stack->top!=0){
    struct Edge* temp = current->next;
    if (temp==0){
      RemoveTop(stack);
      current=Peek(stack);
      //readStack();
      dfsDAG(topset,stack,current);
      return;}

    while(temp->vertex->visited!=0){
      temp=temp->next;
      if (temp==0){
        RemoveTop(stack);
        current=Peek(stack);
        //readStack();
        dfsDAG(topset,stack,current);
        return;}
    }

    Push(stack,temp->vertex);
    current=Peek(stack);

    topset->items[topset->count++]=current;

    current->visited=1;
    while(current->next==0){
      RemoveTop(stack);
      current=Peek(stack);
    }
  }
}

void fillTempSet(struct TopSet* tempSet, struct Vertex* root){
  struct Stack* DepthStack=0;
  DepthStack = createStack(DepthStack,set->count);

  Push(DepthStack,root);
  Peek(DepthStack)->visited=1;
  tempSet->items[tempSet->count++]=Peek(DepthStack);
  dfsDAG(tempSet,DepthStack,root);

  free(DepthStack->items);
  free(DepthStack);
}

void TopologicalSort(struct Vertex** graph, int vertices){
  degree(graph,vertices);
  IndegreeStack = createStack(IndegreeStack,vertices);
  set = malloc(sizeof(struct TopSet));
  set->items=calloc(vertices,sizeof(struct Vertex));
  set->count=0;
  int i=0;
  while(set->items[vertices-1]==0){
    for (size_t j = 0; j < vertices; j++) {
      if(graph[j]->degree==i) Push(IndegreeStack,graph[j]);
    }
    if(IndegreeStack->top==0) continue;
    while(IndegreeStack->top!=0){
      struct Vertex* current = Pop(IndegreeStack);
      set->items[set->count++]=current;
      struct Edge* temp = current->next;
      while (temp!=0) {
        temp->vertex->degree--;
        if(temp->vertex->degree==0) Push(IndegreeStack,temp->vertex);
        temp=temp->next;
      }
    }
    i++;
  }
}

void resetTopSet(){
  for (size_t i = 0; i < set->count; i++) {
    set->items[i]->distance=INT_MAX;
    set->items[i]->visited=0;
  }
}

void resetVisited(struct Vertex** graph, int vertices){
  for (size_t i = 0; i < vertices; i++) {
    graph[i]->visited = 0;
  }

}

void findDistance(struct TopSet* tempSet,int i){
  for (size_t j = 0; j < tempSet->count; j++) {
    if(strcmp(set->items[i]->letter, tempSet->items[j]->letter)==0) break;
    if(j==tempSet->count-1) return;
  }

  struct Edge* adjacent = set->items[i]->next;
  while(adjacent!=0){

    int current, from, distance; //current is the node we're visiting
    current = adjacent->vertex->distance;
    from = set->items[i]->distance;

    distance = adjacent->weight + from;
    if(current==INT_MAX) adjacent->vertex->distance=distance;
    if(distance<current) adjacent->vertex->distance=distance;

    adjacent=adjacent->next;
  }
}

void ShortestDistance(struct Vertex** graph, char* src){
  resetTopSet();

  if (strcmp(set->items[set->count-1]->letter,src)==0){
    set->items[set->count-1]->distance=0;
    return;
  }

  int found=0;
  struct TopSet* tempSet = 0;
  tempSet=createTopSet(tempSet,set->count);
  for (size_t i = 0; i < set->count; i++) {
    if(strcmp(set->items[i]->letter,src)==0 && found==0) {
      found=1;
      set->items[i]->distance=0;
      fillTempSet(tempSet,set->items[i]);
      //readTopSet(tempSet,tempSet->count);
    }
    if (found==1){
      findDistance(tempSet,i);
    }
  }
  free(tempSet->items);
  free(tempSet);
  //readShortestDist(graph);
}

void readAnswer(struct Vertex** graph,int vertices){
  for (size_t i = 0; i < vertices; i++) {
    printf("%s ",graph[i]->letter);
    if(graph[i]->distance==INT_MAX) {
      printf("INF\n");
      continue;}
    printf("%d\n", graph[i]->distance);
  }
}


void dfs(struct Stack* CycleStack,struct Vertex* root,struct Vertex* current){

  while(CycleStack->top!=0){
    struct Edge* temp = current->next;
    if (temp==0){
      Pop(CycleStack);
      current=Peek(CycleStack);
      //readStack();
      dfs(CycleStack,root,current);
      return;}

    while(temp->vertex->visited!=0){
      if(temp->vertex->visited==1 && strcmp(temp->letter,root->letter)==0) {
        //printf("(%s) ", temp->letter);
        printf("\nCYCLE\n");
        exit(EXIT_FAILURE);
      };
      temp=temp->next;
      if (temp==0){
        Pop(CycleStack);
        current=Peek(CycleStack);
        //readStack();
        dfs(CycleStack,root,current);
        return;}
    }

    Push(CycleStack,temp->vertex);
    current=Peek(CycleStack);
    current->visited=1;
    //printf("%s ", current->letter);
    while(current->next==0){
      Pop(CycleStack);
      current=Peek(CycleStack);
    }
  }
}

void checkLoop(struct Vertex** graph, int vertices){

  struct Stack* CycleStack=0;
  CycleStack=createStack(CycleStack,vertices);

  for (size_t i = 0; i < vertices; i++) {
    resetVisited(graph,vertices);
    Push(CycleStack,graph[i]);
    //printf("current: %s ", Peek()->letter);
    Peek(CycleStack)->visited=1;
    dfs(CycleStack,graph[i],graph[i]);
  }

  resetVisited(graph,vertices);
  free(CycleStack->items);
  free(CycleStack);
  printf("\n");
}

int main(int argc, char *argv[argc+1]) {

  FILE *f;
  f = fopen(argv[1],"r");
  if (f==0) {
    printf("error\n");
    return EXIT_SUCCESS;
  }

  int vertices;
  fscanf(f,"%d\n",&vertices);

  //intiliaze graph
  struct Vertex **graph = calloc(vertices,sizeof(struct Vertex));
  for (size_t i = 0; i < vertices; i++) {
    char content[20];
    fscanf(f,"%s\n",content);
    addVertex(graph,vertices,content,i);
  }
  //read(graph,vertices);

  //add nodes
  int weight;
  char source[20],to[20];
  while ((fscanf(f,"%s %s %d",source,to,&weight))!=EOF) fill(graph,vertices,source,to,weight);

  checkLoop(graph,vertices);

  //TopSort
  TopologicalSort(graph,vertices);
  //readAll(graph,vertices);

  f = fopen(argv[2],"r");
  if (f==0) {
    printf("error\n");
    return EXIT_SUCCESS;
  }

  //ShortestDistance(graph,"L");
  //readTopSet(set,set->count);

  //distances

  while(fscanf(f,"%s",source)!=EOF){
    printf("\n");
    ShortestDistance(graph,source);
    //readTopSet(set,set->count);
    readAnswer(graph,vertices);
  }


  freeEverything(graph,vertices);
  return EXIT_SUCCESS;
}
