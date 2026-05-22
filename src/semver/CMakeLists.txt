cmake_minimum_required(VERSION 3.5...3.31)
project(semver)

add_library(semver STATIC
    semver.c
    semver.h
)

encoding_check(semver)
