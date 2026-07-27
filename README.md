# refl
Header-only C++20 library that allows you to reflect C++ types and use that information at runtime (find and call a class method by name using a pointer to an object, get a class member value, and so on).

Dependencies:
 - [edt](https://github.com/Sunday111/edt)
 - [googletest](https://github.com/google/googletest) for tests

## Runtime identity

The type registry has normal function-local static lifetime within the linked image, and the per-type caches are inline state. Every translation unit in one executable observes the same reflected `Type` identity. Reflection or metadata access during static teardown after the registry has been destroyed is unsupported. Loading refl independently into multiple shared libraries or plugins is also unsupported: each dynamic shared object may own a separate registry and cache, so reflected pointers and registration identity must not cross DSO boundaries.

## Migration notes

The metadata API deliberately no longer exposes polymorphic extension points. `Type`, `Field`, `Function`, and
`TypeRegistry` are concrete `final` classes owned by the library's static-lifetime registry. The former `Alloc*`,
`Register*`, `Refresh`, and `Clear` mutation APIs were removed; clients inspect metadata through the read-only
registry and create it through `TypeReflectionProvider<T, Enable>` or a type's `ReflectType` member.

refl is now header-only. This gives one registry and one identity cache across translation units in an
executable, but it does not establish shared identity across independently loaded dynamic libraries. Applications
that previously passed reflected metadata between DSOs must instead arrange a single owning registry at an ABI
boundary or avoid exchanging reflection pointers.

## Development checks

Format the repository and run clang-tidy against a consumer compilation database:

```sh
yae format --repository_dir /path/to/refl
yae tidy --repository_dir /path/to/refl --build_dir /path/to/verlet/build --all
```

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

The root `CMakeLists.txt` is yae-generated; CI runs on every push. The repo carries its own
`.clang-format` / `.clang-tidy`.
