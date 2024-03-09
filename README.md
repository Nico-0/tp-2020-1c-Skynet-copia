# tp-2020-1c-Skynet
# Delibird

## Installation
- `make install` the [commons library](https://github.com/sisoputnfrba/so-commons-library.git).
- `make` utils first, and include them in library path with `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/utnso/tp-2020-1c-Skynet/utils/Debug`.
- `.config` files must be present at your current directory, independently of where the executables are.

## Usage with Docker
- `docker compose up`
- Run any of the available scripts in the `_pruebas` folder.
    - `docker exec gameboy sh -c "cd ./_pruebas && sh scriptname.sh"`

## readme todo
Encontrar documento de pruebas y usarlo de guia

Broker cache dump with SIGUSR1 signal