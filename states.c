#define RUN_STATES 4

// Super states
typedef enum {
    IDLE = 0,
    SYNC = 1,
    RUN = 2
} SUPER;

// Sub states for RUN Super State
typedef enum {
    DARK_LEFT = 0,
    LIGHT_FORWARD = 1,
    DARK_RIGHT = 2,
    LIGHT_BACKWARD = 3
} RUN_STATE;

