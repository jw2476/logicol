#pragma once

#include "base.h"
#include "circuit.h"

void save_circuit(circuit_circuit* circuit, const char* path);
i64 load_circuit(circuit_library* library, const char* name);