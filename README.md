# cropfit
For when you need to get the highest quality version of a cropped image.
The main reason I built this is for recovering old profile pictures when you only have a low quality version.

## Using

```
Usage: ./cropfit <template_image> <source_image> <output_image>
```

## Compiling
- Ensure you have OpenCV installed

`g++ -O2 -std=c++17 main.cpp -o sift_crop \`pkg-config --cflags --libs --static opencv4\``

## So how does it work?
TODO.
I have no idea. SIFT is a crazy algorithm
Too much complicated math D: