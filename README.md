# Hunter's Template Library
Hunter's Template Library - a colletion of template containers, not provided in the STL, which I use in my projects

## Provided Classes

### htl::shared_span\<T\>

## Install
HTL is a header only library. If you only need one class, you can copy it from the
inlclude/htl directory, and place it in your projects include directory. To facilitate
easier use in larger projects, or if you wish to insall it on your unix system, you
can use cmake to install the header files, and the cmake config.
```
$ cmake -DCMAKE_INSTALL_PREFIX=/path/to/install ..
$ make install
```
Using the above commands, the htl directory inside include will be copied to
```/path/to/install/include/htl```.
