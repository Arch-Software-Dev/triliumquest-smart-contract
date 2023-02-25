project(triliumquest)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( triliumquest triliumquest triliumquest.cpp )
target_include_directories( triliumquest PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( triliumquest ${CMAKE_SOURCE_DIR}/../ricardian )