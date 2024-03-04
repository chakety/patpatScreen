#ifndef MEMO_H
#define MEMO_H

typedef struct {
    char *name[100];
    char *content[1000];
} memo;

int fetch_memo_data(memo* data);

#endif // MEMO_H