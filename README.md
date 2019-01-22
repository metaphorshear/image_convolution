# Image Convolution
This was created for a parallel programming assignment at DSU.  The main code is written in C with OpenMP.  It uses [Sean T. Barrett's libraries for image manipulation.](https://github.com/nothings/stb).  The front-end is in Node, using node-ffi to bind to the C code.  [The Scientist and Engineer's Guide to Digital Signal Processing](https://www.dspguide.com/) was indispensable in understanding convolution, and in providing good kernel examples.

## Disclaimers
I was a complete novice at Node when I made this, and the time to learn it appropriately was very limited.  I followed [this tutorial](https://nodeaddons.com/getting-your-c-to-the-web-with-node-js/) and that's pretty much it.  Transferring images from the client to the server is very, very slow.<sup>*</sup>  I am not sure why, but any attempt to clear out the node-modules folder and reinstall with npm introduces bugs.  If you want to fix that for some reason, here's what I believe the dependencies are:

```
  "dependencies" :
    { "express" : "^4.14.0" ,
      "body-parser" : "^1.15.2" ,
      "ffi" : "^2.2.0" ,
      "ref" : "^1.3.3" ,
      "ref-struct" :"^1.1.0" ,
      "ref-array" : "^1.2.0" ,
      "pug" : "^2.0.0-beta6"
    }
```
## Usage

Anyway, it *should* work if you copy it as-is, `cd` to `web`, and run `node index.js`.  Select an image, mess with the settings, and click go.  For example, try entering this for the convolution kernel:

```
0 0 0
0 1 0
0 0 -1
```
and leave the divisor as `1`.  After you've picked an image, click "Go".  If everything is working, then the server should output something like this:

```
Divisor: 1
Kernel:
0.000000	0.000000	0.000000	
0.000000	1.000000	0.000000	
0.000000	0.000000	-1.000000	
Image width: 800, height: 600, components: 4
```

and after a few seconds/minutes, your original image will be replaced in the canvas by your convolved image.

* <sub><sup>IIRC, raw image data is transferred from client to server in JSON.  I tried to look into Streams, but it was a bit too much effort at the time.  Now I think looking into compression would also be a good idea, at least maybe gzip.</sup></sub>
