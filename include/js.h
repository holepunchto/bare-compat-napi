#ifndef JS_H
#define JS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <utf.h>
#include <uv.h>

#include <node_version.h> // Node-API version information

#ifndef NAPI_VERSION
#define NAPI_VERSION NODE_API_SUPPORTED_VERSION_MAX
#endif

#include <node_api.h>

typedef struct napi_env__ js_env_t;
typedef struct napi_handle_scope__ js_handle_scope_t;
typedef struct napi_escapable_handle_scope__ js_escapable_handle_scope_t;
typedef struct napi_value__ js_value_t;
typedef struct napi_ref__ js_ref_t;
typedef struct napi_deferred__ js_deferred_t;
typedef struct napi_callback_info__ js_callback_info_t;
typedef struct napi_threadsafe_function__ js_threadsafe_function_t;
typedef struct napi_async_cleanup_hook_handle__ js_deferred_teardown_t;

typedef void *js_typed_callback_info_t;

typedef void *js_string_view_t;
typedef void *js_typedarray_view_t;
typedef void *js_dataview_view_t;

typedef struct js_type_tag_s js_type_tag_t;
typedef struct js_property_descriptor_s js_property_descriptor_t;
typedef struct js_callback_signature_s js_callback_signature_t;

typedef js_value_t *(*js_function_cb)(js_env_t *, js_callback_info_t *);
typedef void (*js_finalize_cb)(js_env_t *, void *data, void *finalize_hint);
typedef void (*js_threadsafe_function_cb)(js_env_t *, js_value_t *function, void *context, void *data);
typedef void (*js_teardown_cb)(void *data);
typedef void (*js_deferred_teardown_cb)(js_deferred_teardown_t *, void *data);

enum {
  /**
   * There's a pending exception that, unless handled, will be propagated up the
   * JavaScript execution stack.
   */
  js_pending_exception = -1,

  /**
   * There was an uncaught exception that could not be propagated as the JavaScript
   * execution stack is empty.
   */
  js_uncaught_exception = -2,
};

typedef enum {
  js_undefined = 0,
  js_null = 1,
  js_boolean = 2,
  js_number = 3,
  js_string = 4,
  js_symbol = 5,
  js_object = 6,
  js_function = 7,
  js_external = 8,
  js_bigint = 9,
} js_value_type_t;

typedef enum {
  js_int8array = 0,
  js_uint8array = 1,
  js_uint8clampedarray = 2,
  js_int16array = 3,
  js_uint16array = 4,
  js_int32array = 5,
  js_uint32array = 6,
  js_float32array = 7,
  js_float64array = 8,
  js_bigint64array = 9,
  js_biguint64array = 10,

  // Legacy aliases

  /** @deprecated */ js_int8_array = js_int8array,
  /** @deprecated */ js_uint8_array = js_uint8array,
  /** @deprecated */ js_uint8_clamped_array = js_uint8clampedarray,
  /** @deprecated */ js_int16_array = js_int16array,
  /** @deprecated */ js_uint16_array = js_uint16array,
  /** @deprecated */ js_int32_array = js_int32array,
  /** @deprecated */ js_uint32_array = js_uint32array,
  /** @deprecated */ js_float32_array = js_float32array,
  /** @deprecated */ js_float64_array = js_float64array,
  /** @deprecated */ js_bigint64_array = js_bigint64array,
  /** @deprecated */ js_biguint64_array = js_biguint64array,
} js_typedarray_type_t;

enum {
  // Numeric types

  js_int8 = 1 << 8 | js_number,
  js_uint8 = 2 << 8 | js_number,
  js_int16 = 3 << 8 | js_number,
  js_uint16 = 4 << 8 | js_number,
  js_int32 = 5 << 8 | js_number,
  js_uint32 = 6 << 8 | js_number,
  js_int64 = 7 << 8 | js_number,
  js_uint64 = 8 << 8 | js_number,
  js_float32 = 9 << 8 | js_number,
  js_float64 = 10 << 8 | js_number,
  js_bigint64 = 11 << 8 | js_number,
  js_biguint64 = 12 << 8 | js_number,
};

enum {
  js_writable = 1,
  js_enumerable = 1 << 1,
  js_configurable = 1 << 2,
  js_static = 1 << 10,
};

typedef enum {
  js_utf8 = 1,
  js_utf16le = 2,
  js_latin1 = 3,
} js_string_encoding_t;

typedef enum {
  js_threadsafe_function_release = 0,
  js_threadsafe_function_abort = 1
} js_threadsafe_function_release_mode_t;

typedef enum {
  js_threadsafe_function_nonblocking = 0,
  js_threadsafe_function_blocking = 1
} js_threadsafe_function_call_mode_t;

/** @version 0 */
struct js_property_descriptor_s {
  int version;

  /** @since 0 */
  js_value_t *name;

  /** @since 0 */
  void *data;

  /** @since 0 */
  int attributes;

  // One of:

  // Method

  /** @since 0 */
  js_function_cb method;

  // Accessor

  /** @since 0 */
  js_function_cb getter;

  /** @since 0 */
  js_function_cb setter;

  // Value

  /** @since 0 */
  js_value_t *value;
};

/** @version 0 */
struct js_type_tag_s {
  /** @since 0 */
  uint64_t lower;

  /** @since 0 */
  uint64_t upper;
};

/** @version 0 */
struct js_callback_signature_s {
  int version;

  /** @since 0 */
  int result;

  /** @since 0 */
  size_t args_len;

  /** @since 0 */
  int *args;
};

static inline int
js_convert_from_status(napi_status status) {
  switch (status) {
  case napi_ok:
    return 0;
  default:
    return js_pending_exception;
  }
}

static inline js_value_type_t
js_convert_from_valuetype(napi_valuetype type) {
  switch (type) {
  case napi_undefined:
    return js_undefined;
  case napi_null:
  default:
    return js_null;
  case napi_boolean:
    return js_boolean;
  case napi_number:
    return js_number;
  case napi_string:
    return js_string;
  case napi_symbol:
    return js_symbol;
  case napi_object:
    return js_object;
  case napi_function:
    return js_function;
  case napi_external:
    return js_external;
  case napi_bigint:
    return js_bigint;
  }
}

