#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef struct {
    int x1, y1;
    int x2, y2;
} WorkArea;

typedef struct {
    int x;
    int y;
} Point;

int picture_width, picture_height, color_count;

unsigned char *picture;
unsigned char *gray_picture;
unsigned char *black_white;
unsigned char *used;

WorkArea work_areas[] = {
    {640,  35,  875, 350},
    {590, 380,  985, 500},
    {628, 500, 1095, 665},
    {662, 675, 1030, 725},
    {1265, 478, 1338, 530}
};

int work_areas_count = sizeof(work_areas) / sizeof(work_areas[0]);

int is_inside_picture(int x, int y) {
    return x >= 0 && x < picture_width && y >= 0 && y < picture_height;
}

int pixel_number(int x, int y) {
    return y * picture_width + x;
}

void make_gray_picture() {
    for (int y = 0; y < picture_height; y++) {
        for (int x = 0; x < picture_width; x++) {
            int place = (y * picture_width + x) * color_count;

            unsigned char red = picture[place + 0];
            unsigned char green = picture[place + 1];
            unsigned char blue = picture[place + 2];

            int brightness = (int)(0.299 * red + 0.587 * green + 0.114 * blue);

            gray_picture[pixel_number(x, y)] = (unsigned char)brightness;
        }
    }
}

int find_brightness_border(WorkArea area, int brightness_add) {
    long long sum = 0;
    int pixels_count = 0;

    for (int y = area.y1; y < area.y2; y++) {
        for (int x = area.x1; x < area.x2; x++) {
            if (is_inside_picture(x, y)) {
                sum += gray_picture[pixel_number(x, y)];
                pixels_count++;
            }
        }
    }

    if (pixels_count == 0) {
        return 200;
    }

    int average_brightness = (int)(sum / pixels_count);
    int border = average_brightness + brightness_add;

    if (border > 245) {
        border = 245;
    }

    if (border < 80) {
        border = 80;
    }

    return border;
}

void make_black_white_in_area(WorkArea area, int brightness_add) {
    int border = find_brightness_border(area, brightness_add);

    for (int y = area.y1; y < area.y2; y++) {
        for (int x = area.x1; x < area.x2; x++) {
            if (is_inside_picture(x, y)) {
                int number = pixel_number(x, y);

                if (gray_picture[number] > border) {
                    black_white[number] = 255;
                } else {
                    black_white[number] = 0;
                }
            }
        }
    }
}

int dfs_white_spot(int start_x, int start_y, WorkArea area) {
    int max_stack_size = picture_width * picture_height;

    Point *stack = (Point*)malloc(sizeof(Point) * max_stack_size);

    if (!stack) {
        fprintf(stderr, "Memory error\n");
        exit(1);
    }

    int top = 0;
    int spot_size = 0;

    stack[top++] = (Point){start_x, start_y};
    used[pixel_number(start_x, start_y)] = 1;

    int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int dy[8] = {-1,-1,-1,  0, 0,  1, 1, 1};

    while (top > 0) {
        Point current = stack[--top];
        spot_size++;

        for (int k = 0; k < 8; k++) {
            int next_x = current.x + dx[k];
            int next_y = current.y + dy[k];

            if (next_x >= area.x1 && next_x < area.x2 &&
                next_y >= area.y1 && next_y < area.y2 &&
                is_inside_picture(next_x, next_y)) {

                int next_number = pixel_number(next_x, next_y);

                if (black_white[next_number] == 255 && !used[next_number]) {
                    used[next_number] = 1;
                    stack[top++] = (Point){next_x, next_y};
                }
            }
        }
    }

    free(stack);
    return spot_size;
}

int count_tankers_in_area(WorkArea area, int min_dot_size, int max_dot_size, int brightness_add) {
    int tankers_count = 0;

    make_black_white_in_area(area, brightness_add);

    for (int y = area.y1; y < area.y2; y++) {
        for (int x = area.x1; x < area.x2; x++) {
            if (is_inside_picture(x, y)) {
                int number = pixel_number(x, y);

                if (black_white[number] == 255 && !used[number]) {
                    int spot_size = dfs_white_spot(x, y, area);

                    if (spot_size >= min_dot_size && spot_size <= max_dot_size) {
                        tankers_count++;
                    }
                }
            }
        }
    }

    return tankers_count;
}

int main(int argc, char **argv) {
    int min_dot_size = 6;
    int max_dot_size = 30;
    int brightness_add = 70;

    if (argc < 2) {
        printf("Usage: %s image_file\n", argv[0]);
        return 1;
    }

    picture = stbi_load(argv[1], &picture_width, &picture_height, &color_count, 3);

    if (!picture) {
        fprintf(stderr, "Cannot load image: %s\n", argv[1]);
        return 1;
    }

    color_count = 3;

    gray_picture = (unsigned char*)calloc(picture_width * picture_height, sizeof(unsigned char));
    black_white = (unsigned char*)calloc(picture_width * picture_height, sizeof(unsigned char));
    used = (unsigned char*)calloc(picture_width * picture_height, sizeof(unsigned char));

    if (!gray_picture || !black_white || !used) {
        fprintf(stderr, "Memory allocation error\n");
        stbi_image_free(picture);
        return 1;
    }

    make_gray_picture();

    int all_tankers_count = 0;

    for (int i = 0; i < work_areas_count; i++) {
        int count = count_tankers_in_area(
            work_areas[i],
            min_dot_size,
            max_dot_size,
            brightness_add
        );

        all_tankers_count += count;

        printf("Area %d: %d tankers\n", i + 1, count);
    }

    printf("Total tankers: %d\n", all_tankers_count);

    free(gray_picture);
    free(black_white);
    free(used);
    stbi_image_free(picture);

    return 0;
}
