#include <array>
#include <atomic>
#include <cstddef>
#include <format>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <vector>

#include "refl/get_static_type_info.hpp"
#include "refl/get_type_info.hpp"
#include "edt/template/static_for.hpp"
#include "cross_tu_identity.hpp"
#include "gtest/gtest.h"

namespace
{
struct RecursiveType
{
    RecursiveType* next = nullptr;

    static void ReflectType(refl::TypeReflector<RecursiveType>& reflector)
    {
        reflector.SetName("RecursiveType");
        reflector.AddField<&RecursiveType::next>("next");
    }
};

struct MetadataArgument
{
    static void ReflectType(refl::TypeReflector<MetadataArgument>& reflector)
    {
        reflector.SetName("MetadataArgument");
    }
};

void ObserveMetadataArgument(const MetadataArgument&) {}

struct MetadataHost
{
    [[nodiscard]] static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<MetadataHost>("MetadataHost", {})
            .Method<"ObserveMetadataArgument", &ObserveMetadataArgument>();
    }
};

struct ZeroGuidA
{
    static void ReflectType(refl::TypeReflector<ZeroGuidA>& reflector) { reflector.SetName("ZeroGuidA"); }
};

struct ZeroGuidB
{
    static void ReflectType(refl::TypeReflector<ZeroGuidB>& reflector) { reflector.SetName("ZeroGuidB"); }
};

struct DuplicateGuidA
{
    [[nodiscard]] static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<DuplicateGuidA>(
            "DuplicateGuidA",
            edt::GUID::Create("75AB8166-6742-4128-94C2-CF53364B75F8"));
    }
};

struct DuplicateGuidB
{
    [[nodiscard]] static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<DuplicateGuidB>(
            "DuplicateGuidB",
            edt::GUID::Create("75AB8166-6742-4128-94C2-CF53364B75F8"));
    }
};

struct ThrowingFieldType
{
    static void ReflectType(refl::TypeReflector<ThrowingFieldType>&)
    {
        throw std::runtime_error("field type reflection failed");
    }
};

struct ContainsThrowingField
{
    ThrowingFieldType field;
    static void ReflectType(refl::TypeReflector<ContainsThrowingField>& reflector)
    {
        reflector.SetName("ContainsThrowingField");
        reflector.AddField<&ContainsThrowingField::field>("field");
    }
};

struct ThrowingReturnType
{
    static void ReflectType(refl::TypeReflector<ThrowingReturnType>&)
    {
        throw std::runtime_error("return type reflection failed");
    }
};

ThrowingReturnType MakeThrowingReturnType()
{
    return {};
}

struct ContainsThrowingFunction
{
    static void ReflectType(refl::TypeReflector<ContainsThrowingFunction>& reflector)
    {
        reflector.SetName("ContainsThrowingFunction");
        reflector.AddMethod<&MakeThrowingReturnType>("MakeThrowingReturnType");
    }
};

struct SubstringNames
{
    int value = 0;
    void Method() {}

    static void ReflectType(refl::TypeReflector<SubstringNames>& reflector)
    {
        const std::string storage = "xxTypeNameyyFieldNamezzMethodName";
        reflector.SetName(std::string_view(storage).substr(2, 8));
        reflector.AddField<&SubstringNames::value>(std::string_view(storage).substr(12, 9));
        reflector.AddMethod<&SubstringNames::Method>(std::string_view(storage).substr(23, 10));
    }
};

struct FailedGraphRoot;
FailedGraphRoot MakeFailedGraphRoot();

struct StagedGraphNested
{
    static void ReflectType(refl::TypeReflector<StagedGraphNested>& reflector)
    {
        reflector.SetName("StagedGraphNested");
        reflector.AddMethod<&MakeFailedGraphRoot>("MakeFailedGraphRoot");
    }
};

struct FailedGraphRoot
{
    inline static std::size_t reflection_attempts = 0;

    static void ReflectType(refl::TypeReflector<FailedGraphRoot>& reflector)
    {
        ++reflection_attempts;
        reflector.SetName("FailedGraphRoot");
        static_cast<void>(refl::GetTypeInfo<StagedGraphNested>());
        throw std::runtime_error("root graph reflection failed");
    }
};

FailedGraphRoot MakeFailedGraphRoot()
{
    return {};
}

