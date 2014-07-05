#ifndef CONFIG_H
#define CONFIG_H

#define TU 1
#define TS 2
#define TIT 3
#define MERQURY 4
#define PROTO_MBUS 6
#define PROTO_DCON 7

typedef struct _GLOBALSTRUCT_ {
    unsigned char mbus;
    unsigned char type;
    unsigned char com;
    unsigned char regs;
    unsigned char maska[32];
    unsigned char password[8];
    unsigned short cycle;
    unsigned short begin;
    unsigned short end;
    unsigned char tic;
    unsigned char proto;
} GLOBALSTRUCT;

#endif // CONFIG_H
