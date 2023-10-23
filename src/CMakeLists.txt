project(triliumquest)

find_package(cdt)

add_contract( triliumquest triliumquest triliumquest.cpp )
target_include_directories( triliumquest PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( triliumquest ${CMAKE_SOURCE_DIR}/../ricardian )