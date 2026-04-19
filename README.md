# Short

Basic way to put commands into a file to run them

Example:

build:
```
gcc

// Comments are supported

main.c
helper.c

-o main
```

Then you can run:

`short build`

## Flags

You can add -h or --here to execute a file from where your current working directory instead of the files local directory
You can add -s or --silent to not print any of the short notifiers ("Running command..." and "Ended with status code...")

Only works on linux.
