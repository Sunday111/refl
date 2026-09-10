#include "gtest/gtest.h"
#include "refl/call_reflected_function.hpp"
#include "refl/get_static_type_info.hpp"

namespace
{
struct Left
{
    int left = 11;
    int ReadLeft() const { return left; }

    static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<Left>(
                   "InheritedLeft",
                   edt::GUID::Create("CB5C9E53-9D28-4246-91A4-751F40BC8054"))
            .Field<"left", &Left::left>()
            .Method<"ReadLeft", &Left::ReadLeft>();
    }
};

struct Right
{
    int right = 22;

    int Read() const { return right; }
    void Write(int value) { right = value; }
    int ReadVolatile() const volatile { return right; }
    int ReadRvalue() && { return right; }

    static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<Right>(
                   "InheritedRight",
                   edt::GUID::Create("DB405CA8-2025-4028-9D11-EB481EB0E3CF"))
            .Field<"right", &Right::right>()
            .Method<"Read", &Right::Read>()
            .Method<"Write", &Right::Write>()
            .Method<"ReadVolatile", &Right::ReadVolatile>()
            .Method<"ReadRvalue", &Right::ReadRvalue>();
    }
};

struct Child : Left, Right
{
    static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<Child>(
                   "InheritedChild",
                   edt::GUID::Create("E2D095C7-E22A-4058-B4A9-85DC84D90129"))
            .Base<Left>()
            .Base<Right>();
    }
};

struct VirtualChild : Left, virtual Right
{
    static void ReflectType(refl::TypeReflector<VirtualChild>& rt)
    {
        rt.SetName("InheritedVirtualChild");
        rt.SetGUID(edt::GUID::Create("5E184C6B-95C4-44BD-95FC-24196D20E610"));
        rt.SetBaseClass<Left>();
        rt.SetBaseClass<Right>();
    }
};

struct Prefix
{
    int prefix = 7;
};

struct Grandchild : Prefix, Child
{
    static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<Grandchild>(
                   "InheritedGrandchild",
                   edt::GUID::Create("F5539C17-47CD-4DB3-AF80-2BD07F9881E0"))
            .Base<Child>();
    }
};

template <typename T>
class InheritedMembersTest : public testing::Test
{
};

using InheritanceTypes = testing::Types<Child, VirtualChild>;
TYPED_TEST_SUITE(InheritedMembersTest, InheritanceTypes);

TYPED_TEST(InheritedMembersTest, AdjustsFieldAndMethodOwners)
{
    TypeParam instance;
    ASSERT_NE(static_cast<void*>(&instance), static_cast<void*>(static_cast<Right*>(&instance)));
    const auto* type = refl::GetTypeInfo<TypeParam>();
    const auto* read = type->GetMethod("Read");
    ASSERT_NE(read, nullptr);
    EXPECT_EQ(read->GetObjectType(), type);
    int result = 0;
    read->Call(&instance, &result, nullptr, 0);
    EXPECT_EQ(result, 22);
    EXPECT_EQ(refl::CallMethod<int>(read, std::as_const(instance)), 22);

    const auto* field = type->GetField("right");
    ASSERT_NE(field, nullptr);
    EXPECT_EQ(field->GetValue(&instance), &instance.right);
    EXPECT_EQ(*static_cast<int*>(field->GetValue(&instance)), 22);
    *static_cast<int*>(field->GetValue(&instance)) = 37;
    EXPECT_EQ(instance.right, 37);
    EXPECT_EQ(instance.left, 11);

    read->Call(&instance, &result, nullptr, 0);
    EXPECT_EQ(result, 37);
    EXPECT_EQ(refl::CallMethod<int>(read, std::as_const(instance)), 37);
    refl::CallMethod<void>(type->GetMethod("Write"), instance, 51);
    EXPECT_EQ(instance.right, 51);
    EXPECT_EQ(instance.left, 11);
    EXPECT_EQ(refl::CallMethod<int>(type->GetMethod("ReadVolatile"), instance), 51);
    EXPECT_EQ(refl::CallMethod<int>(type->GetMethod("ReadRvalue"), instance), 51);
}

TYPED_TEST(InheritedMembersTest, AdjustsTypedInstanceToRegisteredBase)
{
    TypeParam instance;
    const auto* method = refl::GetTypeInfo<Right>()->GetMethods()[0];
    EXPECT_EQ(refl::CallMethod<int>(method, instance), 22);
    EXPECT_EQ(refl::CallMethod<int>(method, std::as_const(instance)), 22);
}

TEST(InheritedMembersTest, AdjustsThroughMultipleRegisteredBases)
{
    Grandchild instance;
    const auto* method = refl::GetTypeInfo<Right>()->GetMethods()[0];
    EXPECT_EQ(refl::CallMethod<int>(method, instance), 22);
    const auto* child_method = refl::GetTypeInfo<Child>()->GetMethod("Read");
    EXPECT_EQ(refl::CallMethod<int>(child_method, instance), 22);
}

TEST(InheritedMembersTest, RejectsIncompatibleMethodOwner)
{
    Right instance;
    const auto* method = refl::GetTypeInfo<Child>()->GetMethod("Read");
    EXPECT_THROW(refl::CallMethod<int>(method, instance), std::invalid_argument);
}

struct ConflictingBase
{
    float value = 63.0f;
    float Read() const { return value; }

    static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<ConflictingBase>("ConflictingBase", {})
            .Field<"right", &ConflictingBase::value>()
            .Method<"Read", &ConflictingBase::Read>();
    }
};

struct AmbiguousChild : Child, ConflictingBase
{
    static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<AmbiguousChild>("AmbiguousChild", {}).Base<Child>().Base<ConflictingBase>();
    }
};

struct RuntimeAmbiguousChild : Child, ConflictingBase
{
    static void ReflectType(refl::TypeReflector<RuntimeAmbiguousChild>& rt)
    {
        rt.SetName("RuntimeAmbiguousChild");
        rt.SetBaseClass<Child>();
        rt.SetBaseClass<ConflictingBase>();
    }
};

struct OtherRight : Right
{
    static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<OtherRight>("OtherRight", {}).Base<Right>();
    }
};

struct Diamond : Child, OtherRight
{
    static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<Diamond>("Diamond", {}).Base<Child>().Base<OtherRight>();
    }
};

struct VirtualRight : virtual Right
{
    static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<VirtualRight>("VirtualRight", {}).Base<Right>();
    }
};

struct OtherVirtualRight : virtual Right
{
    static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<OtherVirtualRight>("OtherVirtualRight", {}).Base<Right>();
    }
};

struct VirtualDiamond : VirtualRight, OtherVirtualRight
{
    static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<VirtualDiamond>("VirtualDiamond", {})
            .Base<VirtualRight>()
            .Base<OtherVirtualRight>();
    }
};

template <typename Info>
concept CanFindRight = requires(const Info& info) { info.template GetField<"right">(); };

template <typename Info>
concept CanFindRead = requires(const Info& info) { info.template GetMethod<"Read">(); };

TEST(InheritedMembersTest, InheritsStaticTraitsFromEveryBase)
{
    constexpr auto info = refl::GetStaticTypeInfo<Child>();
    static_assert(info.GetField<"left">() == &Left::left);
    static_assert(info.GetField<"right">() == &Right::right);
    static_assert(info.GetMethod<"Read">() == &Right::Read);
    static_assert(info.GetMethod<"ReadLeft">() == &Left::ReadLeft);
    static_assert(refl::GetStaticTypeInfo<Grandchild>().GetField<"right">() == &Right::right);
    const auto* type = refl::GetTypeInfo<Child>();
    EXPECT_EQ(type->GetBaseClasses().size(), 2);
    EXPECT_TRUE(type->IsA(refl::GetTypeInfo<Left>()->GetGuid()));
    EXPECT_TRUE(type->IsA(refl::GetTypeInfo<Right>()->GetGuid()));
    EXPECT_EQ(type->GetField("missing"), nullptr);
    EXPECT_EQ(type->GetMethod("missing"), nullptr);
    EXPECT_EQ(type->GetFields().size(), 2);
    EXPECT_EQ(type->GetMethods().size(), 5);
}