struct SavepointNested;
SavepointNested MakeSavepointNested();

struct SavepointChild
{
    static void ReflectType(refl::TypeReflector<SavepointChild>& reflector)
    {
        reflector.SetName("SavepointChild");
        reflector.SetGUID(edt::GUID::Create("C12AE0D7-8539-4167-995C-F6F70A784E13"));
        reflector.AddMethod<&MakeSavepointNested>("MakeSavepointNested");
    }
};

struct SavepointNested
{
    static void ReflectType(refl::TypeReflector<SavepointNested>& reflector)
    {
        reflector.SetName("SavepointNested");
        static_cast<void>(refl::GetTypeInfo<SavepointChild>());
        throw std::runtime_error("nested reflection failed after staging a child");
    }
};

SavepointNested MakeSavepointNested()
{
    return {};
}

struct CatchesNestedFailure
{
    static void ReflectType(refl::TypeReflector<CatchesNestedFailure>& reflector)
    {
        reflector.SetName("CatchesNestedFailure");
        try
        {
            static_cast<void>(refl::GetTypeInfo<SavepointNested>());
        }
        catch (const std::runtime_error&)
        {
        }
    }
};

struct SuccessfulLeaf
{
    static void ReflectType(refl::TypeReflector<SuccessfulLeaf>& reflector)
    {
        reflector.SetName("SuccessfulLeaf");
        reflector.SetGUID(edt::GUID::Create("47E7D53A-BB53-40E5-89E8-8B239DA873F2"));
    }
};

struct SuccessfulMiddle
{
    static void ReflectType(refl::TypeReflector<SuccessfulMiddle>& reflector)
    {
        reflector.SetName("SuccessfulMiddle");
        reflector.SetGUID(edt::GUID::Create("87BF1892-E50A-414C-A8D6-DCFA84ED6191"));
        static_cast<void>(refl::GetTypeInfo<SuccessfulLeaf>());
    }
};

struct SuccessfulSibling
{
    static void ReflectType(refl::TypeReflector<SuccessfulSibling>& reflector)
    {
        reflector.SetName("SuccessfulSibling");
        reflector.SetGUID(edt::GUID::Create("66C0829F-05BC-48A9-9489-B659C45D0784"));
    }
};

struct SuccessfulRoot
{
    static void ReflectType(refl::TypeReflector<SuccessfulRoot>& reflector)
    {
        reflector.SetName("SuccessfulRoot");
        reflector.SetGUID(edt::GUID::Create("663F11C4-3B16-4209-A1F0-8EB43B01237C"));
        static_cast<void>(refl::GetTypeInfo<SuccessfulMiddle>());
        static_cast<void>(refl::GetTypeInfo<SuccessfulSibling>());
    }
};

struct BatchDuplicateFirst
{
    static void ReflectType(refl::TypeReflector<BatchDuplicateFirst>& reflector)
    {
        reflector.SetName("BatchDuplicateFirst");
        reflector.SetGUID(edt::GUID::Create("81500CE2-29D6-4330-8F85-05C0232A276E"));
    }
};

struct BatchDuplicateSecond
{
    static void ReflectType(refl::TypeReflector<BatchDuplicateSecond>& reflector)
    {
        reflector.SetName("BatchDuplicateSecond");
        reflector.SetGUID(edt::GUID::Create("81500CE2-29D6-4330-8F85-05C0232A276E"));
    }
};

struct BatchDuplicateRoot
{
    static void ReflectType(refl::TypeReflector<BatchDuplicateRoot>& reflector)
    {
        reflector.SetName("BatchDuplicateRoot");
        static_cast<void>(refl::GetTypeInfo<BatchDuplicateFirst>());
        static_cast<void>(refl::GetTypeInfo<BatchDuplicateSecond>());
    }
};

struct MutualB;

struct MutualA
{
    MutualB* other = nullptr;
    static void ReflectType(refl::TypeReflector<MutualA>& reflector);
};

struct MutualB
{
    MutualA* other = nullptr;
    static void ReflectType(refl::TypeReflector<MutualB>& reflector);
};

void MutualA::ReflectType(refl::TypeReflector<MutualA>& reflector)
{
    reflector.SetName("MutualA");
    reflector.AddField<&MutualA::other>("other");
}

