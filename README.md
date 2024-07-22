# **[Проект plc](https://github.com/Fogotcheck/ProgramLogicController.git)**

## *Клонирование репозитория*

Клонировать репозиторий вы можете следующим образом:

```console
git clone "https://github.com/Fogotcheck/ProgramLogicController.git"
```
```console
git submodule update --init --recursive
```

## *Структура проекта*

```
├── AppMain                             // перенаправление основной функции
|
├── Cmake
|       ├── Toolchain                   // минимальный набор правил для сборки
|       └── Utils                       // дополнительные утилиты сборки
|
├── lib                                 // библиотеки для проектов
|       ├── ...
|       ├── CMakeLists.txt              // CMakeLists для сборки библиотек
|       └── StCube
|           ├── Core
|           ├── Drivers                 // CMSIS+HAL от stm32
|           ├── StCube.ioc              // файл для генерации STM32CubeMX
|           └── CMakeLists.txt          // основной файл для сборки hal
|
|
├── CMakeLists.txt                      // основной файл для сборки проекта
|
└── README.md                           // Вы находитесь тут

```

## *Сборка проекта*

### Настройка сборки проекта
Сборка проекта основана на [Cmake](https://cmake.org/). Минимальные требования для настройки сборки проекта - являются указание toolchain:

```console
CMAKE_TOOLCHAIN_FILE=./Cmake/Toolcain/Arm-none-eabi-toolchain.cmake
```
Таким образом настройка проекта для сборки arm-none-eabi будет выглядеть:

```console
cmake -DCMAKE_TOOLCHAIN_FILE=./Cmake/Toolcain/Arm-none-eabi-toolchain.cmake -B ./build
```

### Дополнительные опции настройки сборки

Опция для прошивки mcu при помощи [openocd](https://openocd.org/), по умолчанию OFF:

```console
OPENOCD_ENABLE=ON
```

Опция для генерации файлов настройки окружения [VScode](https://code.visualstudio.com/), по умолчанию OFF.

```console
ENVIRONMENT_VSCODE=ON
```

Таким образом настройка проекта с дополнительными опциями будет выглядеть:

```console
cmake -DOPENOCD_ENABLE=ON -DENVIRONMENT_VSCODE=ON -DCMAKE_TOOLCHAIN_FILE=./Cmake/Toolcain/Arm-none-eabi-toolchain.cmake -B ./build
```

### Сборка проекта

Сборка осуществляется командой:

```console
cmake --build ./build
```

## *Дополнительные ссылки*

* [Трэкер задач](https://github.com/Fogotcheck/ProgramLogicController/issues/new/choose)
