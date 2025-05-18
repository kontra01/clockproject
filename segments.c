
#include "definitions.c"

// TODO: include "zeichen.c" list and implement it here


int validate_led_segment(char segment) {
    return 1;
}

int validate_led_index(int index) {
    return index >= 0 && index < SCREEN_WIDTH_PIXEL;
}


typedef enum {
    LEFT = -1,
    CENTERED = 0,
    RIGHT = 1
} Alignment;

// note: could be problematic if width gets too big.
typedef struct {
    char rend[SCREEN_WIDTH_PIXEL];
    char rend_length;
    Alignment rend_alignment;
    int index;
} SegmentsRendered;

char rendsegments_get(SegmentsRendered *r, int index) {
    return validate_led_index(index) ? r->rend[index] : EMPTY_CHAR;
}

char rendsegments_next(SegmentsRendered *r) {
    char ret = rendsegments_get(r, r->index);
    r->index = (r->index + 1) % SCREEN_WIDTH_PIXEL;
    return ret;
}

char rendsegments_prev(SegmentsRendered *r) {
    r->index = (r->index - 1) % SCREEN_WIDTH_PIXEL;
    return rendsegments_get(r, r->index);
}


typedef struct {
    char virt[SCREEN_WIDTH_PIXEL];
    char virt_length;
} Segments;

void segments_clear(Segments *s) {
    s->virt_length = 0;
}

int segments_add(Segments *s, char segment) {
    if (s->virt_length >= SCREEN_WIDTH_PIXEL) {
        return -1;
    }
    s->virt[s->virt_length] = segment;
    s->virt_length++;
    return 0;
}

char* letter_to_segments(char letter) {
    // TODO: implement zeichen here!
    return 0;
}

int segments_addletter(Segments *s, char letter) {
    char* segments = letter_to_segments(letter);
    // TODO: find length
    // memcpy(s->virt+virt_length, segments, length?);
    // or iterate through and stop at identified EMPTY_CHAR
}

int segments_addtext(Segments *s, char *letters) {
    for (int i = 0; letters[i] != '\0'; i++) {
        int ret = segments_addletter(s, letters[i]);
        if (ret < 0) return -1;
    }
    return 0;
}

SegmentsRendered* render(Segments *s, Alignment alignment) {
    SegmentsRendered* r;
    r->rend_length = s->virt_length;
    r->rend_alignment = alignment;
    r->index = 0;

    int left = 0, right = 0;

    switch (alignment) {
        case LEFT:
            right = SCREEN_WIDTH_PIXEL - s->virt_length;
            break;
        case CENTERED:
            // this is fine here because they aren't getting changed anymore
            left = right = (SCREEN_WIDTH_PIXEL - s->virt_length) / 2;
            break;
        case RIGHT:
            left = SCREEN_WIDTH_PIXEL - s->virt_length;
            break;
    }

    if (left > 0) memset(r->rend, EMPTY_CHAR, left);
    if (right > 0) memset(r->rend + left + s->virt_length, EMPTY_CHAR, right);
    memcpy(r->rend + left, s->virt, s->virt_length);

    return r;
}