void MutualB::ReflectType(refl::TypeReflector<MutualB>& reflector)
{
    reflector.SetName("MutualB");
    reflector.AddField<&MutualB::other>("other");
}

struct ConcurrentSuccessChild
{
    static void ReflectType(refl::TypeReflector<ConcurrentSuccessChild>& reflector)
    {
        reflector.SetName("ConcurrentSuccessChild");
    }
};

struct ConcurrentSuccessRoot
{
    static void ReflectType(refl::TypeReflector<ConcurrentSuccessRoot>& reflector)
    {
        reflector.SetName("ConcurrentSuccessRoot");
        static_cast<void>(refl::GetTypeInfo<ConcurrentSuccessChild>());
    }
};

struct ConcurrentFailedChild
{
    static void ReflectType(refl::TypeReflector<ConcurrentFailedChild>& reflector)
    {
        reflector.SetName("ConcurrentFailedChild");
    }
};

struct ConcurrentFailedRoot
{
    inline static std::size_t reflection_attempts = 0;

    static void ReflectType(refl::TypeReflector<ConcurrentFailedRoot>& reflector)
    {
        ++reflection_attempts;
        reflector.SetName("ConcurrentFailedRoot");
        static_cast<void>(refl::GetTypeInfo<ConcurrentFailedChild>());
        throw std::runtime_error("concurrent root reflection failed");
    }
};

struct LegacyProviderType
{
};

template <std::size_t index>
struct StableAddressType
{
    static void ReflectType(refl::TypeReflector<StableAddressType>& reflector)
    {
        reflector.SetName("StableAddressType");
    }
};

template <std::size_t index>
struct DenseMetadataElement
{
    inline static int value = 0;
    static void Function() {}
};

