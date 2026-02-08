project('civ', 'c', meson_version: '>=1.1', version: '9999')

sources = [
  'src/app.c',
  'src/log.c',
  'src/main.c',
  'src/optional.c',
  'src/queue_family.c',
  'src/swap_chain_support.c',
]
cc = meson.get_compiler('c')

m_dep = cc.find_library('m', required: false)

rlc_dep = dependency('rlc', fallback : ['rlc', 'rlc_dep'], default_options: ['default_library=static'])
glfw_dep = dependency('glfw3')
vulkan_dep = dependency('vulkan')

app = executable('c-vulkan-triangle', sources, dependencies: [rlc_dep, glfw_dep, vulkan_dep])

