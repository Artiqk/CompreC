# CompreC

# Dependencies 
libzip

## Installing libzip

```sh
sudo apt install -y libzip-dev
```

# How to compile

## Installing gcc and make
```sh
sudo apt install -y gcc make
```

## Creating the necessary directories
```sh
mkdir obj bin
```

## Compile the project
```
make
```

# How to use

## Creating a symlink
The binary will be placed in the bin/ folder. If you want to access it without having to type every time bin/main, you might want to create a symbolic link. (Make sure you're in the root folder of the projet when you're executing this command)
```sh
ln -s bin/main main
```

## Executing the program
Then you can execute the help option to see what's available to use.
```sh
./main --help
```