
#define RTIME long

#define BUFFER_SIZE 10


typedef struct {
    RTIME values[BUFFER_SIZE];
    int index;
    int count;
    RTIME sum;
    RTIME res;
} tbuffer;

void tbuffer_init(tbuffer *tb) {
    for (int i = 0; i < BUFFER_SIZE; i++) {
        tb->values[i] = 0;
    }
    tb->index = 0;
    tb->count = 0;
    tb->sum = 0;
}

RTIME tbuffer_add(tbuffer *tb, RTIME value) {
    if (tb->count < BUFFER_SIZE) {
        tb->count++;
    } else {
        tb->sum -= tb->values[tb->index];
    }

    tb->values[tb->index] = value;
    tb->sum += value;
    tb->index = (tb->index + 1) % BUFFER_SIZE;
}


RTIME tbuffer_get(tbuffer *tb, int index) {
    if (tb->count == 0) {
        return -1;
    }
    return tb->values[(tb->index + index) % BUFFER_SIZE];
}

RTIME tbuffer_avg(tbuffer *tb) {
    if (tb->count == 0) {
        return 0;
    }
    return tb->sum / tb->count;
    
}

RTIME tbuffer_var(tbuffer *tb) {
    if (tb->count == 0) {
        return 0;
    }
    RTIME avg = tbuffer_avg(tb);
    RTIME vsum = 0;
    for (int i = 0; i < tb->count; i++) {
        RTIME xi_avg_diff = tb->values[i] - avg;
        vsum += xi_avg_diff * xi_avg_diff;
    }
    return vsum / (tb->count - 1);
}
