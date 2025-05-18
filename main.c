#include <linux/module.h>
#include <rtai.h>
#include <rtai_nam2num.h>
#include <rtai_sched.h>

#include "definitions.c"

#include "states.c"
#include "tbuffer.c"
#include "pid.c"
#include "led.c"


static int t_switch;

// FIX VALUES

#define STABLE_VAR 0

static pid* pidT1 = &(pid) {
    .kp = 1,
    .ki = 1,
    .kd = 1
};
static pid* pidT2 = &(pid) {
    .kp = 1,
    .ki = 1,
    .kd = 1
};

#define PID_CURRENT (t_switch ? pidT1 : pidT2)
#define PID_PREVIOUS (t_switch ? pidT1 : pidT2)

// states

static SUPER current_super;
static RUN_STATE current_run_state;


// period controls
// use ->res as a storage of T1 and T2
static tbuffer* tbufferT1;
static tbuffer* tbufferT2;

#define TBUFFER_CURRENT (t_switch ? tbufferT1 : tbufferT2)
#define TBUFFER_PREVIOUS (t_switch ? tbufferT1 : tbufferT2)

static RTIME last_total_time;


// t2L = (T2 - T1) / 2;

// run helpers

static int schedule_next_run_task() {
    if (current_super != RUN) {
        return -1;
    }
    
    // implement logic ( preferrably with get_next_run_state() )
}

static RUN_STATE get_next_run_state() {
    return (current_run_state + 1) % 4;
}


static void switch_tbuffers() {
    tbuffer* temp = tbufferT1;
    tbufferT1 = tbufferT2;
    tbufferT2 = temp;
}

static void switch_pids() {
    pid* temp = pidT1;
    pidT1 = pidT2;
    pidT2 = temp;
}


static void irq_handler() {
    RTIME now = rt_time_now();
    RTIME delta = now - last_total_time;

    // set variables for usage.
    tbuffer* tb_curr = (tbuffer*)TBUFFER_CURRENT; 
    
    tbuffer_add(tb_curr, delta);
    RTIME var = tbuffer_var(tb_curr);
    RTIME avg = tbuffer_avg(tb_curr);

    clear_screen();

    switch (current_super) {
        case IDLE:
            if (tb_curr->count == BUFFER_SIZE && var <= STABLE_VAR) {
                current_super = SYNC;
            }
            break;

        case SYNC:
            if (var > STABLE_VAR) {
                current_super = IDLE;
                break;
            }

            tbuffer* tb_prev = (tbuffer*)TBUFFER_PREVIOUS;

            tb_curr->res = avg;
            tb_prev->res = tbuffer_avg(tb_prev);

            // switches if needed...
            int isT1 = tb_curr->res < tb_prev->res;

            tbuffer* tb_T1 = isT1 ? tb_curr : tb_prev;

            if (tb_T1->res != tbufferT1->res) {
                switch_tbuffers();
                switch_pids();
            }

            current_run_state = isT1 ? LIGHT_FORWARD : DARK_LEFT;
            current_super = RUN;

            pid_reset(pidT1);
            pid_reset(pidT2);

            break;

        case RUN:
            if (var > STABLE_VAR) {
                current_super = IDLE;
                break;
            }

            current_run_state = get_next_run_state();

            if (current_run_state > LIGHT_FORWARD) {
                current_super = SYNC; // or IDLE?
                break;
            }

            pid* pid_curr = PID_CURRENT;

            pid_push(pid_curr, avg, delta);

            tb_curr->res = pid_curr->output;

            // start with something like: schedule_next_run_task();

    }

    // prepare for next interrupt
    last_total_time = now;
    t_switch = !t_switch;

    rt_ack_irq(INTERRUPT);
}



int xinit_module(void) {
    // parport...
    int ret;

    rt_printk("ParPort interrupt latency-test startet...\n");
    ret = rt_request_global_irq(INTERRUPT, (void *)irq_handler);
    rt_enable_irq(INTERRUPT);
    outb_p(0x10, BASEPORT + 2); // activate ParPort interrupt

    led_init();

    // project-specific...
    current_super = IDLE;

    tbuffer_init(&tbufferT1);
    tbuffer_init(&tbufferT2);

    t_switch = 1;
    return 0;
}

void xcleanup_module(void) {
    rt_disable_irq(INTERRUPT);
    rt_free_global_irq(INTERRUPT);
    rt_printk("ParPort interrupt latency-test completed\n");

    // possibly add other things?
}


module_init(xinit_module);
module_exit(xcleanup_module);

MODULE_LICENSE("GPL");
