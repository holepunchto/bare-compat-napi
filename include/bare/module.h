
#ifndef BARE_MODULE_H
#define BARE_MODULE_H

#include <js.h>
#include <node_api.h>
#include <stddef.h>

#define BARE_MODULE_VERSION 0

#define BARE_MODULE(id, fn) \
  napi_value napi_register_module_v1(napi_env env, napi_value exports) { \
    return fn(env, exports); \
  }

typedef struct bare_module_s bare_module_t;

typedef js_value_t *(*bare_module_cb)(js_env_t *env, js_value_t *exports);

/** @version 0 */
struct bare_module_s {
  int version;

  /** @since 0 */
  const char *filename;

  /** @since 0 */
  bare_module_cb init;
};

static inline void
bare_module_register(bare_module_t *mod) {
  napi_module napi_mod = {
    NAPI_MODULE_VERSION,
    0,
    mod->filename,
    mod->init,
    NULL,
    NULL,
    {0},
  };

  napi_module_register(&napi_mod);
}

#endif // BARE_MODULE_H