static inline napi_valuetype
js_convert_to_valuetype(js_value_type_t type) {
  switch (type) {
  case js_undefined:
    return napi_undefined;
  case js_null:
  default:
    return napi_null;
  case js_boolean:
    return napi_boolean;
  case js_number:
    return napi_number;
  case js_string:
    return napi_string;
  case js_symbol:
    return napi_symbol;
  case js_object:
    return napi_object;
  case js_function:
    return napi_function;
  case js_external:
    return napi_external;
  case js_bigint:
    return napi_bigint;
  }
}

static inline js_typedarray_type_t
js_convert_from_typedarray_type(napi_typedarray_type type) {
  switch (type) {
  case napi_int8_array:
    return js_int8array;
  case napi_uint8_array:
  default:
    return js_uint8array;
  case napi_uint8_clamped_array:
    return js_uint8clampedarray;
  case napi_int16_array:
    return js_int16array;
  case napi_uint16_array:
    return js_uint16array;
  case napi_int32_array:
    return js_int32array;
  case napi_uint32_array:
    return js_uint32array;
  case napi_float32_array:
    return js_float32array;
  case napi_float64_array:
    return js_float64array;
  case napi_bigint64_array:
    return js_bigint64array;
  case napi_biguint64_array:
    return js_biguint64array;
  }
}

static inline napi_typedarray_type
js_convert_to_typedarray_type(js_typedarray_type_t type) {
  switch (type) {
  case js_int8array:
    return napi_int8_array;
  case js_uint8array:
  default:
    return napi_uint8_array;
  case js_uint8clampedarray:
    return napi_uint8_clamped_array;
  case js_int16array:
    return napi_int16_array;
  case js_uint16array:
    return napi_uint16_array;
  case js_int32array:
    return napi_int32_array;
  case js_uint32array:
    return napi_uint32_array;
  case js_float32array:
    return napi_float32_array;
  case js_float64array:
    return napi_float64_array;
  case js_bigint64array:
    return napi_bigint64_array;
  case js_biguint64array:
    return napi_biguint64_array;
  }
}

#if NAPI_VERSION >= 4

static inline js_threadsafe_function_release_mode_t
js_convert_from_threadsafe_function_release_mode(napi_threadsafe_function_release_mode mode) {
  switch (mode) {
  case napi_tsfn_release:
  default:
    return js_threadsafe_function_release;
  case napi_tsfn_abort:
    return js_threadsafe_function_abort;
  }
}

static inline napi_threadsafe_function_release_mode
js_convert_to_threadsafe_function_release_mode(js_threadsafe_function_release_mode_t mode) {
  switch (mode) {
  case js_threadsafe_function_release:
  default:
    return napi_tsfn_release;
  case js_threadsafe_function_abort:
    return napi_tsfn_abort;
  }
}

static inline js_threadsafe_function_call_mode_t
js_convert_from_threadsafe_function_call_mode(napi_threadsafe_function_call_mode mode) {
  switch (mode) {
  case napi_tsfn_nonblocking:
  default:
    return js_threadsafe_function_nonblocking;
  case napi_tsfn_blocking:
    return js_threadsafe_function_blocking;
  }
}

static inline napi_threadsafe_function_call_mode
js_convert_to_threadsafe_function_call_mode(js_threadsafe_function_call_mode_t mode) {
  switch (mode) {
  case js_threadsafe_function_nonblocking:
  default:
    return napi_tsfn_nonblocking;
  case js_threadsafe_function_blocking:
    return napi_tsfn_blocking;
  }
}

#endif

#if NAPI_VERSION >= 2

static inline int
js_get_env_loop(js_env_t *env, uv_loop_t **result) {
  napi_status status = napi_get_uv_event_loop(env, result);
  return js_convert_from_status(status);
}

#endif

static inline int
js_open_handle_scope(js_env_t *env, js_handle_scope_t **result) {
  napi_status status = napi_open_handle_scope(env, result);
  return js_convert_from_status(status);
}

static inline int
js_close_handle_scope(js_env_t *env, js_handle_scope_t *scope) {
  napi_status status = napi_close_handle_scope(env, scope);
  return js_convert_from_status(status);
}

static inline int
js_open_escapable_handle_scope(js_env_t *env, js_escapable_handle_scope_t **result) {
  napi_status status = napi_open_escapable_handle_scope(env, result);
  return js_convert_from_status(status);
}

static inline int
js_close_escapable_handle_scope(js_env_t *env, js_escapable_handle_scope_t *scope) {
  napi_status status = napi_close_escapable_handle_scope(env, scope);
  return js_convert_from_status(status);
}

static inline int
js_escape_handle(js_env_t *env, js_escapable_handle_scope_t *scope, js_value_t *escapee, js_value_t **result) {
  napi_status status = napi_escape_handle(env, scope, escapee, result);
  return js_convert_from_status(status);
}

static inline int
js_run_script(js_env_t *env, const char *file, size_t len, int offset, js_value_t *source, js_value_t **result) {
  napi_status status = napi_run_script(env, source, result);
  return js_convert_from_status(status);
}

static inline int
js_create_reference(js_env_t *env, js_value_t *value, uint32_t count, js_ref_t **result) {
  napi_status status = napi_create_reference(env, value, count, result);
  return js_convert_from_status(status);
}

static inline int
js_delete_reference(js_env_t *env, js_ref_t *reference) {
  napi_status status = napi_delete_reference(env, reference);
  return js_convert_from_status(status);
}

static inline int
js_reference_ref(js_env_t *env, js_ref_t *reference, uint32_t *result) {
  napi_status status = napi_reference_ref(env, reference, result);
  return js_convert_from_status(status);
}

static inline int
js_reference_unref(js_env_t *env, js_ref_t *reference, uint32_t *result) {
  napi_status status = napi_reference_unref(env, reference, result);
  return js_convert_from_status(status);
}

static inline int
js_get_reference_value(js_env_t *env, js_ref_t *reference, js_value_t **result) {
  napi_status status = napi_get_reference_value(env, reference, result);
  return js_convert_from_status(status);
}

