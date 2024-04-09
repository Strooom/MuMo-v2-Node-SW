Import("env")

env.Append(
    # Remove stub implementations
    BUILD_UNFLAGS=[
        "-lnosys",
        "--specs=nosys.specs",
    ],

    # Use the semihosted version of the syscalls
    LINKFLAGS=[
        "--specs=rdimon.specs",
    ],
    LIBS=[
        "rdimon",
    ],
)

def extra_http_configuration(env, node):
    """
    `node.name` - a name of File System Node
    `node.get_path()` - a relative path
    `node.get_abspath()` - an absolute path
    """

    # do not modify node if file name does not contain "http"
    if "sht40" not in node.name:
        return node

    # now, we can override ANY SCons variables (CPPDEFINES, CCFLAGS, etc.,) for the specific file
    # pass SCons variables as extra keyword arguments to `env.Object()` function
    # p.s: run `pio run -t envdump` to see a list with SCons variables

    return env.Object(
        node,
        CPPDEFINES=env["CPPDEFINES"],
        CCFLAGS=env["CCFLAGS"] + ["-lgcov"] + ["--coverage"] + ["-fprofile-arcs"] + ["-ftest-coverage"],
    )

env.AddBuildMiddleware(extra_http_configuration)

env.Append(
  LINKFLAGS=[
      "-lgcov",
      "--coverage"
  ]
)