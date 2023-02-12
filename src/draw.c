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

void draw_and(circuit_circuit* circuit, circuit_component* component) {
    Vector2 p1 = transform((Vector2){0, 0}, component->pos);
    Vector2 p2 = transform((Vector2){  150, 0 }, component->pos);
    Vector2 p3 = transform((Vector2){ 250, 100 }, component->pos);
    Vector2 p4 = transform((Vector2){ 150, 200 }, component->pos);
    Vector2 p5 = transform((Vector2){ 0, 200 }, component->pos);

    Vector2 c1 = transform((Vector2){ 250, 0 }, component->pos);
    Vector2 c2 = transform((Vector2){ 250, 200 }, component->pos);

    draw_wire(get_input_position(circuit, component, 0), LEFT);
    draw_wire(get_input_position(circuit, component, 1), LEFT);
    draw_wire(get_output_position(component, 0), RIGHT);

    DrawLineEx(p1, p2, 16.0F, COMPONENT);
    DrawLineBezierQuad(p2, p3, c1, 16.0F, COMPONENT);
    DrawLineBezierQuad(p3, p4, c2, 16.0F, COMPONENT);
    DrawLineEx(p4, p5, 16.0F, COMPONENT);

    // Account for thickness
    p1.y -= 8;
    p5.y += 8;
    DrawLineEx(p5, p1, 16.0F, COMPONENT);

    DrawText(component->name, 50 + (int)component->pos.x, 68 + (int)component->pos.y, 64, COMPONENT);
}

void draw_nand(circuit_circuit* circuit, circuit_component* component) {
    Vector2 p1 = transform((Vector2){0, 0}, component->pos);
    Vector2 p2 = transform((Vector2){  100, 0 }, component->pos);
    Vector2 p3 = transform((Vector2){ 200, 100 }, component->pos);
    Vector2 p4 = transform((Vector2){ 100, 200 }, component->pos);
    Vector2 p5 = transform((Vector2){ 0, 200 }, component->pos);
    Vector2 p6 = transform((Vector2){ 225, 100 }, component->pos);

    Vector2 c1 = transform((Vector2){ 200, 0 }, component->pos);
    Vector2 c2 = transform((Vector2){ 200, 200 }, component->pos);

    draw_wire(get_input_position(circuit, component, 0), LEFT);
    draw_wire(get_input_position(circuit, component, 1), LEFT);
    draw_wire(get_output_position(component, 0), RIGHT);

    DrawLineEx(p1, p2, 16.0F, COMPONENT);
    DrawLineBezierQuad(p2, p3, c1, 16.0F, COMPONENT);
    DrawLineBezierQuad(p3, p4, c2, 16.0F, COMPONENT);
    DrawLineEx(p4, p5, 16.0F, COMPONENT);
    DrawCircleV(p6, 27, COMPONENT);

    // Account for thickness
    p1.y -= 8;
    p5.y += 8;
    DrawLineEx(p5, p1, 16.0F, COMPONENT);

    DrawText(component->name, 20 + (int)component->pos.x, 76 + (int)component->pos.y, 48, COMPONENT);
}

void draw_or(circuit_circuit* circuit, circuit_component* component) {
    Vector2 p1 = transform((Vector2){ 0, 0 }, component->pos);
    Vector2 p2 = transform((Vector2){ 250, 100 }, component->pos);
    Vector2 p3 = transform((Vector2){ 0, 200 }, component->pos);

    Vector2 c1 = transform((Vector2){ 200, 0 }, component->pos);
    Vector2 c2 = transform((Vector2){ 200, 200 }, component->pos);
    Vector2 c3 = transform((Vector2){ 100, 100 }, component->pos);

    draw_wire(get_input_position(circuit, component, 0), LEFT);
    draw_wire(get_input_position(circuit, component, 1), LEFT);
    draw_wire(get_output_position(component, 0), RIGHT);

    DrawLineBezierQuad(p1, p2, c1, 16.0F, COMPONENT);
    DrawLineBezierQuad(p2, p3, c2, 16.0F, COMPONENT);

    // Account for thickness
    p1.x += 5.3F;
    p3.x += 5.3F;
    DrawLineBezierQuad(p3, p1, c3, 16.0F, COMPONENT);

    DrawText(component->name, 90 + (int)component->pos.x, 68 + (int)component->pos.y, 64, COMPONENT);
}

