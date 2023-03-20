
file(GLOB_RECURSE HEADER_LIST "${PROJECT_SOURCE_DIR}/include/pdafusb/*.hh")
file(GLOB_RECURSE SRC_LIST "*.cc")

# Para criar a biblioteca. Será estática ou dinâmica com base nas configurações do usuário
add_library(pdafusb ${HEADER_LIST} ${SRC_LIST})

# Precisamos deste diretório e os usuários desta biblioteca também precisarão
target_include_directories(pdafusb PUBLIC ../include)

# Dependência somente dos headers do Asio
target_link_libraries(pdafusb asio)

# Todos os usuários desta biblioteca precisam ter no mínimo C++11
target_compile_features(pdafusb PUBLIC cxx_std_11)

# IDEs irão colocar os headers em uma pasta adequada
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})

# Para instalar a biblioteca
install(TARGETS pdafusb DESTINATION lib)
install(FILES ${HEADER_LIST} DESTINATION include/pdafusb)