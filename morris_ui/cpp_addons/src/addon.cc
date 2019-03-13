#include <node_api.h>
#include "example.h"
#include "simulation.h"

napi_value Init(napi_env env, napi_value exports) {
    Example::Init(env, exports);
    Simulation::Init(env, exports);
    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
