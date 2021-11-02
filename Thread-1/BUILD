##load("@rules_cc//cc:defs.bzl", "cc_test")  #load the test target

cc_library(
    name = "single_instruction_lib",
    srcs = [
        "single_instruction.cc",
        ],
    hdrs = [
        "single_instruction.h",
        ],
    deps = [
        "//lib:embedding_lib",
        "//lib:instruction_lib",
        "//lib:utils_lib",
        "//lib:model_lib"
    ],
    copts = [
      "-std=c++17",
    ],
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "all_instructions_lib",
    srcs = [
        "all_instructions.cc",
        ],
    hdrs = [
        "all_instructions.h",
        ],
    deps = [
        "//lib:embedding_lib",
        "//lib:instruction_lib",
        "//lib:utils_lib",
        "//lib:model_lib",
        "//:single_instruction_lib"
    ],
    copts = [
      "-std=c++17",
    ],
    visibility = [
        "//visibility:public",
    ],
)

cc_binary(
    name = "q0",
    srcs = [
        "q0.cc"
            ],
    deps = [
        "//lib:embedding_lib",
        "//lib:instruction_lib",
        "//lib:utils_lib",
        "//lib:model_lib"
    ],
    copts = [
        "-std=c++17",
    ],
    data = glob(["data/q0*"]),
)

cc_binary(
    name = "format",
    srcs = [
        "format.cc"
            ],
)

cc_binary(
    name = "q1",
    srcs = [
        "q1.cc"
            ],
    deps = [
        "//lib:embedding_lib",
        "//lib:instruction_lib",
        "//lib:utils_lib",
        "//lib:model_lib",
        "//:single_instruction_lib",
        "//:all_instructions_lib"
    ],
    copts = [
        "-std=c++17",
    ],
    linkopts = ["-lpthread"],
    data = glob(["data/q1*"]),
)

cc_binary(
    name = "q2",
    srcs = [
        "q2.cc"
            ],
    deps = [
        "//lib:embedding_lib",
        "//lib:instruction_lib",
        "//lib:utils_lib",
        "//lib:model_lib",
        "//:single_instruction_lib",
        "//:all_instructions_lib"
    ],
    copts = [
        "-std=c++17",
    ],
    linkopts = ["-lpthread"],
    data = glob(["data/q2*"]),
)

cc_binary(
    name = "q3",
    srcs = [
        "q3.cc"
            ],
    deps = [
        "//lib:embedding_lib",
        "//lib:instruction_lib",
        "//lib:utils_lib",
        "//lib:model_lib",
        "//:single_instruction_lib",
        "//:all_instructions_lib"
    ],
    copts = [
        "-std=c++17",
    ],
    linkopts = ["-lpthread"],
    data = glob(["data/q3*"]),
)

cc_binary(
    name = "q4",
    srcs = [
        "q4.cc"
            ],
    deps = [
        "//lib:embedding_lib",
        "//lib:instruction_lib",
        "//lib:utils_lib",
        "//lib:model_lib",
        "//:single_instruction_lib",
        "//:all_instructions_lib"
    ],
    copts = [
        "-std=c++17",
    ],
    linkopts = ["-lpthread"],
    data = glob(["data/q4*"]),
)

cc_binary(
    name = "q5",
    srcs = [
        "q5.cc"
            ],
    deps = [
        "//lib:embedding_lib",
        "//lib:instruction_lib",
        "//lib:utils_lib",
        "//lib:model_lib",
        "//:single_instruction_lib",
        "//:all_instructions_lib"
    ],
    copts = [
        "-std=c++17",
    ],
    linkopts = ["-lpthread"],
    data = glob(["data/q4*"]),
)

cc_test(
  name = "benchmark",
  size = "small",
  srcs = ["benchmark.cc"],
  deps = [
      "@gbench//:benchmark",
      ],
  copts = [
        "-O3",
        "-std=c++11",
  ],
  data = glob(["data/*"]),
)

cc_test(
    name = "data_generator",
    size = "small",
    srcs = ["data_generator.cc"],
)

cc_test(
    name = "full_test",
    size = "large",
    srcs = ["full_test.cc"],
    deps = [
        "@gtest//:gtest_main",
        ":all_instructions_lib",
        ":single_instruction_lib",
        ],
)
