cmake_minimum_required(VERSION 3.10)
project(TrafficLightSimulation C)

enable_testing()

add_executable(main main.c)

add_test(NAME RunSimulation
         COMMAND main ../tests/test_input.txt ../tests/test_output.txt)

add_test(NAME VerifyOutput
         COMMAND ${CMAKE_COMMAND} -E compare_files ../tests/test_output.txt ../tests/expected_output.txt)
