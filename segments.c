#define SCREEN_WIDTH_PIXEL 120
#define EMPTY_CHAR 0xFF


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
// !!! maybe solve with apres_empty_count and avant_empty_count instead of huge array...
typedef struct {
    char rend[SCREEN_WIDTH_PIXEL];
    char rend_length;
    Alignment rend_alignment;
    int index;
} SegmentsRendered;

char get(SegmentsRendered *r, int index) {
    return validate_led_index(index) ? r->rend[index] : EMPTY_CHAR;
}

char next(SegmentsRendered *r) {
    char ret = get(r, r->index);
    r->index = (r->index + 1) % SCREEN_WIDTH_PIXEL;
    return ret;
}

char prev(SegmentsRendered *r) {
    r->index = (r->index - 1) % SCREEN_WIDTH_PIXEL;
    return get(r, r->index);
}


typedef struct {
    char virt[SCREEN_WIDTH_PIXEL];
    char virt_length;
} Segments;

void clear(Segments *s) {
    s->virt_length = 0;
}

void addSegment(Segments *s, char segment) {
    if (s->virt_length >= SCREEN_WIDTH_PIXEL) {
        return -1;
    }
    s->virt[s->virt_length] = segment;
    s->virt_length++;
}

char letter_to_segment(char letter) {
    return 0;
}

void setText(Segments *s, char *letters) {
    clear(s);
    for (int i = 0; letters[i] != '\0'; i++) {
        char segment = letter_to_segment(letters[i]);
        addSegment(s, segment);
    }
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
