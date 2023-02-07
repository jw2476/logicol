#include "draw.h"
#include "raymath.h"

Vector2 transform(Vector2 in, Vector2 offset) {
    return (Vector2){ in.x + offset.x, in.y + offset.y };
}

void draw_wire(Vector2 in, Vector2 offset, wire_direction direction) {
    Vector2 p1 = transform(in, offset);
    Vector2 p2 = transform((Vector2){ in.x + (100.0F * (float)direction), in.y }, offset);

    DrawLineEx(p1, p2, 16.0F, GRAY);
}

void draw_and(Vector2 pos) {
    Vector2 p1 = transform((Vector2){0, 0}, pos);
    Vector2 p2 = transform((Vector2){  150, 0 }, pos);
    Vector2 p3 = transform((Vector2){ 250, 100 }, pos);
    Vector2 p4 = transform((Vector2){ 150, 200 }, pos);
    Vector2 p5 = transform((Vector2){ 0, 200 }, pos);

    Vector2 c1 = transform((Vector2){ 250, 0 }, pos);
    Vector2 c2 = transform((Vector2){ 250, 200 }, pos);

    Vector2 w1 = {0, 50};
    Vector2 w2 = { 0, 150 };
    Vector2 w3 = { 250, 100 };

    draw_wire(w1, pos, LEFT);
    draw_wire(w2, pos, LEFT);
    draw_wire(w3, pos, RIGHT);

    DrawLineEx(p1, p2, 16.0F, GRAY);
    DrawLineBezierQuad(p2, p3, c1, 16.0F, GRAY);
    DrawLineBezierQuad(p3, p4, c2, 16.0F, GRAY);
    DrawLineEx(p4, p5, 16.0F, GRAY);

    // Account for thickness
    p1.y -= 8;
    p5.y += 8;
    DrawLineEx(p5, p1, 16.0F, GRAY);
}

void draw_or(Vector2 pos) {
    Vector2 p1 = transform((Vector2){ 0, 0 }, pos);
    Vector2 p2 = transform((Vector2){ 250, 100 }, pos);
    Vector2 p3 = transform((Vector2){ 0, 200 }, pos);

    Vector2 c1 = transform((Vector2){ 200, 0 }, pos);
    Vector2 c2 = transform((Vector2){ 200, 200 }, pos);
    Vector2 c3 = transform((Vector2){ 100, 100 }, pos);

    Vector2 w1 = {35, 50};
    Vector2 w2 = {35, 150};
    Vector2 w3 = { 250, 100};

    draw_wire(w1, pos, LEFT);
    draw_wire(w2, pos, LEFT);
    draw_wire(w3, pos, RIGHT);

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

void draw_circuit(circuit_circuit* circuit) {
    for (u64 i = 0; i < circuit->numComponents; i++) {
        switch (circuit->components[i].type) {
            case AND:
                draw_and(circuit->components[i].pos);
                break;
            case OR:
                draw_or(circuit->components[i].pos);
                break;
        }

        for (u32 j = 0; j < circuit->components[i].numInputs; j++) {
            if (circuit->components[i].inputs[j].component) {
                Vector2 from = Vector2Add(get_input_position(&circuit->components[i], j), (Vector2){ -100, 0 });
                Vector2 to = Vector2Add(get_output_position(circuit->components[i].inputs[j].component, j), (Vector2){ 100, 0 });
                draw_connection(from, to);
            }
        }
    }
}