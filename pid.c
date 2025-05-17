
#define RTIME long

typedef struct {
    double kp, ki, kd;
    RTIME prev_error, integral, output;
} pid;

void pid_reset(pid *pid) {
    pid->integral = 0;
    pid->prev_error = 0;
    pid->output = 0;
}

void pid_push(pid *p, RTIME expected, RTIME real) {
    RTIME error = expected - real;
    p->integral += error;
    RTIME dt = error - p->prev_error;
    RTIME new_output = (p->kp * error) + (p->ki * p->integral) + (p->kd * dt);

    // set for next
    p->prev_error = error;
    p->output = expected + new_output;
}
