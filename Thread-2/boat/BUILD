##load("@rules_cc//cc:defs.bzl", "cc_test")  #load the test target

cc_library(
    name = "boatGrader_lib",
    srcs = [
        "boatGrader.cc",
        ],
    hdrs = [
        "boatGrader.h",
        ],
	visibility = [
		"//visibility:public",
	],
)

cc_library(
    name = "boat_lib",
    srcs = [
        "boat.cc",
        ],
    hdrs = [
        "boat.h",
        ],
	deps = [
        ":boatGrader_lib"
    ],
	visibility = [
		"//visibility:public",
	],
)

cc_binary(
    name = "main",
    srcs = [
        "main.cc"
            ],
	deps = [
        ":boatGrader_lib",
		":boat_lib"
    ],
	copts = [
        "-O3",
        "-std=c++11",
    ],
	linkopts = [
        "-pthread",
    ],
)

cc_test(
    name = "boat_test",
    size = "large",
    srcs = ["boat_test.cc"],
    deps = [
        "@gtest//:gtest_main",
        ":boat_lib",
        ":boatGrader_lib",
    ],
)