TEST(InheritedMembersTest, RejectsAmbiguousStaticLookupWithoutRejectingTheType)
{
    using Info = decltype(refl::GetStaticTypeInfo<AmbiguousChild>());
    static_assert(Info::AllFields.Count("right") == 1);
    static_assert(Info::AllFields.IsAmbiguous("right"));
    static_assert(Info::AllMethods.Count("Read") == 1);
    static_assert(Info::AllMethods.IsAmbiguous("Read"));
    static_assert(!CanFindRight<Info>);
    static_assert(!CanFindRead<Info>);
    static_assert(refl::GetStaticTypeInfo<AmbiguousChild>().GetField<"left">() == &Left::left);
    using DiamondInfo = decltype(refl::GetStaticTypeInfo<Diamond>());
    static_assert(!CanFindRight<DiamondInfo>);
    static_assert(!CanFindRead<DiamondInfo>);
}

struct AmbiguousGrandchild : AmbiguousChild
{
    static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<AmbiguousGrandchild>("AmbiguousGrandchild", {}).Base<AmbiguousChild>();
    }
};

struct ConflictsAgain : AmbiguousChild
{
    int own_value = 77;
    int OwnRead() const { return own_value; }

    static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<ConflictsAgain>("ConflictsAgain", {})
            .Base<AmbiguousChild>()
            .Field<"right", &ConflictsAgain::own_value>()
            .Method<"Read", &ConflictsAgain::OwnRead>();
    }
};

struct PartlyReflectedDiamond : Child, OtherRight
{
    static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<PartlyReflectedDiamond>("PartlyReflectedDiamond", {}).Base<Child>();
    }
};

template <typename T>
class AmbiguousMembersTest : public testing::Test
{
};

using AmbiguousTypes = testing::
    Types<AmbiguousChild, RuntimeAmbiguousChild, Diamond, AmbiguousGrandchild, ConflictsAgain, PartlyReflectedDiamond>;
TYPED_TEST_SUITE(AmbiguousMembersTest, AmbiguousTypes);

TYPED_TEST(AmbiguousMembersTest, RejectsConflictingNamesOnlyWhenUsed)
{
    TypeParam instance;
    const auto* type = refl::GetTypeInfo<TypeParam>();
    const auto* ambiguous_field = type->GetField("right");
    const auto* ambiguous_method = type->GetMethod("Read");
    ASSERT_NE(ambiguous_field, nullptr);
    ASSERT_NE(ambiguous_method, nullptr);
    EXPECT_TRUE(ambiguous_field->IsAmbiguous());
    EXPECT_TRUE(ambiguous_method->IsAmbiguous());
    EXPECT_EQ(ambiguous_field->GetType(), nullptr);
    EXPECT_EQ(ambiguous_method->GetReturnType(), nullptr);
    EXPECT_TRUE(ambiguous_method->GetArguments().empty());
    EXPECT_EQ(std::ranges::count(type->GetFields(), "right", &refl::Field::GetName), 1);
    EXPECT_EQ(std::ranges::count(type->GetMethods(), "Read", &refl::Function::GetName), 1);
    EXPECT_EQ(type->GetField("left")->GetValue(&instance), &instance.left);
    EXPECT_EQ(refl::CallMethod<int>(type->GetMethod("ReadLeft"), instance), 11);
    for (const auto* field : type->GetFields())
    {
        if (field->GetName() == "right")
        {
            EXPECT_THROW(static_cast<void>(field->GetValue(&instance)), std::invalid_argument);
        }
    }
    for (const auto* method : type->GetMethods())
    {
        if (method->GetName() == "Read")
        {
            EXPECT_THROW(refl::CallMethod<int>(method, instance), std::invalid_argument);
            int result = 0;
            EXPECT_THROW(method->Call(&instance, &result, nullptr, 0), std::invalid_argument);
        }
    }
}

