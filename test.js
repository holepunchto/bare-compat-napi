const { isBare } = require('which-runtime')
const { build } = require('./test/helpers')

build('test/fixtures/c', (t, addon) => {
  t.is(addon, 'Hello from addon')
})

build('test/fixtures/c++', (t, addon) => {
  t.is(addon, 'Hello from addon')
})

build('test/fixtures/experimental', { skip: isBare }, (t, addon) => {
  t.ok(addon.sharedArrayBuffer instanceof SharedArrayBuffer)
  t.is(addon.isSharedArrayBuffer, true)
  t.is(addon.hasSameData, true)
  t.is(addon.byteLength, 8)
  t.is(new Uint8Array(addon.sharedArrayBuffer)[0], 42)
})
