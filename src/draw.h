#pragma once

#include "base.h"
#include "raylib.h"
#include "circuit.h"

void draw_and(circuit_component* component);
void draw_or(circuit_component* component);
void draw_not(circuit_component* component);
void draw_connection(Vector2 from, Vector2 to);

void draw_circuit(circuit_circuit* circuit);