TEST(InheritedMembersTest, RejectsAmbiguousBaseConversion)
{
    Diamond instance;
    const auto* right = refl::GetTypeInfo<Right>();
    EXPECT_EQ(refl::GetTypeInfo<Diamond>()->CastTo(right, &instance), nullptr);
    EXPECT_THROW(refl::CallMethod<int>(right->GetMethod("Read"), instance), std::invalid_argument);
    EXPECT_EQ(
        refl::GetTypeInfo<Diamond>()->CastTo(refl::GetTypeInfo<Child>(), &instance),
        static_cast<Child*>(&instance));
    EXPECT_TRUE(refl::GetTypeInfo<Diamond>()->IsA(right->GetGuid()));
}

TEST(InheritedMembersTest, ConvertsSharedVirtualBaseOnce)
{
    VirtualDiamond instance;
    const auto* right = refl::GetTypeInfo<Right>();
    EXPECT_EQ(refl::GetTypeInfo<VirtualDiamond>()->CastTo(right, &instance), static_cast<Right*>(&instance));
    EXPECT_EQ(refl::CallMethod<int>(right->GetMethod("Read"), instance), 22);
}

struct RuntimeBase
{
    int field = 19;
    int Read() const { return field; }
    static void ReflectType(refl::TypeReflector<RuntimeBase>& rt)
    {
        rt.SetName("RuntimeBase");
        rt.AddField<&RuntimeBase::field>("field");
        rt.AddMethod<&RuntimeBase::Read>("Read");
    }
};

struct RuntimeChild : Left, virtual RuntimeBase
{
    static void ReflectType(refl::TypeReflector<RuntimeChild>& rt)
    {
        rt.SetName("RuntimeChild");
        rt.SetBaseClass<Left>();
        rt.SetBaseClass<RuntimeBase>();
    }
};

TEST(InheritedMembersTest, InheritsRuntimeOnlyBaseMetadata)
{
    RuntimeChild instance;
    const auto* type = refl::GetTypeInfo<RuntimeChild>();
    EXPECT_EQ(type->GetField("field")->GetValue(&instance), &instance.field);
    EXPECT_EQ(refl::CallMethod<int>(type->GetMethod("Read"), instance), 19);
    EXPECT_EQ(refl::CallMethod<int>(type->GetMethod("ReadLeft"), instance), 11);
}

struct RecursiveChild;

struct RecursiveBase
{
    RecursiveChild* child = nullptr;
    int value = 29;
    int Read() const { return value; }
    static void ReflectType(refl::TypeReflector<RecursiveBase>& rt);
};

struct RecursiveChild : Left, RecursiveBase
{
    static void ReflectType(refl::TypeReflector<RecursiveChild>& rt)
    {
        rt.SetName("InheritedRecursiveChild");
        rt.SetBaseClass<RecursiveBase>();
    }
};

void RecursiveBase::ReflectType(refl::TypeReflector<RecursiveBase>& rt)
{
    rt.SetName("InheritedRecursiveBase");
    rt.AddField<&RecursiveBase::child>("child");
    rt.AddField<&RecursiveBase::value>("value");
    rt.AddMethod<&RecursiveBase::Read>("Read");
}

TEST(InheritedMembersTest, CompletesInheritedMetadataAfterRecursiveRegistration)
{
    static_cast<void>(refl::GetTypeInfo<RecursiveBase>());
    RecursiveChild instance;
    const auto* type = refl::GetTypeInfo<RecursiveChild>();
    ASSERT_EQ(type->GetFields().size(), 2);
    ASSERT_EQ(type->GetMethods().size(), 1);
    EXPECT_EQ(type->GetField("value")->GetValue(&instance), &instance.value);
    EXPECT_EQ(refl::CallMethod<int>(type->GetMethod("Read"), instance), 29);
}
}  // namespace
