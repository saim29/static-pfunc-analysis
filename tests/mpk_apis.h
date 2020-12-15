#ifndef _MPK_API_H
#define _MPK_API_H
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef MPK_DBG
  #define MPK_DBM(...)				\
    do {					\
      fprintf(stderr, __VA_ARGS__);		\
      fprintf(stderr, "\n");			\
    } while(0)
#else
   #define MPK_DBM(...)
#endif

unsigned long switch_cnt;

#define INCR_SCNT switch_cnt += 1;


#define MPK_ERR(...)				\
    do {					\
      fprintf(stderr, __VA_ARGS__);		\
      fprintf(stderr, "\n");			\
    } while(0)
  
#define RFLAG 0x1
#define WFLAG 0x2
#define RWFLAG 0x3
#define UNIT 0x1000

//signature for mpt functions for compiler testing

int mpt_mmap(void** addr, int a, int b, int c) {

  return 0;
}

int mpt_init(double a) {

  return 0;
}


int mpt_begin(int a, int b) {

  return 0;
}

int mpt_end(int a) {

  return 0;
}

int mpt_destroy(int a){

  return 0;
}


#define rwmmap(addr) \
                mpt_mmap(addr, UNIT, RWFLAG , MAP_ANONYMOUS | MAP_PRIVATE)

#define _init(val) \
                mpt_init(val); 

#define BRIDGE_DOMAINRW(dom) \
                mpt_begin(dom, RWFLAG)

#define BRIDGE_DOMAINR(dom) \
                mpt_begin(dom,RFLAG)

#define EXIT_DOMAIN(dom) \
                mpt_end(dom)

#define DESTROY_DOMAIN(dom) \
                mpt_destroy(dom)

