#include "raylib.h"
#include "base.h"

typedef enum wire_direction_t {
    LEFT = -1,
    RIGHT = 1
} wire_direction;

Vector2 transform(Vector2 in, float x, float y) {
    return (Vector2){ in.x + x, in.y + y };
}

void draw_wire(Vector2 in, float x, float y, wire_direction direction) {
    Vector2 p1 = transform(in, x, y);
    Vector2 p2 = transform((Vector2){ in.x + (100.0F * (float)direction), in.y }, x, y);

    DrawLineEx(p1, p2, 16.0F, GRAY);
}

void draw_and(float x, float y) {
    Vector2 p1 = transform((Vector2){0, 0}, x, y);
    Vector2 p2 = transform((Vector2){  150, 0 }, x, y);
    Vector2 p3 = transform((Vector2){ 250, 100 }, x, y);
    Vector2 p4 = transform((Vector2){ 150, 200 }, x, y);
    Vector2 p5 = transform((Vector2){ 0, 200 }, x, y);

    Vector2 c1 = transform((Vector2){ 250, 0 }, x, y);
    Vector2 c2 = transform((Vector2){ 250, 200 }, x, y);

    Vector2 w1 = {0, 50};
    Vector2 w2 = { 0, 150 };
    Vector2 w3 = { 250, 100 };

    draw_wire(w1, x, y, LEFT);
    draw_wire(w2, x, y, LEFT);
    draw_wire(w3, x, y, RIGHT);

    DrawLineEx(p1, p2, 16.0F, GRAY);
    DrawLineBezierQuad(p2, p3, c1, 16.0F, GRAY);
    DrawLineBezierQuad(p3, p4, c2, 16.0F, GRAY);
    DrawLineEx(p4, p5, 16.0F, GRAY);

    // Account for thickness
    p1.y -= 8;
    p5.y += 8;
    DrawLineEx(p5, p1, 16.0F, GRAY);
}

void draw_or(float x, float y) {
    Vector2 p1 = transform((Vector2){ 0, 0 }, x, y);
    Vector2 p2 = transform((Vector2){ 250, 100 }, x, y);
    Vector2 p3 = transform((Vector2){ 0, 200 }, x, y);

    Vector2 c1 = transform((Vector2){ 200, 0 }, x, y);
    Vector2 c2 = transform((Vector2){ 200, 200 }, x, y);
    Vector2 c3 = transform((Vector2){ 100, 100 }, x, y);

    Vector2 w1 = {35, 50};
    Vector2 w2 = {35, 150};
    Vector2 w3 = { 250, 100};

    draw_wire(w1, x, y, LEFT);
    draw_wire(w2, x, y, LEFT);
    draw_wire(w3, x, y, RIGHT);

    DrawLineBezierQuad(p1, p2, c1, 16.0F, GRAY);
    DrawLineBezierQuad(p2, p3, c2, 16.0F, GRAY);

    // Account for thickness
    p1.x += 5.3F;
    p3.x += 5.3F;
    DrawLineBezierQuad(p3, p1, c3, 16.0F, GRAY);
}

void draw_connection(Vector2 from, Vector2 to) {
    DrawLineBezier(from, to, 8.0F, GRAY);
}


int main(void) {
    i32 width = GetScreenWidth();
    i32 height = GetScreenHeight();
    InitWindow(width, height, "raylib [core] example - basic window");
    ToggleFullscreen();

    Camera2D camera;
    CLEAR(camera);

    camera.zoom = 1.0F;

    while (!WindowShouldClose())
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            camera.target.x -= GetMouseDelta().x * (1/camera.zoom);
            camera.target.y -= GetMouseDelta().y * (1/camera.zoom);
        }

        float deltaZoom = (float)GetMouseWheelMove() * 0.05F;

        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

        camera.offset = GetMousePosition();

        camera.target = mouseWorldPos;

        camera.zoom += deltaZoom;

        if (camera.zoom > 3.0F) camera.zoom = 3.0F;
        else if (camera.zoom < 0.1F) camera.zoom = 0.1F;

        BeginDrawing();
        BeginMode2D(camera);
        {
            ClearBackground(RAYWHITE);
            draw_and(100, 100);
            draw_or(600, 100);
            draw_connection((Vector2){450, 200}, (Vector2){550, 150});
        }
        EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
