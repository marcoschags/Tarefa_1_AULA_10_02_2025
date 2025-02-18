# Arquivo CMake gerado para o projeto Pico

# Versão mínima do CMake necessária
cmake_minimum_required(VERSION 3.13)

# Configuração dos padrões de linguagem C e C++
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Inicialização do SDK do Raspberry Pi Pico a partir da localização instalada
# (pode vir de variáveis de ambiente, cache do CMake, etc.)

# == NÃO EDITE AS LINHAS ABAIXO para que a extensão do VS Code para Raspberry Pi Pico funcione ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})  # Define o diretório home do usuário no Windows
else()
    set(USERHOME $ENV{HOME})         # Define o diretório home do usuário em outros sistemas
endif()
set(sdkVersion 2.1.0)                # Versão do SDK
set(toolchainVersion 13_3_Rel1)      # Versão da toolchain
set(picotoolVersion 2.1.0)           # Versão do picotool
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)  # Caminho para o script do VS Code
if (EXISTS ${picoVscode})
    include(${picoVscode})           # Inclui o script do VS Code, se existir
endif()
# ====================================================================================

# Define o tipo de placa (pico_w neste caso)
set(PICO_BOARD pico_w CACHE STRING "Tipo de placa")

# Inclui o script de importação do SDK do Raspberry Pi Pico (deve estar antes do projeto)
include(pico_sdk_import.cmake)

# Define o nome do projeto e as linguagens suportadas (C, C++ e ASM)
project(Tarefa_1_AULA_10_02_2025 C CXX ASM)

# Inicializa o SDK do Raspberry Pi Pico
pico_sdk_init()

# Adiciona o executável. O nome padrão é o nome do projeto, versão 0.1
add_executable(Tarefa_1_AULA_10_02_2025 
                Tarefa_1_AULA_10_02_2025.c
                lib/ssd1306.c  # Adicione esta linha
              )

# Define o nome e a versão do programa
pico_set_program_name(Tarefa_1_AULA_10_02_2025 "Tarefa_1_AULA_10_02_2025")
pico_set_program_version(Tarefa_1_AULA_10_02_2025 "0.1")

# Habilita/desabilita a saída via UART/USB
pico_enable_stdio_uart(Tarefa_1_AULA_10_02_2025 0)  # Desabilita UART
pico_enable_stdio_usb(Tarefa_1_AULA_10_02_2025 1)   # Habilita USB

# Adiciona as bibliotecas padrão e de hardware ao projeto
target_link_libraries(Tarefa_1_AULA_10_02_2025
    pico_stdlib      # Biblioteca padrão do Pico
    hardware_pwm     # Biblioteca para controle de PWM
    hardware_adc     # Biblioteca para controle de ADC
    hardware_i2c     # Biblioteca para controle de I2C
)

# Adiciona os diretórios de inclusão padrão ao projeto
target_include_directories(Tarefa_1_AULA_10_02_2025 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}  # Diretório atual do projeto
    ${PICO_SDK_PATH}/src/rp2_common/hardware_pwm/include  # Inclusão do PWM
    ${PICO_SDK_PATH}/src/rp2_common/hardware_adc/include  # Inclusão do ADC
    ${PICO_SDK_PATH}/src/rp2_common/hardware_i2c/include  # Inclusão do I2C
)

# Adiciona bibliotecas adicionais, se necessário
target_link_libraries(Tarefa_1_AULA_10_02_2025 
    # Adicione bibliotecas adicionais aqui, se necessário
)

# Gera arquivos de saída adicionais (por exemplo, .uf2, .hex)
pico_add_extra_outputs(Tarefa_1_AULA_10_02_2025)