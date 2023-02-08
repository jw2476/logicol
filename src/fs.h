#pragma once

#include "base.h"
#include "circuit.h"

void save_circuit(circuit_circuit* circuit, const char* path);
void load_circuit(circuit_circuit* circuit, const char* path);