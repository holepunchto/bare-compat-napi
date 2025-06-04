#ifndef BARE_MODULE_H
#define BARE_MODULE_H

#include <js.h>
#include <node_api.h>
#include <stddef.h>

#define BARE_MODULE_VERSION 0

#define BARE_MODULE(id, fn) NAPI_MODULE(id, fn)

typedef struct bare_module_s bare_module_t;

typedef js_value_t *(*bare_module_register_cb)(js_env_t *env, js_value_t *exports);

/** @version 0 */
struct bare_module_s {
  int version;

  /** @since 0 */
  const char *name;

  /** @since 0 */
  bare_module_register_cb exports;
};

static inline void
bare_module_register(bare_module_t *module) {
  napi_module napi_module = {
    NAPI_MODULE_VERSION,
    0,
    module->name,
    module->exports,
    NULL,
    NULL,
    {0},
  };

  napi_module_register(&napi_module);
}

#endif // BARE_MODULE_H
