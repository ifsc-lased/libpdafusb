add_executable(app-exemplo aplicacao_exemplo.cc)
target_compile_features(app-exemplo PRIVATE cxx_std_11)

# Usar biblioteca wsock32 somente no Windows
if(WIN32)
    target_link_libraries(app-exemplo PRIVATE pdafusb wsock32 ws2_32)
else()
    target_link_libraries(app-exemplo PRIVATE pdafusb)
endif()