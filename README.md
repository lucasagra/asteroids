# Asteroids
## First development project: A game using Allegro 5 C library for Programming 101 course
- This project summarizes what I've learned about programming from zero to finishing my first semester

[img](https://github.com/lucasagra/asteroids/blob/master/img/home.png)
![gif](https://github.com/lucasagra/asteroids/blob/master/img/example.gif)

## Instructions

- Allegro 5.0.10 required

Tested on Windows 10

```bash
gcc -I C:\allegro-5.0.10-mingw-4.7.0\include -c asteroids.c -lm


gcc -o asteroids.exe asteroids.o C:\allegro-5.0.10-mingw-4.7.0\lib\liballegro-5.0.10-monolith-mt.a -lm
# or
gcc -o asteroids.exe asteroids.o ./allegro-5.0.10-monolith-mt.dll -lm
```

