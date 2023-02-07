#include "draw.h"
#include "raymath.h"

Vector2 transform(Vector2 in, Vector2 offset) {
    return (Vector2){ in.x + offset.x, in.y + offset.y };
}

void draw_wire(Vector2 in, wire_direction direction) {
    Vector2 p1 = in;
    Vector2 p2 = { in.x + (100.0F * (float)direction), in.y };

    DrawLineEx(p1, p2, 16.0F, WIRE);
}

void draw_and(circuit_component* component) {
    Vector2 p1 = transform((Vector2){0, 0}, component->pos);
    Vector2 p2 = transform((Vector2){  150, 0 }, component->pos);
    Vector2 p3 = transform((Vector2){ 250, 100 }, component->pos);
    Vector2 p4 = transform((Vector2){ 150, 200 }, component->pos);
    Vector2 p5 = transform((Vector2){ 0, 200 }, component->pos);

    Vector2 c1 = transform((Vector2){ 250, 0 }, component->pos);
    Vector2 c2 = transform((Vector2){ 250, 200 }, component->pos);

    Vector2 w1 = {0, 50};
    Vector2 w2 = { 0, 150 };
    Vector2 w3 = { 250, 100 };

    draw_wire(get_input_position(component, 0), LEFT);
    draw_wire(get_input_position(component, 1), LEFT);
    draw_wire(get_output_position(component, 0), RIGHT);

    DrawLineEx(p1, p2, 16.0F, COMPONENT);
    DrawLineBezierQuad(p2, p3, c1, 16.0F, COMPONENT);
    DrawLineBezierQuad(p3, p4, c2, 16.0F, COMPONENT);
    DrawLineEx(p4, p5, 16.0F, COMPONENT);

    // Account for thickness
    p1.y -= 8;
    p5.y += 8;
    DrawLineEx(p5, p1, 16.0F, COMPONENT);
}

void draw_or(circuit_component* component) {
    Vector2 p1 = transform((Vector2){ 0, 0 }, component->pos);
    Vector2 p2 = transform((Vector2){ 250, 100 }, component->pos);
    Vector2 p3 = transform((Vector2){ 0, 200 }, component->pos);

    Vector2 c1 = transform((Vector2){ 200, 0 }, component->pos);
    Vector2 c2 = transform((Vector2){ 200, 200 }, component->pos);
    Vector2 c3 = transform((Vector2){ 100, 100 }, component->pos);

    draw_wire(get_input_position(component, 0), LEFT);
    draw_wire(get_input_position(component, 1), LEFT);
    draw_wire(get_output_position(component, 0), RIGHT);

    DrawLineBezierQuad(p1, p2, c1, 16.0F, COMPONENT);
    DrawLineBezierQuad(p2, p3, c2, 16.0F, COMPONENT);

    // Account for thickness
    p1.x += 5.3F;
    p3.x += 5.3F;
    DrawLineBezierQuad(p3, p1, c3, 16.0F, COMPONENT);
}

void draw_not(circuit_component* component) {
    Vector2 p1 = transform((Vector2){ 0, 0 }, component->pos);
    Vector2 p2 = transform((Vector2){ 200, 100}, component->pos);
    Vector2 p3 = transform((Vector2){ 0, 200 }, component->pos);
    Vector2 p4 = transform((Vector2){ 225, 100 }, component->pos);

    draw_wire(get_input_position(component, 0), LEFT);
    draw_wire(get_output_position(component, 0), RIGHT);

    DrawLineEx(p1, p2, 16.0F, COMPONENT);
    DrawLineEx(p2, p3, 16.0F, COMPONENT);
    DrawLineEx(p3, p1, 16.0F, COMPONENT);
    DrawCircle((int)p4.x, (int)p4.y, 27, COMPONENT);
}

void draw_input(circuit_component* component) {
    Rectangle rectangle = { component->pos.x, component->pos.y, 250, 200 };

    DrawRectangleLinesEx(rectangle, 16.0F, COMPONENT);

    draw_wire(get_output_position(component, 0), RIGHT);
}

void draw_output(circuit_component* component) {
    Rectangle rectangle = { component->pos.x, component->pos.y, 250, 200 };

    DrawRectangleLinesEx(rectangle, 16.0F, COMPONENT);

    draw_wire(get_input_position(component, 0), LEFT);
}

void draw_connection(Vector2 from, Vector2 to) {
    DrawLineBezier(from, to, 8.0F, CONNECTION);
}

typedef void(*draw_function)(circuit_component*);

static draw_function draw_functions[] = { draw_and, draw_or, draw_not, draw_input, draw_output };

void draw_circuit(circuit_circuit* circuit) {
    for (u64 i = 0; i < circuit->numComponents; i++) {
        draw_functions[circuit->components[i].type](&circuit->components[i]);

        for (u32 j = 0; j < circuit->components[i].numInputs; j++) {
            if (circuit->components[i].inputs[j].component) {
                Vector2 from = Vector2Add(get_input_position(&circuit->components[i], j), (Vector2){ -100, 0 });
                Vector2 to = Vector2Add(get_output_position(circuit->components[i].inputs[j].component, j), (Vector2){ 100, 0 });
                draw_connection(from, to);
            }
        }
    }
}