# Intro

The spectools utilies perform conversions between an EBML Schema (an XML representation of the definition of an EBML Document Type) and other human-readable XML formats or library files.

The utilities include:

- data2lib
- data2lib2
- data2spec
- spec2data
- tabler

# Installation

Building spectools is dependant on coremake. To build for make run these commands from ~/foundation-source.

```
cd corec/tools/coremake
make
sudo make install
cd ../../..
```

To build spectool run these commands from ~/foundation-source:

coremake gcc_linux
(or coremake vs17_x64 or gcc_osx_x64, etc. The list of possible targets are the .build files in corec/tools/coremake)

```
make -C spectool
```

The resulting utilties will appear within the `~/foundation-source/releases` directory.

# Using spectools

The spectools utilities should be executed from within the `~/foundation-source/spectool` directory. For instance, to create `spec.xml` (a Drupal friendly specification table) from `specdata.xml` (an EBML Schema, run.:

```
cd spectool
../releases/gcc_osx_x64/spec2data
```

Note: the subdirectory of `../releases` (i.e. `gcc_osx_x64`) is contextual based on your operating system.

# To Do

Better instructions for individual spectools