struct ManyMembers
{
    static void ReflectType(refl::TypeReflector<ManyMembers>& reflector)
    {
        reflector.SetName("ManyMembers");
        edt::StaticFor<128>(
            [&]<std::size_t index>
            {
                reflector.AddField<&DenseMetadataElement<index>::value>(std::format("value{}", index));
                reflector.AddMethod<&DenseMetadataElement<index>::Function>(std::format("Function{}", index));
            });
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

namespace refl
{
template <typename T>
struct TypeReflectionProvider<T, std::enable_if_t<std::is_same_v<T, LegacyProviderType>>>
{
    static void ReflectType(TypeReflector<T>& reflector) { reflector.SetName("LegacyProviderType"); }
};
}  // namespace refl

static_assert(std::is_final_v<refl::Type>);
static_assert(std::is_final_v<refl::Field>);
static_assert(std::is_final_v<refl::Function>);
static_assert(std::is_final_v<refl::TypeRegistry>);
static_assert(!std::is_abstract_v<refl::Type>);
static_assert(!std::is_abstract_v<refl::Field>);
static_assert(!std::is_abstract_v<refl::Function>);
static_assert(!std::is_abstract_v<refl::TypeRegistry>);
static_assert(!std::is_polymorphic_v<refl::Type>);
static_assert(!std::is_polymorphic_v<refl::Field>);
static_assert(!std::is_polymorphic_v<refl::Function>);
static_assert(!std::is_polymorphic_v<refl::TypeRegistry>);
static_assert(!std::is_copy_constructible_v<refl::Type>);
static_assert(!std::is_copy_constructible_v<refl::Field>);
static_assert(!std::is_copy_constructible_v<refl::Function>);

TEST(TypeRegistryTest, SharesTypeIdentityAcrossTranslationUnits)
{
    const auto* type = GetCrossTranslationUnitTypeFromA();
    EXPECT_EQ(type, GetCrossTranslationUnitTypeFromB());
    EXPECT_EQ(GetCrossTranslationUnitRegistryFromA(), GetCrossTranslationUnitRegistryFromB());
    EXPECT_EQ(GetCrossTranslationUnitRegistryCountFromA(), refl::GetTypeRegistry()->GetTypesCount());
    EXPECT_EQ(FindCrossTranslationUnitTypeFromB(), type);
}

TEST(TypeRegistryTest, SupportsLegacyTwoParameterProviderSpecialization)
{
    EXPECT_EQ(refl::GetTypeInfo<LegacyProviderType>()->GetName(), "LegacyProviderType");
}

TEST(TypeRegistryTest, PublishesOneStableRecursiveTypeAcrossThreads)
{
    constexpr std::size_t thread_count = 16;
    std::array<const refl::Type*, thread_count> results{};
    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    for (std::size_t index = 0; index < thread_count; ++index)
    {
        threads.emplace_back([&, index] { results[index] = refl::GetTypeInfo<RecursiveType>(); });
    }
    for (auto& thread : threads)
    {
        thread.join();
    }

    for (const auto* result : results)
    {
        EXPECT_EQ(result, results.front());
    }
    ASSERT_EQ(results.front()->GetFields().size(), 1);
    EXPECT_EQ(results.front()->GetFields().front()->GetType(), refl::GetTypeInfo<RecursiveType*>());

    EXPECT_EQ(refl::GetTypeInfo<const RecursiveType>(), results.front());
    EXPECT_EQ(refl::GetTypeInfo<volatile RecursiveType>(), results.front());
    EXPECT_EQ(refl::GetTypeInfo<const volatile RecursiveType>(), results.front());

    const auto* mutable_reference = refl::GetTypeInfo<RecursiveType&>();
    const auto* const_reference = refl::GetTypeInfo<const RecursiveType&>();
    const auto* volatile_reference = refl::GetTypeInfo<volatile RecursiveType&>();
    EXPECT_NE(mutable_reference, const_reference);
    EXPECT_EQ(mutable_reference->GetName(), "RecursiveType&");
    EXPECT_EQ(const_reference->GetName(), "RecursiveType const&");
    EXPECT_EQ(volatile_reference->GetName(), "RecursiveType volatile&");
    EXPECT_EQ(refl::GetTypeInfo<const RecursiveType*>()->GetName(), "RecursiveType const*");
    EXPECT_EQ(refl::GetTypeInfo<RecursiveType* const*>()->GetName(), "RecursiveType* const*");

    const auto* method = FindMethod(refl::GetTypeInfo<MetadataHost>(), "ObserveMetadataArgument");
    ASSERT_NE(method, nullptr);
    ASSERT_EQ(method->GetArguments().size(), 1);
    EXPECT_EQ(method->GetArguments().front(), refl::GetTypeInfo<const MetadataArgument&>());
    EXPECT_EQ(method->GetArguments().front()->GetName(), "MetadataArgument const&");
}

TEST(TypeRegistryTest, EnforcesGuidUniquenessTransactionally)
{
    const auto* zero_a = refl::GetTypeInfo<ZeroGuidA>();
    const auto* zero_b = refl::GetTypeInfo<ZeroGuidB>();
    EXPECT_NE(zero_a, zero_b);
    EXPECT_EQ(refl::GetTypeRegistry()->FindType({}), nullptr);

    const auto* duplicate_a = refl::GetTypeInfo<DuplicateGuidA>();
    ASSERT_NE(duplicate_a, nullptr);
    const std::size_t count_before_failure = refl::GetTypeRegistry()->GetTypesCount();
    EXPECT_THROW(static_cast<void>(refl::GetTypeInfo<DuplicateGuidB>()), std::runtime_error);
    EXPECT_EQ(refl::GetTypeRegistry()->GetTypesCount(), count_before_failure);
    EXPECT_EQ(refl::GetTypeRegistry()->FindType(duplicate_a->GetGuid()), duplicate_a);
    EXPECT_THROW(static_cast<void>(refl::GetTypeInfo<DuplicateGuidB>()), std::runtime_error);
    EXPECT_EQ(refl::GetTypeRegistry()->GetTypesCount(), count_before_failure);
}

TEST(TypeRegistryTest, DiscardsMetadataAfterNestedReflectionFailure)
{
    auto* registry = refl::GetTypeRegistry();
    const std::size_t count_before_field = registry->GetTypesCount();
    EXPECT_THROW(static_cast<void>(refl::GetTypeInfo<ContainsThrowingField>()), std::runtime_error);
    EXPECT_EQ(registry->GetTypesCount(), count_before_field);

    const std::size_t count_before_function = registry->GetTypesCount();
    EXPECT_THROW(static_cast<void>(refl::GetTypeInfo<ContainsThrowingFunction>()), std::runtime_error);
    EXPECT_EQ(registry->GetTypesCount(), count_before_function);
}

TEST(TypeRegistryTest, RollsBackSuccessfulNestedReflectionWhenRootFails)
{
    auto* registry = refl::GetTypeRegistry();
    const std::size_t count_before_failure = registry->GetTypesCount();

    EXPECT_THROW(static_cast<void>(refl::GetTypeInfo<FailedGraphRoot>()), std::runtime_error);
    EXPECT_EQ(registry->GetTypesCount(), count_before_failure);

    // The root failure is memoized. The nested type was only staged, so retrying it
    // starts a fresh transaction and observes the root failure instead of stale metadata.
    EXPECT_THROW(static_cast<void>(refl::GetTypeInfo<FailedGraphRoot>()), std::runtime_error);
    EXPECT_EQ(FailedGraphRoot::reflection_attempts, 1);
    EXPECT_THROW(static_cast<void>(refl::GetTypeInfo<StagedGraphNested>()), std::runtime_error);
    EXPECT_THROW(static_cast<void>(refl::GetTypeInfo<StagedGraphNested>()), std::runtime_error);
    EXPECT_EQ(registry->GetTypesCount(), count_before_failure);
}

TEST(TypeRegistryTest, CommitsSuccessfulMultiLevelAndSiblingGraphAtomically)
{
    auto* registry = refl::GetTypeRegistry();
    const std::size_t count_before_reflection = registry->GetTypesCount();

    const auto* root = refl::GetTypeInfo<SuccessfulRoot>();
    EXPECT_EQ(registry->GetTypesCount(), count_before_reflection + 4);
    EXPECT_EQ(registry->FindType(root->GetGuid()), root);
    EXPECT_EQ(
        registry->FindType(edt::GUID::Create("47E7D53A-BB53-40E5-89E8-8B239DA873F2")),
        refl::GetTypeInfo<SuccessfulLeaf>());
    EXPECT_EQ(
        registry->FindType(edt::GUID::Create("87BF1892-E50A-414C-A8D6-DCFA84ED6191")),
        refl::GetTypeInfo<SuccessfulMiddle>());
    EXPECT_EQ(
        registry->FindType(edt::GUID::Create("66C0829F-05BC-48A9-9489-B659C45D0784")),
        refl::GetTypeInfo<SuccessfulSibling>());
}

TEST(TypeRegistryTest, RejectsDuplicateGuidWithinStagedBatchAtomically)
{
    auto* registry = refl::GetTypeRegistry();
    const std::size_t count_before_failure = registry->GetTypesCount();

    EXPECT_THROW(static_cast<void>(refl::GetTypeInfo<BatchDuplicateRoot>()), std::runtime_error);
    EXPECT_EQ(registry->GetTypesCount(), count_before_failure);
    EXPECT_EQ(registry->FindType(edt::GUID::Create("81500CE2-29D6-4330-8F85-05C0232A276E")), nullptr);

    const auto* first = refl::GetTypeInfo<BatchDuplicateFirst>();
    EXPECT_EQ(registry->FindType(first->GetGuid()), first);
    EXPECT_EQ(registry->GetTypesCount(), count_before_failure + 1);
}

TEST(TypeRegistryTest, SupportsMutuallyRecursiveReflection)
{
    const auto* type_a = refl::GetTypeInfo<MutualA>();
    const auto* type_b = refl::GetTypeInfo<MutualB>();
    ASSERT_EQ(type_a->GetFields().size(), 1);
    ASSERT_EQ(type_b->GetFields().size(), 1);
    EXPECT_EQ(type_a->GetFields().front()->GetType(), refl::GetTypeInfo<MutualB*>());
    EXPECT_EQ(type_b->GetFields().front()->GetType(), refl::GetTypeInfo<MutualA*>());
}

TEST(TypeRegistryTest, SerializesConcurrentSuccessfulAndFailingGraphs)
{
    constexpr std::size_t thread_count = 16;
    auto* registry = refl::GetTypeRegistry();
    const std::size_t count_before_reflection = registry->GetTypesCount();
    std::array<const refl::Type*, thread_count / 2> successes{};
    std::atomic_size_t failures = 0;
    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    for (std::size_t index = 0; index < thread_count; ++index)
    {
        threads.emplace_back(
            [&, index]
            {
                if (index % 2 == 0)
                {
                    successes[index / 2] = refl::GetTypeInfo<ConcurrentSuccessRoot>();
                    return;
                }
                try
                {
                    static_cast<void>(refl::GetTypeInfo<ConcurrentFailedRoot>());
                }
                catch (const std::runtime_error&)
                {
                    ++failures;
                }
            });
    }
    for (auto& thread : threads)
    {
        thread.join();
    }

    EXPECT_EQ(failures, thread_count / 2);
    EXPECT_EQ(ConcurrentFailedRoot::reflection_attempts, 1);
    for (const auto* type : successes)
    {
        EXPECT_EQ(type, successes.front());
    }
    EXPECT_EQ(registry->GetTypesCount(), count_before_reflection + 2);
}

TEST(TypeRegistryTest, PreservesMetadataPointersAcrossDequeBlockGrowth)
{
    constexpr std::size_t type_count = 128;
    auto* registry = refl::GetTypeRegistry();
    const std::size_t count_before_reflection = registry->GetTypesCount();
    std::array<const refl::Type*, type_count> types{};
    edt::StaticFor<type_count>([&]<std::size_t index>
                               { types[index] = refl::GetTypeInfo<StableAddressType<index>>(); });

    EXPECT_EQ(registry->GetTypesCount(), count_before_reflection + type_count);
    EXPECT_EQ(types.front()->GetName(), "StableAddressType");
    EXPECT_EQ(registry->GetTypeInfo(count_before_reflection), types.front());
    edt::StaticFor<type_count>([&]<std::size_t index>
                               { EXPECT_EQ(types[index], refl::GetTypeInfo<StableAddressType<index>>()); });

    const auto* many_members = refl::GetTypeInfo<ManyMembers>();
    ASSERT_EQ(many_members->GetFields().size(), 128);
    ASSERT_EQ(many_members->GetMethods().size(), 128);
    const auto* first_field = many_members->GetFields().front();
    const auto* first_method = many_members->GetMethods().front();
    EXPECT_EQ(first_field->GetName(), "value0");
    EXPECT_EQ(first_method->GetName(), "Function0");
    for (std::size_t index = 0; index < many_members->GetFields().size(); ++index)
    {
        const auto* field = many_members->GetFields()[index];
        const auto* method = many_members->GetMethods()[index];
        EXPECT_NE(field, nullptr);
        EXPECT_NE(method, nullptr);
        EXPECT_EQ(field->GetName(), std::format("value{}", index));
        EXPECT_EQ(method->GetName(), std::format("Function{}", index));
        if (index > 0)
        {
            EXPECT_NE(field, many_members->GetFields()[index - 1]);
            EXPECT_NE(method, many_members->GetMethods()[index - 1]);
        }
    }
}

TEST(TypeRegistryTest, RollsBackToNestedSavepointWhenOuterProviderCatchesFailure)
{
    auto* registry = refl::GetTypeRegistry();
    const std::size_t count_before_reflection = registry->GetTypesCount();

    const auto* outer = refl::GetTypeInfo<CatchesNestedFailure>();
    ASSERT_NE(outer, nullptr);
    EXPECT_EQ(registry->GetTypesCount(), count_before_reflection + 1);
    EXPECT_EQ(registry->FindType(edt::GUID::Create("C12AE0D7-8539-4167-995C-F6F70A784E13")), nullptr);

    EXPECT_THROW(static_cast<void>(refl::GetTypeInfo<SavepointNested>()), std::runtime_error);
    EXPECT_THROW(static_cast<void>(refl::GetTypeInfo<SavepointChild>()), std::runtime_error);
    EXPECT_EQ(registry->GetTypesCount(), count_before_reflection + 1);
}

TEST(TypeRegistryTest, CopiesBoundedStringViews)
{
    const auto* type = refl::GetTypeInfo<SubstringNames>();
    EXPECT_EQ(type->GetName(), "TypeName");
    ASSERT_EQ(type->GetFields().size(), 1);
    EXPECT_EQ(type->GetFields().front()->GetName(), "FieldName");
    ASSERT_EQ(type->GetMethods().size(), 1);
    EXPECT_EQ(type->GetMethods().front()->GetName(), "MethodName");
}