static inline int
js_define_class(js_env_t *env, const char *name, size_t len, js_function_cb constructor, void *data, js_property_descriptor_t const properties[], size_t properties_len, js_value_t **result) {
  napi_property_descriptor *napi_properties = malloc(sizeof(napi_property_descriptor) * properties_len);

  for (size_t i = 0; i < properties_len; i++) {
    const js_property_descriptor_t *property = &properties[i];

    napi_property_descriptor *napi_property = &napi_properties[i];

    napi_property->name = property->name;
    napi_property->utf8name = NULL;

    napi_property->method = property->method;
    napi_property->getter = property->getter;
    napi_property->setter = property->setter;
    napi_property->value = property->value;

    napi_property->attributes = property->attributes;
    napi_property->data = property->data;
  }

  napi_status status = napi_define_class(env, name, len, constructor, data, properties_len, napi_properties, result);

  free(napi_properties);

  return js_convert_from_status(status);
}

static inline int
js_define_properties(js_env_t *env, js_value_t *object, js_property_descriptor_t const properties[], size_t properties_len) {
  napi_property_descriptor *napi_properties = malloc(sizeof(napi_property_descriptor) * properties_len);

  for (size_t i = 0; i < properties_len; i++) {
    const js_property_descriptor_t *property = &properties[i];

    napi_property_descriptor *napi_property = &napi_properties[i];

    napi_property->name = property->name;
    napi_property->utf8name = NULL;

    napi_property->method = property->method;
    napi_property->getter = property->getter;
    napi_property->setter = property->setter;
    napi_property->value = property->value;

    napi_property->attributes = property->attributes;
    napi_property->data = property->data;
  }

  napi_status status = napi_define_properties(env, object, properties_len, napi_properties);

  free(napi_properties);

  return js_convert_from_status(status);
}

static inline int
js_wrap(js_env_t *env, js_value_t *object, void *data, js_finalize_cb finalize_cb, void *finalize_hint, js_ref_t **result) {
  napi_status status = napi_wrap(env, object, data, finalize_cb, finalize_hint, result);
  return js_convert_from_status(status);
}

static inline int
js_unwrap(js_env_t *env, js_value_t *object, void **result) {
  napi_status status = napi_unwrap(env, object, result);
  return js_convert_from_status(status);
}

static inline int
js_remove_wrap(js_env_t *env, js_value_t *object, void **result) {
  napi_status status = napi_remove_wrap(env, object, result);
  return js_convert_from_status(status);
}

#if NAPI_VERSION >= 8

static inline int
js_add_type_tag(js_env_t *env, js_value_t *object, const js_type_tag_t *tag) {
  napi_status status = napi_type_tag_object(env, object, &(napi_type_tag) {tag->lower, tag->upper});
  return js_convert_from_status(status);
}

static inline int
js_check_type_tag(js_env_t *env, js_value_t *object, const js_type_tag_t *tag, bool *result) {
  napi_status status = napi_check_object_type_tag(env, object, &(napi_type_tag) {tag->lower, tag->upper}, result);
  return js_convert_from_status(status);
}

#endif

#if NAPI_VERSION >= 5

static inline int
js_add_finalizer(js_env_t *env, js_value_t *object, void *data, js_finalize_cb finalize_cb, void *finalize_hint, js_ref_t **result) {
  napi_status status = napi_add_finalizer(env, object, data, finalize_cb, finalize_hint, result);
  return js_convert_from_status(status);
}

#endif

