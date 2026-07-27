#include <atomic>
#include <memory>
#include <stdexcept>
#include <string_view>

#include "refl/call_reflected_function.hpp"
#include "refl/get_type_info.hpp"
#include "gtest/gtest.h"

namespace
{
struct alignas(128) OverAligned
{
    int value = 17;
    static void ReflectType(refl::TypeReflector<OverAligned>& reflector) { reflector.SetName("OverAligned"); }
};

struct NonDefault
{
    explicit NonDefault(int in_value) : value(in_value) {}
    int value;
    static void ReflectType(refl::TypeReflector<NonDefault>& reflector) { reflector.SetName("NonDefault"); }
};

struct MoveOnly
{
    explicit MoveOnly(int in_value) : value(std::make_unique<int>(in_value)) {}
    MoveOnly(MoveOnly&&) noexcept = default;
    MoveOnly& operator=(MoveOnly&&) noexcept = default;
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
    std::unique_ptr<int> value;
    static void ReflectType(refl::TypeReflector<MoveOnly>& reflector) { reflector.SetName("MoveOnly"); }
};

struct CopyableValue
{
    int value = 0;
    static void ReflectType(refl::TypeReflector<CopyableValue>& reflector)
    {
        reflector.SetName("CopyableValue");
    }
};

struct ThrowOnSecondCopy
{
    ThrowOnSecondCopy() { ++live; }
    ThrowOnSecondCopy(const ThrowOnSecondCopy&)
    {
        ++copies;
        if (copies == 2)
        {
            throw std::runtime_error("second copy");
        }
        ++live;
    }
    ThrowOnSecondCopy& operator=(const ThrowOnSecondCopy&) = delete;
    ~ThrowOnSecondCopy()
    {
        --live;
        ++destructions;
    }

    static void Reset()
    {
        live = 0;
        copies = 0;
        destructions = 0;
    }

    static void ReflectType(refl::TypeReflector<ThrowOnSecondCopy>& reflector)
    {
        reflector.SetName("ThrowOnSecondCopy");
    }

    inline static int live = 0;
    inline static int copies = 0;
    inline static int destructions = 0;
};

OverAligned MakeOverAligned()
{
    return {};
}
NonDefault MakeNonDefault()
{
    return NonDefault(23);
}
MoveOnly MakeMoveOnly()
{
    return MoveOnly(31);
}
ThrowOnSecondCopy MakeThrowOnSecondCopy()
{
    return {};
}

struct InvocationState
{
    int consumed_move_only = 0;
    int consumed_copyable = 0;
    int observed_move_only = 0;
    int global_value = 41;
    MoveOnly rvalue_value{47};
    std::atomic<int> void_calls = 0;
};

InvocationState& GetInvocationState()
{
    static InvocationState state;
    return state;
}

void ConsumeMoveOnly(MoveOnly value)
{
    GetInvocationState().consumed_move_only = *value.value;
}

void ConsumeCopyable(CopyableValue value)
{
    GetInvocationState().consumed_copyable = value.value;
}

void ObserveMoveOnly(const MoveOnly& value)
{
    GetInvocationState().observed_move_only = *value.value;
}

int& GetReference()
{
    return GetInvocationState().global_value;
}

MoveOnly&& GetRvalueReference()
{
    return std::move(GetInvocationState().rvalue_value);
}

void CallVoid()
{
    ++GetInvocationState().void_calls;
}
int ThrowingFunction()
{
    throw std::runtime_error("expected");
}

struct InvocationHost
{
    [[nodiscard]] static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<InvocationHost>(
                   "InvocationHost",
                   edt::GUID::Create("EBED1B11-1FA7-477B-B49E-26C044E13BC2"))
            .Method<"MakeOverAligned", &MakeOverAligned>()
            .Method<"MakeNonDefault", &MakeNonDefault>()
            .Method<"MakeMoveOnly", &MakeMoveOnly>()
            .Method<"MakeThrowOnSecondCopy", &MakeThrowOnSecondCopy>()
            .Method<"ConsumeMoveOnly", &ConsumeMoveOnly>()
            .Method<"ConsumeCopyable", &ConsumeCopyable>()
            .Method<"ObserveMoveOnly", &ObserveMoveOnly>()
            .Method<"GetReference", &GetReference>()
            .Method<"GetRvalueReference", &GetRvalueReference>()
            .Method<"CallVoid", &CallVoid>()
            .Method<"ThrowingFunction", &ThrowingFunction>();
    }
};

