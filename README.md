# Hunter's Template Library
Hunter's Template Library - a colletion of template containers, not provided in the STL, which I use in my projects

## Provided Classes

### htl::shared_span\<T\>
shared_span is similar to the std::span\<T\> object, introduced in the C++20 standard. The STL span is a non-owning
container, which alows you to look into a continuous segment of memory, owned and controled by another container
(such as an std::vector, or std::array). These allow for an easy method to gain access to the same dataset without
copying the contents, as std::span just contains a pointer to the beginning of the data sequence, and the length it
is allowed to access. One problem however, is that if the object containing the data is destroyed or changed
(such as calling push_back on an std::vector), the span can be invalidated, and it has no way of knowing. Another
issue can arrise when you want multiple objects of the same time to access the same data, and this data is not
needed anywhere else. Since a span cannot own the data, you must create a vector elsewhere which has the sole purpose
of just existing, so that all of the other said ojects can view the data. This also alows for the posibility of the
data being destroyed or changed, nulifying the views of the data in all of the other objects. To remedy these
problems, I have written the shared_span container. This is a template container, which owns its data in a manner,
simillar to that of std::shared_ptr. Multiple shared_span objects can share a view into the same dataset, but
the data is only destroyed when the last shared_span is also destroyed. This is accomplished by storing the data
of shared_span in a std::vector, which is maintained by a std::shared_ptr. As such, if you create a shared_span
from a pre-existing data-set, it is forced to initially make a copy of this data, to store internally, ensuring
its perpetual existence for the lifetime of itself and any of its future progeny shared_span. Therefore, it is best
to use shared_span when multiple other objects need view access to the same data set, and when you have no need to
guard the inital data set elsewhere. It is also impossible to add new elements to the shared_span, once it has
been created. Other shared_spans can be generated in the same manner as with std::span, and sub views can also
be generated if only a portion is needed. This container makes no use of raw pointers, and as such, this
container is easily serialized by libraries which could not serialize a std::span, due to their use of a raw
pointer (such as cereal).

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

If you with to build the tests, you can run cmake with ```-DHTL_BUILD_TESTS=On```. This will produce
and executable in the build directory at ```tests/htl-tests```. To build and run the tests, GTest must
already be installed on your system.