static inline int
js_create_int32(js_env_t *env, int32_t value, js_value_t **result) {
  napi_status status = napi_create_int32(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_create_uint32(js_env_t *env, uint32_t value, js_value_t **result) {
  napi_status status = napi_create_uint32(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_create_int64(js_env_t *env, int64_t value, js_value_t **result) {
  napi_status status = napi_create_int64(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_create_double(js_env_t *env, double value, js_value_t **result) {
  napi_status status = napi_create_double(env, value, result);
  return js_convert_from_status(status);
}

#if NAPI_VERSION >= 6

static inline int
js_create_bigint_int64(js_env_t *env, int64_t value, js_value_t **result) {
  napi_status status = napi_create_bigint_int64(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_create_bigint_uint64(js_env_t *env, uint64_t value, js_value_t **result) {
  napi_status status = napi_create_bigint_uint64(env, value, result);
  return js_convert_from_status(status);
}

#endif

static inline int
js_create_string_utf8(js_env_t *env, const utf8_t *str, size_t len, js_value_t **result) {
  napi_status status = napi_create_string_utf8(env, str, len, result);
  return js_convert_from_status(status);
}

static inline int
js_create_string_utf16le(js_env_t *env, const utf16_t *str, size_t len, js_value_t **result) {
  napi_status status = napi_create_string_utf16(env, str, len, result);
  return js_convert_from_status(status);
}

static inline int
js_create_string_latin1(js_env_t *env, const latin1_t *str, size_t len, js_value_t **result) {
  napi_status status = napi_create_string_latin1(env, str, len, result);
  return js_convert_from_status(status);
}

static inline int
js_create_external_string_utf8(js_env_t *env, utf8_t *str, size_t len, js_finalize_cb finalize_cb, void *finalize_hint, js_value_t **result, bool *copied) {
  if (copied) *copied = true;

  napi_status status = js_create_string_utf8(env, str, len, result);

  if (status == napi_ok && finalize_cb) finalize_cb(env, str, finalize_hint);

  return js_convert_from_status(status);
}

static inline int
js_create_external_string_utf16le(js_env_t *env, utf16_t *str, size_t len, js_finalize_cb finalize_cb, void *finalize_hint, js_value_t **result, bool *copied) {
#if NAPI_VERSION >= 10
  napi_status status = node_api_create_external_string_utf16(env, str, len, finalize_cb, finalize_hint, result, copied);
#else
  if (copied) *copied = true;

  napi_status status = js_create_string_utf16le(env, str, len, result);

  if (status == napi_ok && finalize_cb) finalize_cb(env, str, finalize_hint);
#endif
  return js_convert_from_status(status);
}

static inline int
js_create_external_string_latin1(js_env_t *env, latin1_t *str, size_t len, js_finalize_cb finalize_cb, void *finalize_hint, js_value_t **result, bool *copied) {
#if NAPI_VERSION >= 10
  napi_status status = node_api_create_external_string_latin1(env, str, len, finalize_cb, finalize_hint, result, copied);
#else
  if (copied) *copied = true;

  napi_status status = js_create_string_latin1(env, str, len, result);

  if (status == napi_ok && finalize_cb) finalize_cb(env, str, finalize_hint);
#endif
  return js_convert_from_status(status);
}

static inline int
js_create_property_key_utf8(js_env_t *env, const utf8_t *str, size_t len, js_value_t **result) {
#if NAPI_VERSION >= 10
  napi_status status = node_api_create_property_key_utf8(env, str, len, result);
#else
  napi_status status = napi_create_string_utf8(env, str, len, result);
#endif
  return js_convert_from_status(status);
}

static inline int
js_create_property_key_utf16le(js_env_t *env, const utf16_t *str, size_t len, js_value_t **result) {
#if NAPI_VERSION >= 10
  napi_status status = node_api_create_property_key_utf16(env, str, len, result);
#else
  napi_status status = napi_create_string_utf16(env, str, len, result);
#endif
  return js_convert_from_status(status);
}

static inline int
js_create_property_key_latin1(js_env_t *env, const latin1_t *str, size_t len, js_value_t **result) {
#if NAPI_VERSION >= 10
  napi_status status = node_api_create_property_key_latin1(env, str, len, result);
#else
  napi_status status = napi_create_string_latin1(env, str, len, result);
#endif
  return js_convert_from_status(status);
}

static inline int
js_create_symbol(js_env_t *env, js_value_t *description, js_value_t **result) {
  napi_status status = napi_create_symbol(env, description, result);
  return js_convert_from_status(status);
}

#if NAPI_VERSION >= 9

static inline int
js_symbol_for(js_env_t *env, const char *description, size_t len, js_value_t **result) {
  napi_status status = node_api_symbol_for(env, description, len, result);
  return js_convert_from_status(status);
}

#endif

static inline int
js_create_object(js_env_t *env, js_value_t **result) {
  napi_status status = napi_create_object(env, result);
  return js_convert_from_status(status);
}

static inline int
js_create_function(js_env_t *env, const char *name, size_t len, js_function_cb cb, void *data, js_value_t **result) {
  napi_status status = napi_create_function(env, name, len, cb, data, result);
  return js_convert_from_status(status);
}

static inline int
js_create_typed_function(js_env_t *env, const char *name, size_t len, js_function_cb cb, const js_callback_signature_t *signature, const void *address, void *data, js_value_t **result) {
  return js_create_function(env, name, len, cb, data, result);
}

static inline int
js_create_array(js_env_t *env, js_value_t **result) {
  napi_status status = napi_create_array(env, result);
  return js_convert_from_status(status);
}

static inline int
js_create_array_with_length(js_env_t *env, size_t len, js_value_t **result) {
  napi_status status = napi_create_array_with_length(env, len, result);
  return js_convert_from_status(status);
}

static inline int
js_create_external(js_env_t *env, void *data, js_finalize_cb finalize_cb, void *finalize_hint, js_value_t **result) {
  napi_status status = napi_create_external(env, data, finalize_cb, finalize_hint, result);
  return js_convert_from_status(status);
}

#if NAPI_VERSION >= 5

static inline int
js_create_date(js_env_t *env, double time, js_value_t **result) {
  napi_status status = napi_create_date(env, time, result);
  return js_convert_from_status(status);
}

#endif

static inline int
js_create_error(js_env_t *env, js_value_t *code, js_value_t *message, js_value_t **result) {
  napi_status status = napi_create_error(env, code, message, result);
  return js_convert_from_status(status);
}

static inline int
js_create_type_error(js_env_t *env, js_value_t *code, js_value_t *message, js_value_t **result) {
  napi_status status = napi_create_type_error(env, code, message, result);
  return js_convert_from_status(status);
}

static inline int
js_create_range_error(js_env_t *env, js_value_t *code, js_value_t *message, js_value_t **result) {
  napi_status status = napi_create_range_error(env, code, message, result);
  return js_convert_from_status(status);
}

#if NAPI_VERSION >= 9

static inline int
js_create_syntax_error(js_env_t *env, js_value_t *code, js_value_t *message, js_value_t **result) {
  napi_status status = node_api_create_syntax_error(env, code, message, result);
  return js_convert_from_status(status);
}

#endif

static inline int
js_create_promise(js_env_t *env, js_deferred_t **deferred, js_value_t **promise) {
  napi_status status = napi_create_promise(env, deferred, promise);
  return js_convert_from_status(status);
}

static inline int
js_resolve_deferred(js_env_t *env, js_deferred_t *deferred, js_value_t *resolution) {
  napi_status status = napi_resolve_deferred(env, deferred, resolution);
  return js_convert_from_status(status);
}

static inline int
js_reject_deferred(js_env_t *env, js_deferred_t *deferred, js_value_t *resolution) {
  napi_status status = napi_reject_deferred(env, deferred, resolution);
  return js_convert_from_status(status);
}

static inline int
js_create_arraybuffer(js_env_t *env, size_t len, void **data, js_value_t **result) {
  napi_status status = napi_create_arraybuffer(env, len, data, result);
  return js_convert_from_status(status);
}

static inline int
js_create_external_arraybuffer(js_env_t *env, void *data, size_t len, js_finalize_cb finalize_cb, void *finalize_hint, js_value_t **result) {
  napi_status status = napi_create_external_arraybuffer(env, data, len, finalize_cb, finalize_hint, result);
  return js_convert_from_status(status);
}

#if NAPI_VERSION >= 7

static inline int
js_detach_arraybuffer(js_env_t *env, js_value_t *arraybuffer) {
  napi_status status = napi_detach_arraybuffer(env, arraybuffer);
  return js_convert_from_status(status);
}

#endif

static inline int
js_create_typedarray(js_env_t *env, js_typedarray_type_t type, size_t len, js_value_t *arraybuffer, size_t offset, js_value_t **result) {
  napi_status status = napi_create_typedarray(env, js_convert_to_typedarray_type(type), len, arraybuffer, offset, result);
  return js_convert_from_status(status);
}

static inline int
js_create_dataview(js_env_t *env, size_t len, js_value_t *arraybuffer, size_t offset, js_value_t **result) {
  napi_status status = napi_create_dataview(env, len, arraybuffer, offset, result);
  return js_convert_from_status(status);
}

static inline int
js_coerce_to_boolean(js_env_t *env, js_value_t *value, js_value_t **result) {
  napi_status status = napi_coerce_to_bool(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_coerce_to_number(js_env_t *env, js_value_t *value, js_value_t **result) {
  napi_status status = napi_coerce_to_number(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_coerce_to_string(js_env_t *env, js_value_t *value, js_value_t **result) {
  napi_status status = napi_coerce_to_string(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_coerce_to_object(js_env_t *env, js_value_t *value, js_value_t **result) {
  napi_status status = napi_coerce_to_object(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_typeof(js_env_t *env, js_value_t *value, js_value_type_t *result) {
  napi_valuetype napi_type;

  napi_status status = napi_typeof(env, value, &napi_type);

  if (status == napi_ok) *result = js_convert_from_valuetype(napi_type);

  return js_convert_from_status(status);
}

static inline int
js_instanceof(js_env_t *env, js_value_t *object, js_value_t *constructor, bool *result) {
  napi_status status = napi_instanceof(env, object, constructor, result);
  return js_convert_from_status(status);
}

static inline int
js_is_undefined(js_env_t *env, js_value_t *value, bool *result) {
  napi_valuetype napi_type;

  napi_status status = napi_typeof(env, value, &napi_type);

  if (status == napi_ok) *result = napi_type == napi_undefined;

  return js_convert_from_status(status);
}

static inline int
js_is_null(js_env_t *env, js_value_t *value, bool *result) {
  napi_valuetype napi_type;

  napi_status status = napi_typeof(env, value, &napi_type);

  if (status == napi_ok) *result = napi_type == napi_null;

  return js_convert_from_status(status);
}

static inline int
js_is_boolean(js_env_t *env, js_value_t *value, bool *result) {
  napi_valuetype napi_type;

  napi_status status = napi_typeof(env, value, &napi_type);

  if (status == napi_ok) *result = napi_type == napi_boolean;

  return js_convert_from_status(status);
}

static inline int
js_is_number(js_env_t *env, js_value_t *value, bool *result) {
  napi_valuetype napi_type;

  napi_status status = napi_typeof(env, value, &napi_type);

  if (status == napi_ok) *result = napi_type == napi_number;

  return js_convert_from_status(status);
}

static inline int
js_is_string(js_env_t *env, js_value_t *value, bool *result) {
  napi_valuetype napi_type;

  napi_status status = napi_typeof(env, value, &napi_type);

  if (status == napi_ok) *result = napi_type == napi_string;

  return js_convert_from_status(status);
}

static inline int
js_is_symbol(js_env_t *env, js_value_t *value, bool *result) {
  napi_valuetype napi_type;

  napi_status status = napi_typeof(env, value, &napi_type);

  if (status == napi_ok) *result = napi_type == napi_symbol;

  return js_convert_from_status(status);
}

static inline int
js_is_object(js_env_t *env, js_value_t *value, bool *result) {
  napi_valuetype napi_type;

  napi_status status = napi_typeof(env, value, &napi_type);

  if (status == napi_ok) *result = napi_type == napi_object;

  return js_convert_from_status(status);
}

static inline int
js_is_function(js_env_t *env, js_value_t *value, bool *result) {
  napi_valuetype napi_type;

  napi_status status = napi_typeof(env, value, &napi_type);

  if (status == napi_ok) *result = napi_type == napi_function;

  return js_convert_from_status(status);
}

static inline int
js_is_array(js_env_t *env, js_value_t *value, bool *result) {
  napi_status status = napi_is_array(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_is_external(js_env_t *env, js_value_t *value, bool *result) {
  napi_valuetype napi_type;

  napi_status status = napi_typeof(env, value, &napi_type);

  if (status == napi_ok) *result = napi_type == napi_external;

  return js_convert_from_status(status);
}

static inline int
js_is_bigint(js_env_t *env, js_value_t *value, bool *result) {
  napi_valuetype napi_type;

  napi_status status = napi_typeof(env, value, &napi_type);

  if (status == napi_ok) *result = napi_type == napi_bigint;

  return js_convert_from_status(status);
}

#if NAPI_VERSION >= 5

static inline int
js_is_date(js_env_t *env, js_value_t *value, bool *result) {
  napi_status status = napi_is_date(env, value, result);
  return js_convert_from_status(status);
}

#endif

static inline int
js_is_error(js_env_t *env, js_value_t *value, bool *result) {
  napi_status status = napi_is_error(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_is_promise(js_env_t *env, js_value_t *value, bool *result) {
  napi_status status = napi_is_promise(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_is_arraybuffer(js_env_t *env, js_value_t *value, bool *result) {
  napi_status status = napi_is_arraybuffer(env, value, result);
  return js_convert_from_status(status);
}

#if NAPI_VERSION >= 7

static inline int
js_is_detached_arraybuffer(js_env_t *env, js_value_t *value, bool *result) {
  napi_status status = napi_is_detached_arraybuffer(env, value, result);
  return js_convert_from_status(status);
}

#endif

static inline int
js_is_typedarray(js_env_t *env, js_value_t *value, bool *result) {
  napi_status status = napi_is_typedarray(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_is_dataview(js_env_t *env, js_value_t *value, bool *result) {
  napi_status status = napi_is_dataview(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_strict_equals(js_env_t *env, js_value_t *a, js_value_t *b, bool *result) {
  napi_status status = napi_strict_equals(env, a, b, result);
  return js_convert_from_status(status);
}

static inline int
js_get_global(js_env_t *env, js_value_t **result) {
  napi_status status = napi_get_global(env, result);
  return js_convert_from_status(status);
}

static inline int
js_get_undefined(js_env_t *env, js_value_t **result) {
  napi_status status = napi_get_undefined(env, result);
  return js_convert_from_status(status);
}

static inline int
js_get_null(js_env_t *env, js_value_t **result) {
  napi_status status = napi_get_null(env, result);
  return js_convert_from_status(status);
}

static inline int
js_get_boolean(js_env_t *env, bool value, js_value_t **result) {
  napi_status status = napi_get_boolean(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_get_value_bool(js_env_t *env, js_value_t *value, bool *result) {
  napi_status status = napi_get_value_bool(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_get_value_int32(js_env_t *env, js_value_t *value, int32_t *result) {
  napi_status status = napi_get_value_int32(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_get_value_uint32(js_env_t *env, js_value_t *value, uint32_t *result) {
  napi_status status = napi_get_value_uint32(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_get_value_int64(js_env_t *env, js_value_t *value, int64_t *result) {
  napi_status status = napi_get_value_int64(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_get_value_double(js_env_t *env, js_value_t *value, double *result) {
  napi_status status = napi_get_value_double(env, value, result);
  return js_convert_from_status(status);
}

#if NAPI_VERSION >= 6

static inline int
js_get_value_bigint_int64(js_env_t *env, js_value_t *value, int64_t *result, bool *lossless) {
  napi_status status = napi_get_value_bigint_int64(env, value, result, lossless);
  return js_convert_from_status(status);
}

static inline int
js_get_value_bigint_uint64(js_env_t *env, js_value_t *value, uint64_t *result, bool *lossless) {
  napi_status status = napi_get_value_bigint_uint64(env, value, result, lossless);
  return js_convert_from_status(status);
}

#endif

static inline int
js_get_value_string_utf8(js_env_t *env, js_value_t *value, utf8_t *str, size_t len, size_t *result) {
  napi_status status = napi_get_value_string_utf8(env, value, str, len, result);
  return js_convert_from_status(status);
}

static inline int
js_get_value_string_utf16le(js_env_t *env, js_value_t *value, utf16_t *str, size_t len, size_t *result) {
  napi_status status = napi_get_value_string_utf16(env, value, str, len, result);
  return js_convert_from_status(status);
}

static inline int
js_get_value_external(js_env_t *env, js_value_t *value, void **result) {
  napi_status status = napi_get_value_external(env, value, result);
  return js_convert_from_status(status);
}

#if NAPI_VERSION >= 5

static inline int
js_get_value_date(js_env_t *env, js_value_t *value, double *result) {
  napi_status status = napi_get_date_value(env, value, result);
  return js_convert_from_status(status);
}

#endif

static inline int
js_get_array_length(js_env_t *env, js_value_t *value, uint32_t *result) {
  napi_status status = napi_get_array_length(env, value, result);
  return js_convert_from_status(status);
}

static inline int
js_get_prototype(js_env_t *env, js_value_t *object, js_value_t **result) {
  napi_status status = napi_get_prototype(env, object, result);
  return js_convert_from_status(status);
}

static inline int
js_get_property_names(js_env_t *env, js_value_t *object, js_value_t **result) {
  napi_status status = napi_get_property_names(env, object, result);
  return js_convert_from_status(status);
}

static inline int
js_get_property(js_env_t *env, js_value_t *object, js_value_t *key, js_value_t **result) {
  napi_status status = napi_get_property(env, object, key, result);
  return js_convert_from_status(status);
}

static inline int
js_has_property(js_env_t *env, js_value_t *object, js_value_t *key, bool *result) {
  napi_status status = napi_has_property(env, object, key, result);
  return js_convert_from_status(status);
}

static inline int
js_set_property(js_env_t *env, js_value_t *object, js_value_t *key, js_value_t *value) {
  napi_status status = napi_set_property(env, object, key, value);
  return js_convert_from_status(status);
}

static inline int
js_delete_property(js_env_t *env, js_value_t *object, js_value_t *key, bool *result) {
  napi_status status = napi_delete_property(env, object, key, result);
  return js_convert_from_status(status);
}

static inline int
js_get_named_property(js_env_t *env, js_value_t *object, const char *name, js_value_t **result) {
  napi_status status = napi_get_named_property(env, object, name, result);
  return js_convert_from_status(status);
}

static inline int
js_has_named_property(js_env_t *env, js_value_t *object, const char *name, bool *result) {
  napi_status status = napi_has_named_property(env, object, name, result);
  return js_convert_from_status(status);
}

static inline int
js_set_named_property(js_env_t *env, js_value_t *object, const char *name, js_value_t *value) {
  napi_status status = napi_set_named_property(env, object, name, value);
  return js_convert_from_status(status);
}

static inline int
js_delete_named_property(js_env_t *env, js_value_t *object, const char *name, bool *result) {
  napi_status status;

  napi_value key;
  status = napi_create_string_utf8(env, name, -1, &key);

  if (status == napi_ok) status = napi_delete_property(env, object, key, result);

  return js_convert_from_status(status);
}

static inline int
js_get_element(js_env_t *env, js_value_t *object, uint32_t index, js_value_t **result) {
  napi_status status = napi_get_element(env, object, index, result);
  return js_convert_from_status(status);
}

static inline int
js_has_element(js_env_t *env, js_value_t *object, uint32_t index, bool *result) {
  napi_status status = napi_has_element(env, object, index, result);
  return js_convert_from_status(status);
}

static inline int
js_set_element(js_env_t *env, js_value_t *object, uint32_t index, js_value_t *value) {
  napi_status status = napi_set_element(env, object, index, value);
  return js_convert_from_status(status);
}

static inline int
js_delete_element(js_env_t *env, js_value_t *object, uint32_t index, bool *result) {
  napi_status status = napi_delete_element(env, object, index, result);
  return js_convert_from_status(status);
}

static inline int
js_set_array_elements(js_env_t *env, js_value_t *array, const js_value_t *elements[], size_t len, size_t offset) {
  napi_status status = 0;
  for (size_t i = 0; i < len; i++) {
    status = napi_set_element(env, array, offset + i, (js_value_t *) elements[i]);
    if (status != 0) break;
  }
  return js_convert_from_status(status);
}

static inline int
js_get_array_elements(js_env_t *env, js_value_t *array, js_value_t **elements, size_t len, size_t offset, uint32_t *result) {
  napi_status status = 0;
  *result = 0;

  for (; offset < len; offset++) {
    status = napi_get_element(env, array, offset, &elements[offset]);

    if (status == napi_ok) ++*result;
    else break;
  }

  return js_convert_from_status(status);
}

static inline int
js_get_callback_info(js_env_t *env, const js_callback_info_t *info, size_t *argc, js_value_t *argv[], js_value_t **receiver, void **data) {
  napi_status status = napi_get_cb_info(env, (js_callback_info_t *) info, argc, argv, receiver, data);
  return js_convert_from_status(status);
}

static inline int
js_get_typed_callback_info(const js_typed_callback_info_t *info, js_env_t **env, void **data) {
  return 0;
}

static inline int
js_get_new_target(js_env_t *env, const js_callback_info_t *info, js_value_t **result) {
  napi_status status = napi_get_new_target(env, (js_callback_info_t *) info, result);
  return js_convert_from_status(status);
}

static inline int
js_get_arraybuffer_info(js_env_t *env, js_value_t *arraybuffer, void **data, size_t *len) {
  napi_status status = napi_get_arraybuffer_info(env, arraybuffer, data, len);
  return js_convert_from_status(status);
}

static inline int
js_get_typedarray_info(js_env_t *env, js_value_t *typedarray, js_typedarray_type_t *type, void **data, size_t *len, js_value_t **arraybuffer, size_t *offset) {
  napi_typedarray_type napi_type;

  napi_status status = napi_get_typedarray_info(env, typedarray, type == NULL ? NULL : &napi_type, len, data, arraybuffer, offset);

  if (status == napi_ok && type) *type = js_convert_from_typedarray_type(napi_type);

  return 0;
}

static inline int
js_get_dataview_info(js_env_t *env, js_value_t *dataview, void **data, size_t *len, js_value_t **arraybuffer, size_t *offset) {
  napi_status status = napi_get_dataview_info(env, dataview, len, data, arraybuffer, offset);
  return js_convert_from_status(status);
}

static inline int
js_get_string_view(js_env_t *env, js_value_t *string, js_string_encoding_t *encoding, const void **str, size_t *len, js_string_view_t **result) {
  size_t view_len;

  napi_status status = napi_get_value_string_utf8(env, string, NULL, 0, &view_len);

  if (status == napi_ok) {
    void *view = malloc(view_len + 1 /* NULL */);

    status = napi_get_value_string_utf8(env, string, view, view_len + 1 /* NULL */, NULL);

    if (encoding) *encoding = js_utf8;

    if (str) *str = view;

    if (len) *len = view_len;

    *result = view;
  }

  return js_convert_from_status(status);
}

static inline int
js_release_string_view(js_env_t *env, js_string_view_t *view) {
  free(view);

  return 0;
}

static inline int
js_get_typedarray_view(js_env_t *env, js_value_t *typedarray, js_typedarray_type_t *type, void **data, size_t *len, js_typedarray_view_t **result) {
  return js_get_typedarray_info(env, typedarray, type, data, len, NULL, NULL);
}

static inline int
js_release_typedarray_view(js_env_t *env, js_typedarray_view_t *view) {
  return 0;
}

static inline int
js_get_dataview_view(js_env_t *env, js_value_t *dataview, void **data, size_t *len, js_dataview_view_t **result) {
  return js_get_dataview_info(env, dataview, data, len, NULL, NULL);
}

static inline int
js_release_dataview_view(js_env_t *env, js_dataview_view_t *view) {
  return 0;
}

static inline int
js_call_function(js_env_t *env, js_value_t *receiver, js_value_t *function, size_t argc, js_value_t *const argv[], js_value_t **result) {
  napi_status status = napi_call_function(env, receiver, function, argc, argv, result);
  return js_convert_from_status(status);
}

#if NAPI_VERSION >= 3

static inline int
js_call_function_with_checkpoint(js_env_t *env, js_value_t *receiver, js_value_t *function, size_t argc, js_value_t *const argv[], js_value_t **result) {
  napi_status status = napi_make_callback(env, NULL, receiver, function, argc, argv, result);

  if (status == napi_pending_exception) {
    napi_status status;

    napi_value error;
    status = napi_get_and_clear_last_exception(env, &error);
    assert(status == napi_ok);

    status = napi_fatal_exception(env, error);
    assert(status == napi_ok);

    (void) (status);

    return js_uncaught_exception;
  }

  return js_convert_from_status(status);
}

#endif

static inline int
js_new_instance(js_env_t *env, js_value_t *constructor, size_t argc, js_value_t *const argv[], js_value_t **result) {
  napi_status status = napi_new_instance(env, constructor, argc, argv, result);
  return js_convert_from_status(status);
}

#if NAPI_VERSION >= 7

static inline int
js_create_threadsafe_function(js_env_t *env, js_value_t *function, size_t queue_limit, size_t initial_thread_count, js_finalize_cb finalize_cb, void *finalize_hint, void *context, js_threadsafe_function_cb cb, js_threadsafe_function_t **result) {
  napi_status status;

  napi_value resource_name;
  status = napi_create_string_utf8(env, "js_threadsafe_function_t", NAPI_AUTO_LENGTH, &resource_name);
  assert(status == napi_ok);

  status = napi_create_threadsafe_function(env, function, NULL, resource_name, queue_limit, initial_thread_count, finalize_hint, finalize_cb, context, cb, result);

  return js_convert_from_status(status);
}

static inline int
js_get_threadsafe_function_context(js_threadsafe_function_t *function, void **result) {
  napi_status status = napi_get_threadsafe_function_context(function, result);
  return js_convert_from_status(status);
}

static inline int
js_call_threadsafe_function(js_threadsafe_function_t *function, void *data, js_threadsafe_function_call_mode_t mode) {
  napi_status status = napi_call_threadsafe_function(function, data, js_convert_to_threadsafe_function_call_mode(mode));
  return js_convert_from_status(status);
}

static inline int
js_acquire_threadsafe_function(js_threadsafe_function_t *function) {
  napi_status status = napi_acquire_threadsafe_function(function);
  return js_convert_from_status(status);
}

static inline int
js_release_threadsafe_function(js_threadsafe_function_t *function, js_threadsafe_function_release_mode_t mode) {
  napi_status status = napi_release_threadsafe_function(function, js_convert_to_threadsafe_function_release_mode(mode));
  return js_convert_from_status(status);
}

static inline int
js_ref_threadsafe_function(js_env_t *env, js_threadsafe_function_t *function) {
  napi_status status = napi_ref_threadsafe_function(env, function);
  return js_convert_from_status(status);
}

static inline int
js_unref_threadsafe_function(js_env_t *env, js_threadsafe_function_t *function) {
  napi_status status = napi_unref_threadsafe_function(env, function);
  return js_convert_from_status(status);
}

#endif

#if NAPI_VERSION >= 3

static inline int
js_add_teardown_callback(js_env_t *env, js_teardown_cb callback, void *data) {
  napi_status status = napi_add_env_cleanup_hook(env, callback, data);
  return js_convert_from_status(status);
}

static inline int
js_remove_teardown_callback(js_env_t *env, js_teardown_cb callback, void *data) {
  napi_status status = napi_remove_env_cleanup_hook(env, callback, data);
  return js_convert_from_status(status);
}

#endif

#if NAPI_VERSION >= 8

static inline int
js_add_deferred_teardown_callback(js_env_t *env, js_deferred_teardown_cb callback, void *data, js_deferred_teardown_t **result) {
  napi_status status = napi_add_async_cleanup_hook(env, callback, data, result);
  return js_convert_from_status(status);
}

static inline int
js_finish_deferred_teardown_callback(js_deferred_teardown_t *handle) {
  napi_status status = napi_remove_async_cleanup_hook(handle);
  return js_convert_from_status(status);
}

#endif

static inline int
js_throw(js_env_t *env, js_value_t *error) {
  napi_status status = napi_throw(env, error);
  return js_convert_from_status(status);
}

static inline int
js_vformat(char **result, size_t *size, const char *message, va_list args) {
  va_list args_copy;
  va_copy(args_copy, args);

  int res = vsnprintf(NULL, 0, message, args_copy);

  va_end(args_copy);

  if (res < 0) return res;

  *size = res + 1 /* NULL */;
  *result = malloc(*size);

  va_copy(args_copy, args);

  vsnprintf(*result, *size, message, args_copy);

  va_end(args_copy);

  return 0;
}

static inline int
js_throw_error(js_env_t *env, const char *code, const char *message) {
  napi_status status = napi_throw_error(env, code, message);
  return js_convert_from_status(status);
}

static inline int
js_throw_verrorf(js_env_t *env, const char *code, const char *message, va_list args) {
  size_t len;
  char *formatted;
  js_vformat(&formatted, &len, message, args);

  int err = js_throw_error(env, code, formatted);

  free(formatted);

  return err;
}

static inline int
js_throw_errorf(js_env_t *env, const char *code, const char *message, ...) {
  va_list args;
  va_start(args, message);

  int err = js_throw_verrorf(env, code, message, args);

  va_end(args);

  return err;
}

static inline int
js_throw_type_error(js_env_t *env, const char *code, const char *message) {
  napi_status status = napi_throw_type_error(env, code, message);
  return js_convert_from_status(status);
}

static inline int
js_throw_type_verrorf(js_env_t *env, const char *code, const char *message, va_list args) {
  size_t len;
  char *formatted;
  js_vformat(&formatted, &len, message, args);

  int err = js_throw_type_error(env, code, formatted);

  free(formatted);

  return err;
}

static inline int
js_throw_type_errorf(js_env_t *env, const char *code, const char *message, ...) {
  va_list args;
  va_start(args, message);

  int err = js_throw_type_verrorf(env, code, message, args);

  va_end(args);

  return err;
}

static inline int
js_throw_range_error(js_env_t *env, const char *code, const char *message) {
  napi_status status = napi_throw_range_error(env, code, message);
  return js_convert_from_status(status);
}

static inline int
js_throw_range_verrorf(js_env_t *env, const char *code, const char *message, va_list args) {
  size_t len;
  char *formatted;
  js_vformat(&formatted, &len, message, args);

  int err = js_throw_range_error(env, code, formatted);

  free(formatted);

  return err;
}

static inline int
js_throw_range_errorf(js_env_t *env, const char *code, const char *message, ...) {
  va_list args;
  va_start(args, message);

  int err = js_throw_range_verrorf(env, code, message, args);

  va_end(args);

  return err;
}

#if NAPI_VERSION >= 9

static inline int
js_throw_syntax_error(js_env_t *env, const char *code, const char *message) {
  napi_status status = node_api_throw_syntax_error(env, code, message);
  return js_convert_from_status(status);
}

static inline int
js_throw_syntax_verrorf(js_env_t *env, const char *code, const char *message, va_list args) {
  size_t len;
  char *formatted;
  js_vformat(&formatted, &len, message, args);

  int err = js_throw_syntax_error(env, code, formatted);

  free(formatted);

  return err;
}

static inline int
js_throw_syntax_errorf(js_env_t *env, const char *code, const char *message, ...) {
  va_list args;
  va_start(args, message);

  int err = js_throw_syntax_verrorf(env, code, message, args);

  va_end(args);

  return err;
}

#endif

static inline int
js_is_exception_pending(js_env_t *env, bool *result) {
  napi_status status = napi_is_exception_pending(env, result);
  return js_convert_from_status(status);
}

static inline int
js_get_and_clear_last_exception(js_env_t *env, js_value_t **result) {
  napi_status status = napi_get_and_clear_last_exception(env, result);
  return js_convert_from_status(status);
}

#if NAPI_VERSION >= 3

static inline int
js_fatal_exception(js_env_t *env, js_value_t *error) {
  napi_status status = napi_fatal_exception(env, error);
  return js_convert_from_status(status);
}

#endif

static inline int
js_adjust_external_memory(js_env_t *env, int64_t change_in_bytes, int64_t *result) {
  napi_status status = napi_adjust_external_memory(env, change_in_bytes, result);
  return js_convert_from_status(status);
}

#ifdef __cplusplus
}
#endif

#endif // JS_H