const refl::Function* FindMethod(const refl::Type* type, std::string_view name)
{
    for (const auto* method : type->GetMethods())
    {
        if (method->GetName() == name)
        {
            return method;
        }
    }
    return nullptr;
}

}  // namespace

TEST(ReflectedInvocationTest, HandlesValueCategoriesAndReturnTypes)
{
    const auto* type = refl::GetTypeInfo<InvocationHost>();

    const auto over_aligned = refl::CallFunction<OverAligned>(FindMethod(type, "MakeOverAligned"));
    EXPECT_EQ(over_aligned.value, 17);

    const auto non_default = refl::CallFunction<NonDefault>(FindMethod(type, "MakeNonDefault"));
    EXPECT_EQ(non_default.value, 23);

    auto move_only = refl::CallFunction<MoveOnly>(FindMethod(type, "MakeMoveOnly"));
    ASSERT_NE(move_only.value, nullptr);
    EXPECT_EQ(*move_only.value, 31);

    MoveOnly consumed(37);
    refl::CallFunction<void>(FindMethod(type, "ConsumeMoveOnly"), std::move(consumed));
    EXPECT_EQ(consumed.value, nullptr);
    EXPECT_EQ(GetInvocationState().consumed_move_only, 37);

    CopyableValue copyable{38};
    refl::CallFunction<void>(FindMethod(type, "ConsumeCopyable"), copyable);
    EXPECT_EQ(copyable.value, 38);
    EXPECT_EQ(GetInvocationState().consumed_copyable, 38);

    MoveOnly lvalue_move_only(40);
    EXPECT_THROW(
        refl::CallFunction<void>(FindMethod(type, "ConsumeMoveOnly"), lvalue_move_only),
        std::invalid_argument);
    ASSERT_NE(lvalue_move_only.value, nullptr);

    const MoveOnly observed(39);
    refl::CallFunction<void>(FindMethod(type, "ObserveMoveOnly"), observed);
    ASSERT_NE(observed.value, nullptr);
    EXPECT_EQ(GetInvocationState().observed_move_only, 39);

    auto& reference = refl::CallFunction<int&>(FindMethod(type, "GetReference"));
    reference = 43;
    EXPECT_EQ(GetInvocationState().global_value, 43);

    auto moved_reference = refl::CallFunction<MoveOnly&&>(FindMethod(type, "GetRvalueReference"));
    ASSERT_NE(moved_reference.value, nullptr);
    EXPECT_EQ(*moved_reference.value, 47);
    EXPECT_EQ(GetInvocationState().rvalue_value.value, nullptr);

    refl::CallFunction<void>(FindMethod(type, "CallVoid"));
    EXPECT_EQ(GetInvocationState().void_calls, 1);
    EXPECT_THROW(
        static_cast<void>(refl::CallFunction<int>(FindMethod(type, "ThrowingFunction"))),
        std::runtime_error);

    ThrowOnSecondCopy::Reset();
    EXPECT_THROW(
        static_cast<void>(refl::CallFunction<ThrowOnSecondCopy>(FindMethod(type, "MakeThrowOnSecondCopy"))),
        std::runtime_error);
    EXPECT_EQ(ThrowOnSecondCopy::copies, 2);
    EXPECT_EQ(ThrowOnSecondCopy::destructions, 2);
    EXPECT_EQ(ThrowOnSecondCopy::live, 0);
}
