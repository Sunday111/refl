# refl
Header-only C++20 library that allows you to reflect C++ types and use that information at runtime (find and call a class method by name using a pointer to an object, get a class member value, and so on).

Dependencies:
 - [edt](https://github.com/Sunday111/edt)
 - [googletest](https://github.com/google/googletest) for tests

## Runtime identity

The type registry has normal function-local static lifetime within the linked image, and the per-type caches are inline state. Every translation unit in one executable observes the same reflected `Type` identity. Reflection or metadata access during static teardown after the registry has been destroyed is unsupported. Loading refl independently into multiple shared libraries or plugins is also unsupported: each dynamic shared object may own a separate registry and cache, so reflected pointers and registration identity must not cross DSO boundaries.

## Metadata API

The metadata API has no polymorphic extension points: `Type`, `Field`, `Function`, and `TypeRegistry` are
concrete `final` classes owned by the registry, and the registry itself is read-only to clients. Metadata is
produced by specializing `TypeReflectionProvider<T, Enable>` or by giving a type a `ReflectType` member — not
by calling registration or mutation functions.

Each class reflects its own instance fields and methods. Registering a base-class member as a derived-class
member is rejected at compile time. A class declares each reflected base once; its members are inherited
automatically:

```cpp
return refl::StaticClassTypeInfo<Child>("Child", guid)
    .Base<Left>()
    .Base<Right>()
    .Field<"own_field", &Child::own_field>();
```

Base traits are combined into the child description. During runtime metadata registration, inherited accessors
are bound to direct conversions to their declaring classes. Field access, method invocation and `IsA` do not
walk parent metadata. `Type::GetBaseClasses()` exposes the directly registered base GUIDs.

Inherited hierarchies require compile-time base traits to generate those direct conversions. Callback-style
member declarations remain supported, including a runtime-only base with no further reflected bases.
Registering a hierarchy whose ancestor relationships exist only in callbacks is rejected.

When a name appears more than once, the child stores one ambiguous descriptor for that name. Static traits
expose `IsAmbiguous()` on enumerated descriptors; `GetField<"name">()` and `GetMethod<"name">()` are unavailable
for ambiguous names. Runtime `Type::GetField(name)` and `GetMethod(name)` return the descriptor, or `nullptr`
when the name is absent. Inspect `Field::IsAmbiguous()` or `Function::IsAmbiguous()` before using an entry.
Accessing an ambiguous field or invoking an ambiguous method throws `std::invalid_argument`. Its field or
return type is `nullptr`, and an ambiguous method has no argument signature. Other members remain usable.
Ambiguity persists through further inheritance; no declaration silently replaces a conflicting entry.

Inherited field and method adapters accept an instance pointer of the reflected type, including when the
member belongs to a non-first or virtual base. `Function::GetObjectType()` identifies its required object
type. Raw `Field::GetValue`, `Function::Call`, and `Function::CallForwarded` callers must supply the correct
instance pointer before erasing it to `void*`.

`CallMethod` adjusts reflected instances through registered bases and throws `std::invalid_argument` when
the method's object type is unreachable or has several distinct base subobjects. Paths to one shared virtual
base subobject identify one conversion. The GUID-only `SetBaseClass(guid)` overload records ancestry for
`IsA`, but does not inherit members or provide an instance conversion.

## Using it (yae)

refl is a [yae](https://github.com/Sunday111/yae) package. Declare it in your `*.package.json`:

```json
{ "link": "https://github.com/Sunday111/refl main", "packages": ["refl"] }
```

and depend on the `refl` module. Types live in the `refl::` namespace and are included as
`#include "refl/..."` — e.g. `refl::GetTypeInfo<T>()`, `refl::TypeRegistry`,
`refl::StaticClassTypeInfo<T>(...)`.

## Building & testing

```sh
yae build refl_tests
yae run refl_tests
```

The root `CMakeLists.txt` is yae-generated; CI runs on every push.

The repo carries its own `.clang-format` / `.clang-tidy`. Once `build/` exists, both checks
run against it:

```sh
yae format
yae tidy --build_dir build --all
```
