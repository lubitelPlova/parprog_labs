# Process each picture
```
mazur-da@H624:~/dev/parprog_labs/lb3/cppcuda/build$ ./fusion mountain.jpg
Original size: 5464x3640

Timings:
        image read:        220802 microseconds
        resize half:       1800 microseconds
        resize quarter:    1201 microseconds
        grayscale full:    1260 microseconds
        grayscale half:    318 microseconds
        grayscale quarter: 85 microseconds
        fuse grayscale:    1175 microseconds
        save image:        248506 microseconds
        -----------------------------------------
        total time:        545574 microseconds
mazur-da@H624:~/dev/parprog_labs/lb3/cppcuda/build$ ^C
mazur-da@H624:~/dev/parprog_labs/lb3/cppcuda/build$ ./fusion cat.jpg 
Original size: 1024x768

Timings:
        image read:        5370 microseconds
        resize half:       252 microseconds
        resize quarter:    20 microseconds
        grayscale full:    70 microseconds
        grayscale half:    19 microseconds
        grayscale quarter: 10 microseconds
        fuse grayscale:    58 microseconds
        save image:        8312 microseconds
        -----------------------------------------
        total time:        66233 microseconds
mazur-da@H624:~/dev/parprog_labs/lb3/cppcuda/build$ ./fusion car.jpg 
Original size: 1280x960

Timings:
        image read:        8195 microseconds
        resize half:       757 microseconds
        resize quarter:    29 microseconds
        grayscale full:    99 microseconds
        grayscale half:    26 microseconds
        grayscale quarter: 11 microseconds
        fuse grayscale:    83 microseconds
        save image:        16262 microseconds
        -----------------------------------------
        total time:        76560 microseconds
mazur-da@H624:~/dev/parprog_labs/lb3/cppcuda/build$ ./fusion car2.jpg 
Original size: 2048x1536

Timings:
        image read:        18119 microseconds
        resize half:       320 microseconds
        resize quarter:    55 microseconds
        grayscale full:    217 microseconds
        grayscale half:    58 microseconds
        grayscale quarter: 20 microseconds
        fuse grayscale:    190 microseconds
        save image:        42562 microseconds
        -----------------------------------------
        total time:        115181 microseconds
mazur-da@H624:~/dev/parprog_labs/lb3/cppcuda/build$ ./fusion nat
natr.jpeg   nature.jpg  
mazur-da@H624:~/dev/parprog_labs/lb3/cppcuda/build$ ./fusion nat
natr.jpeg   nature.jpg  
mazur-da@H624:~/dev/parprog_labs/lb3/cppcuda/build$ ./fusion natr.jpeg 
Original size: 1920x1080

Timings:
        image read:        20551 microseconds
        resize half:       255 microseconds
        resize quarter:    41 microseconds
        grayscale full:    153 microseconds
        grayscale half:    39 microseconds
        grayscale quarter: 15 microseconds
        fuse grayscale:    131 microseconds
        save image:        25663 microseconds
        -----------------------------------------
        total time:        99308 microseconds
mazur-da@H624:~/dev/parprog_labs/lb3/cppcuda/build$ ./fusion nature.jpg 
Original size: 1920x1200

Timings:
        image read:        16193 microseconds
        resize half:       286 microseconds
        resize quarter:    45 microseconds
        grayscale full:    170 microseconds
        grayscale half:    45 microseconds
        grayscale quarter: 17 microseconds
        fuse grayscale:    143 microseconds
        save image:        29012 microseconds
        -----------------------------------------
        total time:        98289 microseconds
mazur-da@H624:~/dev/parprog_labs/lb3/cppcuda/build$ ./fusion zamok.jpg 
Original size: 1024x640

Timings:
        image read:        8178 microseconds
        resize half:       174 microseconds
        resize quarter:    18 microseconds
        grayscale full:    61 microseconds
        grayscale half:    17 microseconds
        grayscale quarter: 9 microseconds
        fuse grayscale:    48 microseconds
        save image:        8497 microseconds
        -----------------------------------------
        total time:        72238 microseconds
```

# Process 1 pic three times

```
mazur-da@H624:~/dev/parprog_labs/lb3/cppcuda/build$ ./fusion mountain.jpg
Original size: 5464x3640

Timings:
        image read:        220857 microseconds
        resize half:       1575 microseconds
        resize quarter:    358 microseconds
        grayscale full:    1277 microseconds
        grayscale half:    317 microseconds
        grayscale quarter: 85 microseconds
        fuse grayscale:    1170 microseconds
        save image:        234202 microseconds
        -----------------------------------------
        total time:        530143 microseconds
mazur-da@H624:~/dev/parprog_labs/lb3/cppcuda/build$ ./fusion mountain.jpg
Original size: 5464x3640

Timings:
        image read:        222377 microseconds
        resize half:       1499 microseconds
        resize quarter:    354 microseconds
        grayscale full:    1263 microseconds
        grayscale half:    546 microseconds
        grayscale quarter: 239 microseconds
        fuse grayscale:    1296 microseconds
        save image:        249874 microseconds
        -----------------------------------------
        total time:        549440 microseconds
mazur-da@H624:~/dev/parprog_labs/lb3/cppcuda/build$ ./fusion mountain.jpg
Original size: 5464x3640

Timings:
        image read:        220802 microseconds
        resize half:       1800 microseconds
        resize quarter:    1201 microseconds
        grayscale full:    1260 microseconds
        grayscale half:    318 microseconds
        grayscale quarter: 85 microseconds
        fuse grayscale:    1175 microseconds
        save image:        248506 microseconds
        -----------------------------------------
        total time:        545574 microseconds
```