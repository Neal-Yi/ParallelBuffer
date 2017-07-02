# Parallel Buffer #

为并行程序设计的，无锁的、无内存拷贝的高速缓冲区

## 生产者/消费者模型 ##

线程Producer产生我们所需的数据，线程Consumer消耗Producer生成的数据。为使Producer与Consumer并行，连接两者的缓冲区进行良好的设计。

![生产者/消费者问题的有限循环缓冲区](./Image/parallelbuffer.jpg "生产者/消费者问题的有限循环缓冲" )

以下是《操作系统 精髓与设计原理》中解决这一问题的代码
~~~c
const int sizeofbuffer = /* 缓冲区大小*/
semaphore s = 1, n = 0, e = sizeofbuffer;
/* s 是缓冲区的锁*/
void producer()
{
  while (true){
    produce();
    semWait(e);
    semWait(s);
    append();
    semSignal(s);
    semSignal(n);
  }
}
void consumer()
{
  while (true){
    semWait(n);
    semWait(s);
    take();
    semSignal(s);
    semSignal(e);
    consume();
  }
}

void main()
{
  parbegin(producer, consumer);
}
~~~


上述模型在使用时应注意两个问题：
1. 对缓冲区的访问仍然是有锁的。
2. 缓冲区中的数据是由append()和take()两个函数得到更新的，应尽可能的避免内存拷贝。

为解决这两个问题，作如下改进：

~~~c
void producer()
{
  while (true){
    semWait(e);
    buf = getonebuf(); /*向缓冲区申请空间*/
    produce(buf); /*将produce生成的数据放置于缓冲区*/
    semSignal(n);
  }
}
void consumer()
{
  while (true){
    semWait(n);
    buf = take(); /*从缓冲区中提取一份数据*/
    consume(buf); /*完成对该数据的处理*/
    semSignal(e);
  }
}
~~~

以以上代码为原型，将缓冲区封装后：

~~~c
ParallelBuffer *pb;
void producer()
{
  void *buf;
  while (true){
    buf = push(pb); /*若缓冲区则满等待,直到consumer取走了一份数据*/
    produce(buf);
    append(pb); /*缓冲区数据量加一*/
  }
}
void consumer()
{
  void *buf;
  while (true){
    buf = pop(pb); /*若缓冲区为空则等待，直到producer生成了一份数据*/
    consume(buf);
    take(pb);/*缓冲区数据量减一*/
  }
}

void main()
{
  init(pb);
  parbegin(producer, consumer);
}
~~~
## 用法 ##

main:
~~~c
  const elementsofbuffer = /*缓冲区中元素个数(>=1)*/
  ParallelBuffer pb;
  pb_init(&pb, sizeof(element), elementsofbuffer);

  ...
  pb_free(&pb); /*结束时释放缓存空间*/
~~~
producer:
~~~c
  void *buf;
  size_t elementsize; /*produce生成数据的长度*/
  while(some condition){
    buf = pb_push(&pb);
    elementsize = produce(buf);/*保存生成长度为elementsize的数据到buf中*/
    pb_append(&pb, elementsize);
  }
  pb_producefin(&pb);
~~~
consumer:
~~~c
  void *buf;
  size_t elementsize;
  /*producer 数据生成完成，且缓冲区数据都已处理时结束 */
  while(!pb_processfin(&pb)){
    buf = pb_pop(&pb, &elementsize);/*从缓冲区中取出一份数据，长度保留在elementsize中*/
    consume(buf, elementsize);/*处理取出的数据*/
    pb_take(&pb);
  }
~~~
