{
  'variables': {
    'deps': '../',
    'v8_use_siphash%': 0,
    'v8_trace_maps%': 0,
    'use_node_zlib%': 'true',
    'enable_hilog%' : 'true',
    'v8_enable_pointer_compression%': 1,
    'v8_enable_31bit_smis_on_64bit_arch%': 0,
    'node_use_dtrace%': 'false',
    'node_use_etw%': 'false',
    'node_no_browser_globals%': 'false',
    'node_snapshot_main%': '',
    'node_use_node_snapshot%': 'false',
    'node_use_v8_platform%': 'true',
    'node_use_bundled_v8%': 'true',
    'node_shared%': 'false',
    'force_dynamic_crt%': 0,
    'ossfuzz' : 'false',
    'node_module_version%': '',
    'node_shared_brotli%': 'false',
    'node_shared_zlib%': 'false',
    'node_shared_http_parser%': 'false',
    'node_shared_cares%': 'false',
    'node_shared_libuv%': 'false',
    'node_shared_nghttp2%': 'false',
    'node_use_openssl%': 'true',
    'node_shared_openssl%': 'false',
    'node_v8_options%': '',
    'node_enable_v8_vtunejit%': 'false',
    'node_core_target_name%': 'node',
    'node_lib_target_name%': 'libjsvm',
    'node_intermediate_lib_type%': 'static_library',
    'node_builtin_modules_path%': '',
    'TARGET_DIR': '$(TARGET_GEN_DIR)',
    'NDK_SYS_ROOT': '<(TARGET_DIR)/../../../../..',
    'linked_module_files': [
    ],
    # We list the deps/ files out instead of globbing them in js2c.py since we
    # only include a subset of all the files under these directories.
    # The lengths of their file names combined should not exceed the
    # Windows command length limit or there would be an error.
    # See https://docs.microsoft.com/en-us/troubleshoot/windows-client/shell-experience/command-line-string-limitation
    'library_files': [
      '<@(node_library_files)',
      '<@(linked_module_files)',
    ],
    'deps_files': [
      '<(deps)/v8/tools/splaytree.mjs',
      '<(deps)/v8/tools/codemap.mjs',
      '<(deps)/v8/tools/consarray.mjs',
      '<(deps)/v8/tools/csvparser.mjs',
      '<(deps)/v8/tools/profile.mjs',
      '<(deps)/v8/tools/profile_view.mjs',
      '<(deps)/v8/tools/logreader.mjs',
      '<(deps)/v8/tools/arguments.mjs',
      '<(deps)/v8/tools/tickprocessor.mjs',
      '<(deps)/v8/tools/sourcemap.mjs',
      '<(deps)/v8/tools/tickprocessor-driver.mjs',
      'deps/acorn/acorn/dist/acorn.js',
      'deps/acorn/acorn-walk/dist/walk.js',
      'deps/minimatch/index.js',
      '<@(node_builtin_shareable_builtins)',
    ],
    'node_mksnapshot_exec': '<(PRODUCT_DIR)/<(EXECUTABLE_PREFIX)node_mksnapshot<(EXECUTABLE_SUFFIX)',
    'conditions': [
      ['GENERATOR == "ninja"', {
        'node_text_start_object_path': 'src/large_pages/node_text_start.node_text_start.o'
      }, {
        'node_text_start_object_path': 'node_text_start/src/large_pages/node_text_start.o'
      }],
      [ 'node_shared=="true"', {
        'node_target_type%': 'shared_library',
        'conditions': [
          ['OS in "aix os400"', {
            # For AIX, always generate static library first,
            # It needs an extra step to generate exp and
            # then use both static lib and exp to create
            # shared lib.
            'node_intermediate_lib_type': 'static_library',
          }, {
            'node_intermediate_lib_type': 'shared_library',
          }],
        ],
      }, {
        'node_target_type%': 'executable',
      }],
      [ 'OS=="win" and '
        'node_use_openssl=="true" and '
        'node_shared_openssl=="false"', {
        'use_openssl_def%': 1,
      }, {
        'use_openssl_def%': 0,
      }],
    ],
  },

  'target_defaults': {
    # Putting these explicitly here so not to depend on `common.gypi`.
    # `common.gypi` need to be more general because it is used to build userland native addons.
    # Refs: https://github.com/nodejs/node-gyp/issues/1118
    'cflags': [ '-Wall', '-Wextra', '-Wno-unused-parameter', ],
    'xcode_settings': {
      'WARNING_CFLAGS': [
        '-Wall',
        '-Wendif-labels',
        '-W',
        '-Wno-unused-parameter',
        '-Werror=undefined-inline',
        '-Werror=extra-semi',
      ],
    },

    # Relevant only for x86.
    # Refs: https://github.com/nodejs/node/pull/25852
    # Refs: https://docs.microsoft.com/en-us/cpp/build/reference/safeseh-image-has-safe-exception-handlers
    'msvs_settings': {
      'VCLinkerTool': {
        'ImageHasSafeExceptionHandlers': 'false',
      },
    },

    'conditions': [
      ['OS in "aix os400"', {
        'ldflags': [
          '-Wl,-bnoerrmsg',
        ],
      }],
      #['OS == "linux" and llvm_version != "0.0"', {
      #  'libraries': ['-latomic'],
      #}],
    ],
  },

  'targets': [
    {
      'target_name': 'node_text_start',
      'type': 'none',
      'conditions': [
        [ 'OS in "linux freebsd solaris" and '
          'target_arch=="x64"', {
          'type': 'static_library',
          'sources': [
            'src/large_pages/node_text_start.S'
          ]
        }],
      ]
    },
    {
      'target_name': '<(node_core_target_name)',
      'type': 'executable',

      'defines': [
        'NODE_ARCH="<(target_arch)"',
        'NODE_PLATFORM="<(OS)"',
        'NODE_WANT_INTERNALS=1',
      ],

      'includes': [
        'node.gypi'
      ],

      'include_dirs': [
        'src',
        '<(deps)/v8/include',
        'deps/postject'
      ],

      'sources': [
        'src/node_main.cc'
      ],

      'dependencies': [
        'deps/histogram/histogram.gyp:histogram',
        'deps/uvwasi/uvwasi.gyp:uvwasi',
      ],

      'msvs_settings': {
        'VCLinkerTool': {
          'GenerateMapFile': 'true', # /MAP
          'MapExports': 'true', # /MAPINFO:EXPORTS
          'RandomizedBaseAddress': 2, # enable ASLR
          'DataExecutionPrevention': 2, # enable DEP
          'AllowIsolation': 'true',
          # By default, the MSVC linker only reserves 1 MiB of stack memory for
          # each thread, whereas other platforms typically allow much larger
          # stack memory sections. We raise the limit to make it more consistent
          # across platforms and to support the few use cases that require large
          # amounts of stack memory, without having to modify the node binary.
          'StackReserveSize': 0x800000,
        },
      },

      # - "C4244: conversion from 'type1' to 'type2', possible loss of data"
      #   Ususaly safe. Disable for `dep`, enable for `src`
      'msvs_disabled_warnings!': [4244],

      'conditions': [
        [ 'error_on_warn=="true"', {
          'cflags': ['-Werror'],
          'xcode_settings': {
            'WARNING_CFLAGS': [ '-Werror' ],
          },
        }],
        [ 'node_intermediate_lib_type=="static_library" and '
            'node_shared=="true" and OS in "aix os400"', {
          # For AIX, shared lib is linked by static lib and .exp. In the
          # case here, the executable needs to link to shared lib.
          # Therefore, use 'node_aix_shared' target to generate the
          # shared lib and then executable.
          'dependencies': [ 'node_aix_shared' ],
        }, {
          'dependencies': [ '<(node_lib_target_name)' ],
          'conditions': [
            ['OS=="win" and node_shared=="true"', {
              'dependencies': ['generate_node_def'],
              'msvs_settings': {
                'VCLinkerTool': {
                  'ModuleDefinitionFile': '<(PRODUCT_DIR)/<(node_core_target_name).def',
                },
              },
            }],
          ],
        }],
        [ 'node_intermediate_lib_type=="static_library" and node_shared=="false"', {
          'xcode_settings': {
            'OTHER_LDFLAGS': [
              '-Wl,-force_load,<(PRODUCT_DIR)/<(STATIC_LIB_PREFIX)<(node_core_target_name)<(STATIC_LIB_SUFFIX)',
              '-Wl,-force_load,<(PRODUCT_DIR)/<(STATIC_LIB_PREFIX)v8_base_without_compiler<(STATIC_LIB_SUFFIX)',
            ],
          },
          'msvs_settings': {
            'VCLinkerTool': {
              'AdditionalOptions': [
                '/WHOLEARCHIVE:<(node_lib_target_name)<(STATIC_LIB_SUFFIX)',
                '/WHOLEARCHIVE:<(STATIC_LIB_PREFIX)v8_base_without_compiler<(STATIC_LIB_SUFFIX)',
              ],
            },
          },
          'conditions': [
            ['OS != "aix" and OS != "os400" and OS != "mac" and OS != "ios"', {
              'ldflags': [
                '-Wl,--whole-archive',
                '<(obj_dir)/<(STATIC_LIB_PREFIX)<(node_core_target_name)<(STATIC_LIB_SUFFIX)',
                '<(obj_dir)/tools/v8_gypfiles/<(STATIC_LIB_PREFIX)v8_base_without_compiler<(STATIC_LIB_SUFFIX)',
                '-Wl,--no-whole-archive',
              ],
            }],
            [ 'OS=="win"', {
              'sources': [ 'src/res/node.rc' ],
              'conditions': [
                [ 'node_use_etw=="true"', {
                  'sources': [
                    'tools/msvs/genfiles/node_etw_provider.rc'
                  ],
                }],
              ],
            }],
          ],
        }],
        [ 'node_shared=="true"', {
          'xcode_settings': {
            'OTHER_LDFLAGS': [ '-Wl,-rpath,@loader_path', '-Wl,-rpath,@loader_path/../lib'],
          },
          'conditions': [
            ['OS=="linux"', {
               'ldflags': [
                 '-Wl,-rpath,\\$$ORIGIN/../lib'
               ],
            }],
          ],
        }],
        [ 'enable_lto=="true"', {
          'xcode_settings': {
            'OTHER_LDFLAGS': [
              # man ld -export_dynamic:
              # Preserves all global symbols in main executables during LTO.
              # Without this option, Link Time Optimization is allowed to
              # inline and remove global functions. This option is used when
              # a main executable may load a plug-in which requires certain
              # symbols from the main executable.
              '-Wl,-export_dynamic',
            ],
          },
        }],
        ['OS=="win"', {
          'libraries': [
            'Dbghelp.lib',
            'winmm.lib',
            'Ws2_32.lib',
          ],
        }],
        ['node_with_ltcg=="true"', {
          'msvs_settings': {
            'VCCLCompilerTool': {
              'WholeProgramOptimization': 'true'   # /GL, whole program optimization, needed for LTCG
            },
            'VCLibrarianTool': {
              'AdditionalOptions': [
                '/LTCG:INCREMENTAL',               # link time code generation
              ],
            },
            'VCLinkerTool': {
              'OptimizeReferences': 2,             # /OPT:REF
              'EnableCOMDATFolding': 2,            # /OPT:ICF
              'LinkIncremental': 1,                # disable incremental linking
              'AdditionalOptions': [
                '/LTCG:INCREMENTAL',               # incremental link-time code generation
              ],
            }
          }
        }, {
          'msvs_settings': {
            'VCCLCompilerTool': {
              'WholeProgramOptimization': 'false'
            },
            'VCLinkerTool': {
              'LinkIncremental': 2                 # enable incremental linking
            },
          },
         }],
         ['node_use_node_snapshot=="true"', {
          'dependencies': [
            'node_mksnapshot',
          ],
          'conditions': [
            ['node_snapshot_main!=""', {
              'actions': [
                {
                  'action_name': 'node_mksnapshot',
                  'process_outputs_as_sources': 1,
                  'inputs': [
                    '<(node_mksnapshot_exec)',
                    '<(node_snapshot_main)',
                  ],
                  'outputs': [
                    '<(SHARED_INTERMEDIATE_DIR)/node_snapshot.cc',
                  ],
                  'action': [
                    '<(node_mksnapshot_exec)',
                    '--build-snapshot',
                    '<(node_snapshot_main)',
                    '<@(_outputs)',
                  ],
                },
              ],
            }, {
              'actions': [
                {
                  'action_name': 'node_mksnapshot',
                  'process_outputs_as_sources': 1,
                  'inputs': [
                    '<(node_mksnapshot_exec)',
                  ],
                  'outputs': [
                    '<(SHARED_INTERMEDIATE_DIR)/node_snapshot.cc',
                  ],
                  'action': [
                    '<@(_inputs)',
                    '<@(_outputs)',
                  ],
                },
              ],
            }],
          ],
          }, {
          'sources': [
            'src/node_snapshot_stub.cc'
          ],
        }],
        [ 'OS in "linux freebsd" and '
          'target_arch=="x64"', {
          'dependencies': [ 'node_text_start' ],
          'ldflags+': [
            '<(obj_dir)/<(node_text_start_object_path)'
          ]
        }],

        ['node_fipsinstall=="true"', {
          'variables': {
            'openssl-cli': '<(PRODUCT_DIR)/<(EXECUTABLE_PREFIX)openssl-cli<(EXECUTABLE_SUFFIX)',
            'provider_name': 'libopenssl-fipsmodule',
            'opensslconfig': './deps/openssl/nodejs-openssl.cnf',
            'conditions': [
              ['GENERATOR == "ninja"', {
	        'fipsmodule_internal': '<(PRODUCT_DIR)/lib/<(provider_name).so',
                'fipsmodule': '<(PRODUCT_DIR)/obj/lib/openssl-modules/fips.so',
                'fipsconfig': '<(PRODUCT_DIR)/obj/lib/fipsmodule.cnf',
                'opensslconfig_internal': '<(PRODUCT_DIR)/obj/lib/openssl.cnf',
             }, {
	        'fipsmodule_internal': '<(PRODUCT_DIR)/obj.target/deps/openssl/<(provider_name).so',
                'fipsmodule': '<(PRODUCT_DIR)/obj.target/deps/openssl/lib/openssl-modules/fips.so',
                'fipsconfig': '<(PRODUCT_DIR)/obj.target/deps/openssl/fipsmodule.cnf',
                'opensslconfig_internal': '<(PRODUCT_DIR)/obj.target/deps/openssl/openssl.cnf',
             }],
            ],
          },
          'actions': [
            {
              'action_name': 'fipsinstall',
              'process_outputs_as_sources': 1,
              'inputs': [
                '<(fipsmodule_internal)',
              ],
              'outputs': [
                '<(fipsconfig)',
              ],
              'action': [
                '<(openssl-cli)', 'fipsinstall',
                '-provider_name', '<(provider_name)',
                '-module', '<(fipsmodule_internal)',
                '-out', '<(fipsconfig)',
                #'-quiet',
              ],
            },
            {
              'action_name': 'copy_fips_module',
              'inputs': [
                '<(fipsmodule_internal)',
              ],
              'outputs': [
                '<(fipsmodule)',
              ],
              'action': [
                'python', 'tools/copyfile.py',
                '<(fipsmodule_internal)',
                '<(fipsmodule)',
              ],
            },
            {
              'action_name': 'copy_openssl_cnf_and_include_fips_cnf',
              'inputs': [ '<(opensslconfig)', ],
              'outputs': [ '<(opensslconfig_internal)', ],
              'action': [
                'python', 'tools/enable_fips_include.py',
                '<(opensslconfig)',
                '<(opensslconfig_internal)',
                '<(fipsconfig)',
              ],
            },
          ],
         }, {
           'variables': {
              'opensslconfig_internal': '<(obj_dir)/deps/openssl/openssl.cnf',
              'opensslconfig': './deps/openssl/nodejs-openssl.cnf',
           },
           'actions': [
             {
               'action_name': 'reset_openssl_cnf',
               'inputs': [ '<(opensslconfig)', ],
               'outputs': [ '<(opensslconfig_internal)', ],
               'action': [
                 '<(python)', 'tools/copyfile.py',
                 '<(opensslconfig)',
                 '<(opensslconfig_internal)',
               ],
             },
           ],
         }],
      ],
    }, # node_core_target_name
    {
      'target_name': '<(node_lib_target_name)',
      'type': '<(node_intermediate_lib_type)',
      'dependencies': [
        'deps/llhttp/llhttp.gyp:llhttp',
      ],
      'conditions': [
        [ 'use_node_zlib=="true"' , {
          'dependencies': [ 'deps/zlib/zlib.gyp:zlib' ],
        } ],
        [ 'use_node_zlib=="false"' , {
            'include_dirs': [
              '../../third_party/zlib/include',
              '../../third_party/zlib',
            ],
            'ldflags' : [
              '<(NDK_SYS_ROOT)/obj/third_party/zlib/libz.a',
            ]
        } ],
        [ 'enable_hilog=="true"' , {
            'ldflags' : [
              '<(NDK_SYS_ROOT)/hiviewdfx/hilog/libhilog.so',
            ],
            'defines': [
              'ENABLE_HILOG',
            ]
        } ],
      ],
      'sources': [
        'jsvm/src/platform/platform_ohos.cpp',
        'jsvm/src/inspector/js_native_api_v8_inspector.cpp',
        'jsvm/src/inspector/inspector_socket_server.cpp',
        'jsvm/src/inspector/inspector_socket.cpp',
        'jsvm/src/inspector/inspector_utils.cpp',
        'jsvm/src/js_native_api_v8.cpp',
        'jsvm/src/jsvm_env.cpp',
        'jsvm/src/jsvm_reference.cpp',
        # Dependency headers
        '<(deps)/v8/include/v8.h',
        'deps/postject/postject-api.h'
      ],


      'cflags': [
        '-fstack-protector-strong',
        '--target=aarch64-linux-ohos',
        '-march=armv8-a',
        '-mfpu=neon',
        '-m64',
        '-msign-return-address=all',
        '-mbranch-protection=pac-ret+b-key+bti',
        '-pthread',
        '-Wall',
        '-Wextra',
        '-Wno-unused-parameter',
        '-fPIC',
        '-Werror=unused-result',
        '-O3',
        '-fno-omit-frame-pointer',
        '-fno-rtti',
        '-fno-exceptions',
        '-std=gnu++17',
        '-fvisibility=hidden'
      ],

      'defines': [
        '_GLIBCXX_USE_CXX11_ABI=1',
        '__STDC_FORMAT_MACROS',
        '__POSIX__',
        'TARGET_OHOS',
        'ENABLE_HISYSEVENT',
        'ENABLE_INSPECTOR',
        'HAVE_OPENSSL=1'
      ],
      'include_dirs': [
        'jsvm/interface/innerkits',
        'jsvm/interface/kits',
        'jsvm/src',
        '../../base/hiviewdfx/hisysevent/interfaces/native/innerkits/hisysevent/include',
        '../../base/hiviewdfx/hitrace/interfaces/native/innerkits/include/hitrace_meter',
        '../../base/hiviewdfx/hilog/interfaces/native/innerkits/include',
        '../../base/startup/init/interfaces/innerkits/include/param',
        '../../third_party/libuv/include',
        '../../third_party/icu/icu4c/source/common',
        '<(NDK_SYS_ROOT)/obj/third_party/openssl/build_all_generated/linux-aarch64/include',
        '../openssl/include',
        '<(obj_dir)/../../../v8-include'
      ],
      'ldflags' : [
        '-fvisibility=hidden',
        '<(NDK_SYS_ROOT)/resourceschedule/resource_schedule_service/libressched_client.z.so',
        '<(NDK_SYS_ROOT)/startup/init/libbegetutil.z.so',
        '<(NDK_SYS_ROOT)/hiviewdfx/hitrace/libhitrace_meter.so',
        '<(NDK_SYS_ROOT)/thirdparty/libuv/libuv.so',
        '<(NDK_SYS_ROOT)/thirdparty/icu/libhmicuuc.z.so',
        '<(NDK_SYS_ROOT)/thirdparty/icu/libhmicui18n.z.so',
        '<(NDK_SYS_ROOT)/thirdparty/openssl/libcrypto_openssl.z.so',
        '<(NDK_SYS_ROOT)/thirdparty/openssl/libssl_openssl.z.so',
        '<(obj_dir)/../../../libv8_shared.so',
      ],
    }, # node_lib_target_name
    {
       # generate ETW header and resource files
      'target_name': 'node_etw',
      'type': 'none',
      'conditions': [
        [ 'node_use_etw=="true"', {
          'actions': [
            {
              'action_name': 'node_etw',
              'inputs': [ 'src/res/node_etw_provider.man' ],
              'outputs': [
                'tools/msvs/genfiles/node_etw_provider.rc',
                'tools/msvs/genfiles/node_etw_provider.h',
                'tools/msvs/genfiles/node_etw_providerTEMP.BIN',
              ],
              'action': [ 'mc <@(_inputs) -h tools/msvs/genfiles -r tools/msvs/genfiles' ]
            }
          ]
        } ]
      ]
    }, # node_etw
    {
      'target_name': 'node_dtrace_header',
      'type': 'none',
      'conditions': [
        [ 'node_use_dtrace=="true" and OS!="linux"', {
          'actions': [
            {
              'action_name': 'node_dtrace_header',
              'inputs': [ 'src/node_provider.d' ],
              'outputs': [ '<(SHARED_INTERMEDIATE_DIR)/node_provider.h' ],
              'action': [ 'dtrace', '-h', '-xnolibs', '-s', '<@(_inputs)',
                '-o', '<@(_outputs)' ]
            }
          ]
        } ],
        [ 'node_use_dtrace=="true" and OS=="linux"', {
          'actions': [
            {
              'action_name': 'node_dtrace_header',
              'inputs': [ 'src/node_provider.d' ],
              'outputs': [ '<(SHARED_INTERMEDIATE_DIR)/node_provider.h' ],
              'action': [ 'dtrace', '-h', '-s', '<@(_inputs)',
                '-o', '<@(_outputs)' ]
            }
          ]
        } ],
      ]
    }, # node_dtrace_header
    {
      'target_name': 'node_dtrace_provider',
      'type': 'none',
      'conditions': [
        [ 'node_use_dtrace=="true" and OS!="mac" and OS!="linux"', {
          'actions': [
            {
              'action_name': 'node_dtrace_provider_o',
              'inputs': [
                '<(obj_dir)/<(node_lib_target_name)/src/node_dtrace.o',
              ],
              'outputs': [
                '<(obj_dir)/<(node_lib_target_name)/src/node_dtrace_provider.o'
              ],
              'action': [ 'dtrace', '-G', '-xnolibs', '-s', 'src/node_provider.d',
                '<@(_inputs)', '-o', '<@(_outputs)' ]
            }
          ]
        }],
        [ 'node_use_dtrace=="true" and OS=="linux"', {
          'actions': [
            {
              'action_name': 'node_dtrace_provider_o',
              'inputs': [ 'src/node_provider.d' ],
              'outputs': [
                '<(SHARED_INTERMEDIATE_DIR)/node_dtrace_provider.o'
              ],
              'action': [
                'dtrace', '-C', '-G', '-s', '<@(_inputs)', '-o', '<@(_outputs)'
              ],
            }
          ],
        }],
      ]
    }, # node_dtrace_provider
    {
      'target_name': 'node_dtrace_ustack',
      'type': 'none',
      'conditions': [
        [ 'node_use_dtrace=="true" and OS!="mac" and OS!="linux"', {
          'actions': [
            {
              'action_name': 'node_dtrace_ustack_constants',
              'inputs': [
                '<(obj_dir)/tools/v8_gypfiles/<(STATIC_LIB_PREFIX)v8_base_without_compiler<(STATIC_LIB_SUFFIX)'
              ],
              'outputs': [
                '<(SHARED_INTERMEDIATE_DIR)/v8constants.h'
              ],
              'action': [
                'tools/genv8constants.py',
                '<@(_outputs)',
                '<@(_inputs)'
              ]
            },
            {
              'action_name': 'node_dtrace_ustack',
              'inputs': [
                'src/v8ustack.d',
                '<(SHARED_INTERMEDIATE_DIR)/v8constants.h'
              ],
              'outputs': [
                '<(obj_dir)/<(node_lib_target_name)/src/node_dtrace_ustack.o'
              ],
              'conditions': [
                [ 'target_arch=="ia32" or target_arch=="arm"', {
                  'action': [
                    'dtrace', '-32', '-I<(SHARED_INTERMEDIATE_DIR)', '-Isrc',
                    '-C', '-G', '-s', 'src/v8ustack.d', '-o', '<@(_outputs)',
                  ]
                } ],
                [ 'target_arch=="x64"', {
                  'action': [
                    'dtrace', '-64', '-I<(SHARED_INTERMEDIATE_DIR)', '-Isrc',
                    '-C', '-G', '-s', 'src/v8ustack.d', '-o', '<@(_outputs)',
                  ]
                } ],
              ]
            },
          ]
        } ],
      ]
    }, # node_dtrace_ustack
    {
      'target_name': 'specialize_node_d',
      'type': 'none',
      'conditions': [
        [ 'node_use_dtrace=="true"', {
          'actions': [
            {
              'action_name': 'specialize_node_d',
              'inputs': [
                'src/node.d'
              ],
              'outputs': [
                '<(PRODUCT_DIR)/node.d',
              ],
              'action': [
                'tools/specialize_node_d.py',
                '<@(_outputs)',
                '<@(_inputs)',
                '<@(OS)',
                '<@(target_arch)',
              ],
            },
          ],
        } ],
      ]
    }, # specialize_node_d
    { # fuzz_url
      'target_name': 'fuzz_url',
      'type': 'executable',
      'dependencies': [
        '<(node_lib_target_name)',
      ],
      'includes': [
        'node.gypi'
      ],
      'include_dirs': [
        'src',
      ],
      'defines': [
        'NODE_ARCH="<(target_arch)"',
        'NODE_PLATFORM="<(OS)"',
        'NODE_WANT_INTERNALS=1',
      ],
      'sources': [
        'src/node_snapshot_stub.cc',
        'test/fuzzers/fuzz_url.cc',
      ],
      'conditions': [
        ['OS=="linux"', {
          'ldflags': [ '-fsanitize=fuzzer' ]
        }],
        # Ensure that ossfuzz flag has been set and that we are on Linux
        [ 'OS!="linux" or ossfuzz!="true"', {
          'type': 'none',
        }],
      ],
    }, # fuzz_url
    { # fuzz_env
      'target_name': 'fuzz_env',
      'type': 'executable',
      'dependencies': [
        '<(node_lib_target_name)',
        'deps/histogram/histogram.gyp:histogram',
        'deps/uvwasi/uvwasi.gyp:uvwasi',
        'node_dtrace_header',
        'node_dtrace_ustack',
        'node_dtrace_provider',
      ],
      'includes': [
        'node.gypi'
      ],
      'include_dirs': [
        'src',
        'tools/msvs/genfiles',
        '<(deps)/v8/include',
        'deps/cares/include',
        'deps/uv/include',
        'deps/uvwasi/include',
        'test/cctest',
      ],
      'defines': [
        'NODE_ARCH="<(target_arch)"',
        'NODE_PLATFORM="<(OS)"',
        'NODE_WANT_INTERNALS=1',
      ],
      'sources': [
        'src/node_snapshot_stub.cc',
        'test/fuzzers/fuzz_env.cc',
      ],
      'conditions': [
        ['OS=="linux"', {
          'ldflags': [ '-fsanitize=fuzzer' ]
        }],
        # Ensure that ossfuzz flag has been set and that we are on Linux
        [ 'OS!="linux" or ossfuzz!="true"', {
          'type': 'none',
        }],
        # Avoid excessive LTO
        ['enable_lto=="true"', {
          'ldflags': [ '-fno-lto' ],
        }],
      ],
    }, # fuzz_env
    #{
    #  'target_name': 'cctest',
    #  'type': 'executable',

    #  'dependencies': [
    #    '<(node_lib_target_name)',
    #    'deps/base64/base64.gyp:base64',
    #    'deps/googletest/googletest.gyp:gtest',
    #    'deps/googletest/googletest.gyp:gtest_main',
    #    'deps/histogram/histogram.gyp:histogram',
    #    'deps/uvwasi/uvwasi.gyp:uvwasi',
    #    'node_dtrace_header',
    #    'node_dtrace_ustack',
    #    'node_dtrace_provider',
    #    'deps/simdutf/simdutf.gyp:simdutf',
    #    'deps/ada/ada.gyp:ada',
    #  ],

    #  'includes': [
    #    'node.gypi'
    #  ],

    #  'include_dirs': [
    #    'src',
    #    'tools/msvs/genfiles',
    #    '<(deps)/v8/include',
    #    'deps/cares/include',
    #    'deps/uv/include',
    #    'deps/uvwasi/include',
    #    'test/cctest',
    #  ],

    #  'defines': [
    #    'NODE_ARCH="<(target_arch)"',
    #    'NODE_PLATFORM="<(OS)"',
    #    'NODE_WANT_INTERNALS=1',
    #  ],

    #  'sources': [
    #    'src/node_snapshot_stub.cc',
    #    'test/cctest/node_test_fixture.cc',
    #    'test/cctest/node_test_fixture.h',
    #    'test/cctest/test_aliased_buffer.cc',
    #    'test/cctest/test_base64.cc',
    #    'test/cctest/test_base_object_ptr.cc',
    #    'test/cctest/test_node_postmortem_metadata.cc',
    #    'test/cctest/test_environment.cc',
    #    'test/cctest/test_linked_binding.cc',
    #    'test/cctest/test_node_api.cc',
    #    'test/cctest/test_per_process.cc',
    #    'test/cctest/test_platform.cc',
    #    'test/cctest/test_report.cc',
    #    'test/cctest/test_json_utils.cc',
    #    'test/cctest/test_sockaddr.cc',
    #    'test/cctest/test_traced_value.cc',
    #    'test/cctest/test_util.cc',
    #  ],

    #  'conditions': [
    #    [ 'node_use_openssl=="true"', {
    #      'defines': [
    #        'HAVE_OPENSSL=1',
    #      ],
    #      'sources': [
    #        'test/cctest/test_crypto_clienthello.cc',
    #        'test/cctest/test_node_crypto.cc',
    #        'test/cctest/test_node_crypto_env.cc',
    #      ]
    #    }],
    #    ['v8_enable_inspector==1', {
    #      'sources': [
    #        'test/cctest/test_inspector_socket.cc',
    #        'test/cctest/test_inspector_socket_server.cc'
    #      ],
    #      'defines': [
    #        'HAVE_INSPECTOR=1',
    #      ],
    #    }, {
    #       'defines': [
    #         'HAVE_INSPECTOR=0',
    #       ]
    #    }],
    #    ['OS=="solaris"', {
    #      'ldflags': [ '-I<(SHARED_INTERMEDIATE_DIR)' ]
    #    }],
        # Skip cctest while building shared lib node for Windows
    #    [ 'OS=="win" and node_shared=="true"', {
    #      'type': 'none',
    #    }],
    #    [ 'node_shared=="true"', {
    #      #'xcode_settings': {
    #        'OTHER_LDFLAGS': [ '-Wl,-rpath,@loader_path', ],
    #      },
    #    }],
    #    ['OS=="win"', {
    #      'libraries': [
    #        'Dbghelp.lib',
    #        'winmm.lib',
    #        'Ws2_32.lib',
    #      ],
    #    }],
        # Avoid excessive LTO
    #    ['enable_lto=="true"', {
    #      'ldflags': [ '-fno-lto' ],
    #    }],
    #  ],
    #}, # cctest

    #{
    #  'target_name': 'embedtest',
    #  'type': 'executable',

    #  'dependencies': [
    #    '<(node_lib_target_name)',
    #    'deps/histogram/histogram.gyp:histogram',
    #    'deps/uvwasi/uvwasi.gyp:uvwasi',
    #    'node_dtrace_header',
    #    'node_dtrace_ustack',
    #    'node_dtrace_provider',
    #    'deps/ada/ada.gyp:ada',
    #  ],

    #  'includes': [
    #    'node.gypi'
    #  ],

    #  'include_dirs': [
    #    'src',
    #    'tools/msvs/genfiles',
    #    '<(deps)/v8/include',
    #    'deps/cares/include',
    #    'deps/uv/include',
    #    'deps/uvwasi/include',
    #    'test/embedding',
    #  ],

    #  'sources': [
    #    'src/node_snapshot_stub.cc',
    #    'test/embedding/embedtest.cc',
    #  ],

    #  'conditions': [
    #    ['OS=="solaris"', {
    #      'ldflags': [ '-I<(SHARED_INTERMEDIATE_DIR)' ]
    #    }],
        # Skip cctest while building shared lib node for Windows
    #    [ 'OS=="win" and node_shared=="true"', {
    #      'type': 'none',
    #    }],
    #    [ 'node_shared=="true"', {
    #      'xcode_settings': {
    #        'OTHER_LDFLAGS': [ '-Wl,-rpath,@loader_path', ],
    #      },
    #    }],
    #    ['OS=="win"', {
    #      'libraries': [
    #        'Dbghelp.lib',
    #        'winmm.lib',
    #        'Ws2_32.lib',
    #      ],
    #    }],
        # Avoid excessive LTO
    #    ['enable_lto=="true"', {
    #      'ldflags': [ '-fno-lto' ],
    #    }],
    #  ],
    #}, # embedtest

    {
      'target_name': 'overlapped-checker',
      'type': 'executable',

      'conditions': [
        ['OS=="win"', {
          'sources': [
            'test/overlapped-checker/main_win.c'
          ],
        }],
        ['OS!="win"', {
          'sources': [
            'test/overlapped-checker/main_unix.c'
          ],
        }],
        # Avoid excessive LTO
        ['enable_lto=="true"', {
          'ldflags': [ '-fno-lto' ],
        }],
      ]
    }, # overlapped-checker
    {
      'target_name': 'node_mksnapshot',
      'type': 'executable',

      'dependencies': [
        '<(node_lib_target_name)',
        'deps/histogram/histogram.gyp:histogram',
        'deps/uvwasi/uvwasi.gyp:uvwasi',
        'deps/ada/ada.gyp:ada',
      ],

      'includes': [
        'node.gypi'
      ],

      'include_dirs': [
        'src',
        'tools/msvs/genfiles',
        '<(deps)/v8/include',
        'deps/cares/include',
        'deps/uv/include',
        'deps/uvwasi/include',
        '../../base/startup/init/interfaces/innerkits/include/param',
      ],

      'defines': [ 'NODE_WANT_INTERNALS=1' ],
   
      'ldflags' : [
        '<(NDK_SYS_ROOT)/resourceschedule/resource_schedule_service/libressched_client.z.so',
        '<(NDK_SYS_ROOT)/startup/init/libbegetutil.z.so'
      ],

      'sources': [
        'src/node_snapshot_stub.cc',
        'tools/snapshot/node_mksnapshot.cc',
      ],

      'conditions': [
        [ 'node_use_openssl=="true"', {
          'defines': [
            'HAVE_OPENSSL=1',
          ],
        }],
        [ 'node_use_node_code_cache=="true"', {
          'defines': [
            'NODE_USE_NODE_CODE_CACHE=1',
          ],
        }],
        ['v8_enable_inspector==1', {
          'defines': [
            'HAVE_INSPECTOR=1',
          ],
        }],
        ['OS=="win"', {
          'libraries': [
            'Dbghelp.lib',
            'winmm.lib',
            'Ws2_32.lib',
          ],
        }],
        # Avoid excessive LTO
        ['enable_lto=="true"', {
          'ldflags': [ '-fno-lto' ],
        }],
      ],
    }, # node_mksnapshot
  ], # end targets

  'conditions': [
    ['OS in "aix os400" and node_shared=="true"', {
      'targets': [
        {
          'target_name': 'node_aix_shared',
          'type': 'shared_library',
          'product_name': '<(node_core_target_name)',
          'ldflags': ['--shared'],
          'product_extension': '<(shlib_suffix)',
          'includes': [
            'node.gypi'
          ],
          'dependencies': ['<(node_lib_target_name)'],
          'include_dirs': [
            'src',
            '<(deps)/v8/include',
          ],
          'sources': [
            '<@(library_files)',
            '<@(deps_files)',
            'common.gypi',
          ],
          'direct_dependent_settings': {
            'ldflags': [ '-Wl,-brtl' ],
          },
        },
      ]
    }], # end aix section
    ['OS=="win" and node_shared=="true"', {
     'targets': [
       {
         'target_name': 'gen_node_def',
         'type': 'executable',
         'sources': [
           'tools/gen_node_def.cc'
         ],
       },
       {
         'target_name': 'generate_node_def',
         'dependencies': [
           'gen_node_def',
           '<(node_lib_target_name)',
         ],
         'type': 'none',
         'actions': [
           {
             'action_name': 'generate_node_def_action',
             'inputs': [
               '<(PRODUCT_DIR)/<(node_lib_target_name).dll'
             ],
             'outputs': [
               '<(PRODUCT_DIR)/<(node_core_target_name).def',
             ],
             'action': [
               '<(PRODUCT_DIR)/gen_node_def.exe',
               '<@(_inputs)',
               '<@(_outputs)',
             ],
           },
         ],
       },
     ],
   }], # end win section
  ], # end conditions block
}
