#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <ranges>
#include <string>
#include <string_view>

#include "refl/get_type_info.hpp"
#include "gtest/gtest.h"

enum class ExampleEnum : std::uint8_t
{
    EntryA,
    EntryB,
    EntryC,
    EntryD,
    EntryE,
    EntryF,
    EntryG
};

namespace refl
{
template <>
struct TypeReflectionProvider<ExampleEnum>
{
    [[nodiscard]] inline constexpr static auto ReflectType()
    {
        return StaticEnumTypeInfo<ExampleEnum>("ExampleEnum", edt::GUID::Create("0706BBE6-79CC-425C-812E-5ABDDC52DCAD"))
            .Value(ExampleEnum::EntryA, "EntryA")
            .Value(ExampleEnum::EntryB, "EntryB")
            .Value(ExampleEnum::EntryC, "EntryC")
            .Value(ExampleEnum::EntryD, "EntryD")
            .Value(ExampleEnum::EntryE, "EntryE")
            .Value(ExampleEnum::EntryF, "EntryF")
            .Value(ExampleEnum::EntryG, "EntryG");
    }
};

}  // namespace refl

struct ExampleStruct
{
    std::int32_t field_i32;
    std::int64_t field_i64;

    [[nodiscard]] constexpr int32_t Foo(int32_t arg) const { return field_i32 * arg; }
    [[nodiscard]] constexpr int64_t Bar(int64_t arg) const { return field_i64 * arg; }

    [[nodiscard]] inline constexpr static auto ReflectType()
    {
        return refl::StaticClassTypeInfo<ExampleStruct>(
                   "ExampleStruct",
                   edt::GUID::Create("3F1144D7-97A8-40E8-AD83-58CDC61B4815"))
            .Field<"field_i32", &ExampleStruct::field_i32>()
            .Field<"field_i64", &ExampleStruct::field_i64>()
            .Method<"Foo", &ExampleStruct::Foo>()
            .Method<"Bar", &ExampleStruct::Bar>();
    }
};

TEST(reflTest, StaticEnumerationInfo)
{
    constexpr const auto example_ti = refl::GetStaticTypeInfo<ExampleEnum>();
    static_assert(example_ti.ToString(ExampleEnum::EntryA) == "EntryA");
    static_assert(example_ti.Parse("EntryA") == ExampleEnum::EntryA);

    static_assert(example_ti.ToString(ExampleEnum::EntryB) == "EntryB");
    static_assert(example_ti.Parse("EntryB") == ExampleEnum::EntryB);

    static_assert(example_ti.ToString(ExampleEnum::EntryC) == "EntryC");
    static_assert(example_ti.Parse("EntryC") == ExampleEnum::EntryC);

    static_assert(example_ti.ToString(ExampleEnum::EntryD) == "EntryD");
    static_assert(example_ti.Parse("EntryD") == ExampleEnum::EntryD);

    static_assert(example_ti.ToString(ExampleEnum::EntryE) == "EntryE");
    static_assert(example_ti.Parse("EntryE") == ExampleEnum::EntryE);

    static_assert(example_ti.ToString(ExampleEnum::EntryF) == "EntryF");
    static_assert(example_ti.Parse("EntryF") == ExampleEnum::EntryF);

    static_assert(example_ti.ToString(ExampleEnum::EntryG) == "EntryG");
    static_assert(example_ti.Parse("EntryG") == ExampleEnum::EntryG);

    const refl::Type* dyn_ti = refl::GetTypeInfo<ExampleEnum>();
    EXPECT_EQ(dyn_ti->GetName(), "ExampleEnum");
    EXPECT_EQ(dyn_ti->GetGuid(), example_ti.guid);
}

TEST(reflTest, StaticStructureInfo)
{
    constexpr const auto example_ti = refl::GetStaticTypeInfo<ExampleStruct>();
    static_assert(example_ti.HasField("field_i32"));
    static_assert(!example_ti.HasField("field_hint"));

    static_assert(example_ti.HasMethod("Foo"));
    static_assert(!example_ti.HasMethod("Fooo"));

    constexpr ExampleStruct obj{.field_i32 = 11, .field_i64 = 99};

    // Find and call method
    constexpr auto method_ptr = example_ti.template GetMethod<"Foo">();
    static_assert(((obj.*method_ptr)(10)) == 110);

    // Find field and get value
    constexpr auto field_ptr = example_ti.template GetField<"field_i32">();
    static_assert((obj.*field_ptr) == 11);

    const refl::Type* dyn_ti = refl::GetTypeInfo<ExampleStruct>();
    EXPECT_EQ(dyn_ti->GetName(), "ExampleStruct");
    EXPECT_EQ(dyn_ti->GetGuid(), example_ti.guid);

    auto find_by_name = [](const auto& collection, std::string_view name) -> int
    {
        auto it =
            std::find_if(collection.begin(), collection.end(), [&](const auto* o) { return o->GetName() == name; });

        if (it != collection.end())
        {
            return static_cast<int>(std::distance(collection.begin(), it));
        }

        return -1;
    };

    const auto fields = dyn_ti->GetFields();
    EXPECT_EQ(fields.size(), 2);
    ASSERT_NE(find_by_name(fields, "field_i32"), -1);
    ASSERT_NE(find_by_name(fields, "field_i64"), -1);

    const auto methods = dyn_ti->GetMethods();
    EXPECT_EQ(methods.size(), 2);
    ASSERT_NE(find_by_name(methods, "Foo"), -1);
    ASSERT_NE(find_by_name(methods, "Bar"), -1);
}
