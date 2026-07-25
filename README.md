# CppReflection
Header-only C++20 library that allows you to reflect C++ types and use that information at runtime (find and call a class method by name using a pointer to an object, get a class member value, and so on).

Dependencies:
 - [EverydayTools](https://github.com/Sunday111/EverydayTools)
 - [googletest](https://github.com/google/googletest) for tests

## Runtime identity

The type registry has normal function-local static lifetime within the linked image, and the per-type caches are inline state. Every translation unit in one executable observes the same reflected `Type` identity. Reflection or metadata access during static teardown after the registry has been destroyed is unsupported. Loading CppReflection independently into multiple shared libraries or plugins is also unsupported: each dynamic shared object may own a separate registry and cache, so reflected pointers and registration identity must not cross DSO boundaries.

## Migration notes

The metadata API deliberately no longer exposes polymorphic extension points. `Type`, `Field`, `Function`, and
`TypeRegistry` are concrete `final` classes owned by the library's static-lifetime registry. The former `Alloc*`,
`Register*`, `Refresh`, and `Clear` mutation APIs were removed; clients inspect metadata through the read-only
registry and create it through `TypeReflectionProvider<T, Enable>` or a type's `ReflectType` member.

CppReflection is now header-only. This gives one registry and one identity cache across translation units in an
executable, but it does not establish shared identity across independently loaded dynamic libraries. Applications
that previously passed reflected metadata between DSOs must instead arrange a single owning registry at an ABI
boundary or avoid exchanging reflection pointers.

## Development checks

Format the repository and run clang-tidy against a consumer compilation database:

```sh
yae format --repository_dir /path/to/CppReflection
yae tidy --repository_dir /path/to/CppReflection --build_dir /path/to/verlet/build --all
```
