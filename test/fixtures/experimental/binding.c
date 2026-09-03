#include <assert.h>
#include <bare.h>
#include <js.h>
#include <stddef.h>
#include <stdint.h>

static js_value_t *
addon_exports(js_env_t *env, js_value_t *exports) {
  int err;

  uint8_t *data;
  js_value_t *sharedarraybuffer;
  err = js_create_sharedarraybuffer(env, 8, (void **) &data, &sharedarraybuffer);
  assert(err == 0);

  data[0] = 42;

  bool is_sharedarraybuffer;
  err = js_is_sharedarraybuffer(env, sharedarraybuffer, &is_sharedarraybuffer);
  assert(err == 0);

  uint8_t *info_data;
  size_t info_len;
  err = js_get_sharedarraybuffer_info(env, sharedarraybuffer, (void **) &info_data, &info_len);
  assert(err == 0);

  err = js_set_named_property(env, exports, "sharedArrayBuffer", sharedarraybuffer);
  assert(err == 0);

  js_value_t *value;

  err = js_get_boolean(env, is_sharedarraybuffer, &value);
  assert(err == 0);

  err = js_set_named_property(env, exports, "isSharedArrayBuffer", value);
  assert(err == 0);

  err = js_get_boolean(env, info_data == data, &value);
  assert(err == 0);

  err = js_set_named_property(env, exports, "hasSameData", value);
  assert(err == 0);

  err = js_create_uint32(env, (uint32_t) info_len, &value);
  assert(err == 0);

  err = js_set_named_property(env, exports, "byteLength", value);
  assert(err == 0);

  return exports;
}

BARE_MODULE(addon, addon_exports)
