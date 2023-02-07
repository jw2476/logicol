#pragma once

#include "base.h"
#include "raylib.h"
#include "circuit.h"

void draw_and(Vector2 pos);
void draw_or(Vector2 pos);
void draw_connection(Vector2 from, Vector2 to);

void draw_circuit(circuit_circuit* circuit);