#pragma once

#include "base.h"
#include "circuit.h"

void save_circuit(circuit_circuit* circuit, const char* path);
circuit_circuit* load_circuit(circuit_library* library, const char* name);