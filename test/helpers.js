const test = require('brittle')
const path = require('path')
const make = require('bare-make')
const { platform, arch } = require('which-runtime')

const modules = path.resolve(__dirname, '..', 'node_modules')

exports.build = function build(fixture, opts, assert) {
  if (typeof opts === 'function') {
    assert = opts
    opts = {}
  }

  test(fixture, { ...opts, timeout: 300000 }, async (t) => {
    const cwd = path.resolve(__dirname, '..', fixture)

    await make.generate({
      cwd,
      source: '.',
      build: 'build',
      cache: false,
      define: [`CMAKE_PREFIX_PATH=${modules}`],
      stdio: 'inherit'
    })

    await make.build({ cwd, build: 'build', clean: true, stdio: 'inherit' })

    await make.install({
      cwd,
      build: 'build',
      prefix: 'prebuilds',
      stdio: 'inherit'
    })

    assert(t, require(path.join(cwd, 'prebuilds', `${platform}-${arch}`, 'addon.node')))
  })
}