void draw_not(circuit_circuit* circuit, circuit_component* component) {
    Vector2 p1 = transform((Vector2){ 0, 0 }, component->pos);
    Vector2 p2 = transform((Vector2){ 200, 100}, component->pos);
    Vector2 p3 = transform((Vector2){ 0, 200 }, component->pos);
    Vector2 p4 = transform((Vector2){ 225, 100 }, component->pos);

    draw_wire(get_input_position(circuit, component, 0), LEFT);
    draw_wire(get_output_position(component, 0), RIGHT);

    DrawLineEx(p1, p2, 16.0F, COMPONENT);
    DrawLineEx(p2, p3, 16.0F, COMPONENT);
    DrawLineEx(p3, p1, 16.0F, COMPONENT);
    DrawCircleV(p4, 27, COMPONENT);

    DrawText(component->name, 20 + (int)component->pos.x, 76 + (int)component->pos.y, 48, COMPONENT);
}

void draw_buffer(circuit_circuit* circuit, circuit_component* component) {
    Vector2 p1 = transform((Vector2){ 0, 0 }, component->pos);
    Vector2 p2 = transform((Vector2){ 250, 100}, component->pos);
    Vector2 p3 = transform((Vector2){ 0, 200 }, component->pos);

    draw_wire(get_input_position(circuit, component, 0), LEFT);
    draw_wire(get_output_position(component, 0), RIGHT);

    DrawLineEx(p1, p2, 16.0F, component->internallyActive ? ON : OFF);
    DrawLineEx(p2, p3, 16.0F, component->internallyActive ? ON : OFF);
    DrawLineEx(p3, p1, 16.0F, component->internallyActive ? ON : OFF);

    DrawText(component->name, 20 + (int)component->pos.x, 76 + (int)component->pos.y, 48, component->internallyActive ? ON : OFF);
}

void draw_input(circuit_circuit* _, circuit_component* component) {
    Rectangle rectangle = { component->pos.x, component->pos.y, 250, 200 };

    DrawRectangleLinesEx(rectangle, 16.0F, component->internallyActive ? ON : OFF);

    draw_wire(get_output_position(component, 0), RIGHT);

    DrawText(component->name, 50 + (int)component->pos.x, 68 + (int)component->pos.y, 64, component->internallyActive ? ON : OFF);
}

void draw_output(circuit_circuit* circuit, circuit_component* component) {
    Rectangle rectangle = { component->pos.x, component->pos.y, 250, 200 };

    DrawRectangleLinesEx(rectangle, 16.0F, component->internallyActive ? ON : OFF);

    draw_wire(get_input_position(circuit, component, 0), LEFT);

    DrawText(component->name, 50 + (int)component->pos.x, 68 + (int)component->pos.y, 64, component->internallyActive ? ON : OFF);
}

void draw_custom(circuit_circuit* circuit, circuit_component* component) {
    Rectangle rectangle = { component->pos.x, component->pos.y, 250, 200 };

    DrawRectangleLinesEx(rectangle, 16.0F, COMPONENT);

    circuit_graph_node* node = circuit_graph_find(circuit->components, component);
    u32 numInputs = circuit_graph_edge_list_length(node->edges);

    for (u32 i = 0; i < numInputs; i++) {
        draw_wire(get_input_position(circuit, component, i), LEFT);
    }

    for (u32 i = 0; i < component->numOutputs; i++) {
        draw_wire(get_output_position(component, i), RIGHT);
    }

    DrawText(component->name, 50 + (int)component->pos.x, 68 + (int)component->pos.y, 64, COMPONENT);
}

void draw_connection(Vector2 from, Vector2 to, bool on) {
    DrawLineBezier(from, to, 8.0F, on ? ON : OFF);
}

typedef void(*draw_function)(circuit_circuit*, circuit_component*);

static draw_function draw_functions[] = { draw_and, draw_nand,draw_or, draw_not, draw_input, draw_output, draw_buffer, draw_custom };

void draw_circuit(circuit_circuit* circuit) {
    for (circuit_graph_node_list* nodeItem = circuit->components->nodes; nodeItem != NULL; nodeItem = nodeItem->next) {
        if (nodeItem->data == NULL) break;

        circuit_graph_node* node = nodeItem->data;
        circuit_component* component = node->data;

        draw_functions[component->type](circuit, component);

        u32 j = 0;
        for (circuit_graph_edge_list* edgeItem = node->edges; edgeItem != NULL; edgeItem = edgeItem->next) {
            circuit_graph_edge* edge = edgeItem->data;

            if (edge != NULL && edge->data != NULL && edge->node != NULL) {
                Vector2 from = Vector2Add(get_input_position(circuit, component, j), (Vector2){ -100, 0 });
                Vector2 to = Vector2Add(get_output_position(edge->node->data, edge->data->output), (Vector2){ 100, 0 });
                draw_connection(from, to, edge->data->on);
            }

            j++;
        }
    }
}