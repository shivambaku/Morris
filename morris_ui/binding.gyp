{
    "targets": [
        {
            "target_name": "addon",
            "sources": [
                "cpp_addons/src/addon.cc",
                "cpp_addons/src/example.cc",
                "cpp_addons/src/simulation.cc"
            ],
            'include_dirs': [
                '<(module_root_dir)/cpp_addons/inc',
            ],
            'conditions': [
                ['OS=="win"', {
                    'libraries': [
                        '<(module_root_dir)/cpp_addons/lib/morris.lib'
                    ]
                }, { # OS != "win",
                    'libraries': [
                        '<(module_root_dir)/cpp_addons/lib/libmorris.a'
                    ]
                }]
            ]
        }
    ]
}