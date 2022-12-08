#include <gtest/gtest.h>

#include "reservoir-sampler/reservoir_sampler_linear.h"

#include <array>
#include <numeric>

#include "TestTypes.h"

TEST(ReservoirSamplerLinear, SamplersOfDifferentTypes_CreeateFillAndDestroy_DoNotCrash)
{
	{
		ReservoirSamplerLinear<std::string> sampler;
		sampler.sampleElement(1, "list");
		sampler.sampleElement(1, "of");
		sampler.sampleElement(1, "test");
		sampler.sampleElement(1, "string");
		sampler.sampleElement(1, "items");
	}

	{
		ReservoirSamplerLinear<size_t> sampler;
		sampler.sampleElement(1, 0);
		sampler.sampleElement(1, 1);
		sampler.sampleElement(1, 2);
		sampler.sampleElement(1, 3);
		sampler.sampleElement(1, 4);
	}

	{
		ReservoirSamplerLinear<std::vector<int>> sampler;
		sampler.sampleElement(1, std::vector<int>{{1, 2}});
		sampler.sampleElement(1, std::vector<int>{{3, 4}});
		sampler.sampleElement(1, std::vector<int>{{5, 6}});
		sampler.sampleElement(1, std::vector<int>{{7, 8}});
		sampler.sampleElement(1, std::vector<int>{{9, 10}});
	}

	{
		ReservoirSamplerLinear<std::thread> sampler;
		sampler.sampleElementEmplace(1);
		sampler.sampleElementEmplace(1);
		sampler.sampleElementEmplace(1);
		sampler.sampleElementEmplace(1);
		sampler.sampleElementEmplace(1);
	}
}

TEST(ReservoirSamplerLinear, Sampler_OneElementsAdded_HasOnlyTheOriginalElement)
{
	ReservoirSamplerLinear<size_t> sampler;
	const size_t value{10};
	sampler.sampleElement(1, value);

	{
		ASSERT_TRUE(sampler.getResult().has_value());
		EXPECT_EQ(value, *sampler.getResult());
	}

	{
		std::optional<size_t> result = sampler.consumeResult();
		ASSERT_TRUE(result.has_value());
		EXPECT_EQ(value, *sampler.getResult());
	}
}

TEST(ReservoirSamplerLinear, EmptySampler_Reset_DoesNotCrash)
{
	ReservoirSamplerLinear<size_t> sampler;
	sampler.reset();
}

TEST(ReservoirSamplerLinear, SamplerWithAResult_Reset_CanBeReused)
{
	ReservoirSamplerLinear<size_t> sampler;
	const size_t value1{10};
	const size_t value2{15};

	sampler.sampleElement(1, value1);

	sampler.reset();

	sampler.sampleElement(1, value2);

	ASSERT_TRUE(sampler.getResult().has_value());
	EXPECT_EQ(value2, *sampler.getResult());
}

TEST(ReservoirSamplerLinear, SamplerWithAResult_Consume_CanBeReused)
{
	ReservoirSamplerLinear<size_t> sampler;
	const size_t value1{10};
	const size_t value2{15};

	sampler.sampleElement(1, value1);

	{
		std::optional<size_t> result = sampler.consumeResult();
		ASSERT_TRUE(result.has_value());
		EXPECT_EQ(value1, *result);
	}

	sampler.sampleElement(1, value2);

	ASSERT_TRUE(sampler.getResult().has_value());
	EXPECT_EQ(value2, *sampler.getResult());
}

TEST(ReservoirSamplerLinear, EmptySampler_Copied_DoesNotCrash)
{
	ReservoirSamplerLinear<size_t> sampler;
	ReservoirSamplerLinear<size_t> copySampler(sampler);
	(void)copySampler;
}

TEST(ReservoirSamplerLinear, Sampler_Copied_HoldsTheData)
{
	ReservoirSamplerLinear<size_t> sampler;
	const size_t value{10};

	sampler.sampleElement(1, value);

	ReservoirSamplerLinear<size_t> samplerCopy(sampler);

	{
		std::optional<size_t> result = samplerCopy.consumeResult();
		ASSERT_TRUE(result.has_value());
		EXPECT_EQ(value, *result);
	}

	{
		std::optional<size_t> result = sampler.consumeResult();
		ASSERT_TRUE(result.has_value());
		EXPECT_EQ(value, *result);
	}
}

TEST(ReservoirSamplerLinear, EmptySampler_Moved_DoesNotCrash)
{
	ReservoirSamplerLinear<size_t> sampler;
	ReservoirSamplerLinear<size_t> copySampler(std::move(sampler));
	(void)copySampler;
}

TEST(ReservoirSamplerLinear, Sampler_Moved_ValueIsMoved)
{
	ReservoirSamplerLinear<size_t> sampler;
	const size_t value{10};

	sampler.sampleElement(1, value);

	ReservoirSamplerLinear<size_t> samplerMovedTo(std::move(sampler));

	{
		std::optional<size_t> result = samplerMovedTo.consumeResult();
		ASSERT_TRUE(result.has_value());
		EXPECT_EQ(value, *result);
	}
}

TEST(ReservoirSamplerLinear, Sampler_Moved_OldSamplerCanBeReused)
{
	ReservoirSamplerLinear<size_t> sampler;
	const size_t value1{10};
	const size_t value2{15};

	sampler.sampleElement(1, value1);

	{
		ReservoirSamplerLinear<size_t> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	sampler.sampleElement(1, value2);

	{
		std::optional<size_t> result = sampler.consumeResult();
		ASSERT_TRUE(result.has_value());
		EXPECT_EQ(value2, *result);
	}
}

TEST(ReservoirSamplerLinear, EmptySampler_CopyAssigned_DoesNotCrash)
{
	ReservoirSamplerLinear<size_t> sampler;
	ReservoirSamplerLinear<size_t> sampler2;
	sampler = sampler2;
}

TEST(ReservoirSamplerLinear, Sampler_CopyAssigned_HoldsTheData)
{
	const size_t valueOld{10};
	const size_t value{15};
	ReservoirSamplerLinear<size_t> sampler1;
	ReservoirSamplerLinear<size_t> sampler2;

	sampler1.sampleElement(1, valueOld);
	sampler2.sampleElement(1, value);

	sampler1 = sampler2;

	{
		std::optional<size_t> result = sampler1.consumeResult();
		ASSERT_TRUE(result.has_value());
		EXPECT_EQ(value, *result);
	}

	{
		std::optional<size_t> result = sampler2.consumeResult();
		ASSERT_TRUE(result.has_value());
		EXPECT_EQ(value, *result);
	}
}

TEST(ReservoirSamplerLinear, EmptySampler_MoveAssigned_DoesNotCrash)
{
	ReservoirSamplerLinear<size_t> sampler;
	ReservoirSamplerLinear<size_t> sampler2;
	sampler = std::move(sampler2);
}

TEST(ReservoirSamplerLinear, Sampler_MoveAssigned_ValueIsMoved)
{
	const size_t valueOld{10};
	const size_t value{15};
	ReservoirSamplerLinear<size_t> sampler1;
	ReservoirSamplerLinear<size_t> sampler2;

	sampler1.sampleElement(1, valueOld);
	sampler2.sampleElement(1, value);

	sampler1 = std::move(sampler2);

	{
		std::optional<size_t> result = sampler1.consumeResult();
		ASSERT_TRUE(result.has_value());
		EXPECT_EQ(value, *result);
	}
}

TEST(ReservoirSamplerLinear, Sampler_MoveAssigned_OldSamplerCanBeReused)
{
	const size_t valueOld{20};
	const size_t value{10};
	const size_t value2{33};
	ReservoirSamplerLinear<size_t> sampler1;
	ReservoirSamplerLinear<size_t> sampler2;

	sampler1.sampleElement(1, valueOld);
	sampler2.sampleElement(1, value);

	sampler1 = std::move(sampler2);

	sampler2.sampleElement(1, value2);

	{
		std::optional<size_t> result = sampler2.consumeResult();
		ASSERT_TRUE(result.has_value());
		EXPECT_EQ(value2, *result);
	}
}

TEST(ReservoirSamplerLinear, SamplersWithDifferentWeightTypes_FilledWithData_ProduceExpectedResults)
{
	{
		ReservoirSamplerLinear<int, int> sampler;
		for (int i = 0; i < 100; ++i)
		{
			const int weight = (i == 5) ? 1 : 0;
			sampler.sampleElement(weight, i);
		}

		std::optional<int> result = sampler.consumeResult();
		ASSERT_TRUE(result.has_value());
		ASSERT_EQ(*result, 5);
	}

	{
		ReservoirSamplerLinear<int, char> sampler;
		for (int i = 0; i < 100; ++i)
		{
			const char weight = (i == 5) ? 1 : 0;
			sampler.sampleElement(weight, i);
		}

		std::optional<int> result = sampler.consumeResult();
		ASSERT_TRUE(result.has_value());
		ASSERT_EQ(*result, 5);
	}

	{
		ReservoirSamplerLinear<int, bool> sampler;
		for (int i = 0; i < 100; ++i)
		{
			const bool weight = (i == 5);
			sampler.sampleElement(weight, i);
		}

		std::optional<int> result = sampler.consumeResult();
		ASSERT_TRUE(result.has_value());
		ASSERT_EQ(*result, 5);
	}
}

TEST(ReservoirSamplerLinear, SamplerSizeOfFive_SamplingFromStreamOfTwenty_ProducesEqualFrequencies)
{
	std::array<int, 20> frequences{};
	// reuse random to speed things up a bit
	std::mt19937 rand{std::random_device{}()};
	for (int i = 0; i < 10000; ++i)
	{
		ReservoirSamplerLinear<int, int, std::mt19937&> sampler(rand);

		for (int n = 0; n < 20; ++n)
		{
			sampler.sampleElement(1, n);
		}

		ReservoirSamplerLinear<int, int, std::mt19937&> samplerCopy(sampler);
		ReservoirSamplerLinear<int, int, std::mt19937&> samplerMoved(std::move(samplerCopy));

		const auto result = samplerMoved.getResult();
		ASSERT_TRUE(result.has_value());
		++frequences[*result];
	}

	const float frequencySum = std::accumulate(frequences.begin(), frequences.end(), 0.0f);
	ASSERT_EQ(10000, frequencySum);
	for (const int freq : frequences)
	{
		EXPECT_NEAR(0.05f, freq/frequencySum, 0.01f);
	}
}

TEST(ReservoirSamplerLinear, Sampler_SamplingFromStreamOfWeightedValues_ProducesExpectedFrequencies)
{
	constexpr size_t elementsCount = 21;
	std::array<int, elementsCount> weights;
	std::array<float, elementsCount> expectedFrequencies;

	for (size_t i = 0; i < elementsCount; ++i)
	{
		// triangle distrubution that peaks at 10 with value of 11
		weights[i] = 11 - std::abs(static_cast<int>(i) - 10);
	}

	const float weightSum = std::accumulate(weights.begin(), weights.end(), 0.0f);
	for (size_t i = 0; i < elementsCount; ++i)
	{
		expectedFrequencies[i] = weights[i] / weightSum;
	}

	std::array<int, elementsCount> frequences{};
	// reuse random to speed things up a bit
	std::mt19937 rand{std::random_device{}()};
	for (int i = 0; i < 100000; ++i)
	{
		ReservoirSamplerLinear<size_t, int, std::mt19937&> sampler(rand);

		for (size_t n = 0; n < elementsCount; ++n)
		{
			sampler.sampleElement(weights[n], n);
		}

		ReservoirSamplerLinear<size_t, int, std::mt19937&> samplerCopy(sampler);
		ReservoirSamplerLinear<size_t, int, std::mt19937&> samplerMoved(std::move(samplerCopy));

		const auto result = samplerMoved.getResult();
		ASSERT_TRUE(result.has_value());
#pragma GCC diagnostic push // GCC 11 complains about this line and it doesn't make sense, ignore
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
		++frequences[*result];
#pragma GCC diagnostic pop
	}

	const float frequencySum = std::accumulate(frequences.begin(), frequences.end(), 0.0f);
	for (size_t i = 0; i < elementsCount; ++i)
	{
		EXPECT_NEAR(expectedFrequencies[i], frequences[i]/frequencySum, 0.01f);
	}
}

TEST(ReservoirSamplerLinear, SamplersWithDifferentTypes_ConstructedFilledCopiedAndMoved_Compiles)
{
	{
		ReservoirSamplerLinear<int, int> sampler;
		sampler.sampleElement(2, 10);
		sampler.sampleElementEmplace(1, 20);
		sampler.sampleElement(3, 40);

		ReservoirSamplerLinear<int, int> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerLinear<int, int> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerLinear<std::string, int> sampler;
		sampler.sampleElement(2, "test");
		sampler.sampleElementEmplace(1, "test2");
		sampler.sampleElement(3, "test3");

		ReservoirSamplerLinear<std::string, int> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerLinear<std::string, int> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerLinear<ImplicitCtor> sampler;
		ImplicitCtor v(2);
		sampler.sampleElement(4, v);
		sampler.sampleElement(2, ImplicitCtor(2));
		sampler.sampleElementEmplace(1, 2);
		sampler.sampleElement(3, 2);

		ReservoirSamplerLinear<ImplicitCtor> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerLinear<ImplicitCtor> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerLinear<Simple> sampler;
		Simple v(2);
		sampler.sampleElement(4, v);
		sampler.sampleElement(2, Simple(2));
		sampler.sampleElementEmplace(1, 2);
		//sampler.sampleElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerLinear<Simple> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerLinear<Simple> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerLinear<TwoArgs> sampler;
		TwoArgs v(2, 5.5f);
		sampler.sampleElement(4, v);
		sampler.sampleElement(2, TwoArgs(2, 9.0f));
		sampler.sampleElementEmplace(1, 2, 3.5f);
		//sampler.sampleElement(3, 2); // won't compile, no matching constructor

		ReservoirSamplerLinear<TwoArgs> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerLinear<TwoArgs> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerLinear<NonCopyable> sampler;
		//NonCopyable v(2);
		//sampler.sampleElement(4, v);	// won't compile, no copy constructor
		sampler.sampleElement(2, NonCopyable(2));
		sampler.sampleElementEmplace(1, 2);
		//sampler.sampleElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerLinear<NonCopyable> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerLinear<NonMovable> sampler;
		NonMovable v(2);
		sampler.sampleElement(4, v);
		sampler.sampleElement(2, NonMovable(2));
		sampler.sampleElementEmplace(1, 2);
		//sampler.sampleElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerLinear<NonMovable> samplerCopy(sampler);
		(void)samplerCopy;
	}

	{
		ReservoirSamplerLinear<NonCopyableNonMovable> sampler;
		//NonCopyableNonMovable v(2);
		//sampler.sampleElement(4, v); // won't compile, no copy constructor
		//sampler.sampleElement(2, NonCopyableNonMovable(2)); // won't compile, should be copy or move constructible
		sampler.sampleElementEmplace(1, 2);
		//sampler.sampleElement(3, 2); // won't compile, explicit constructor
	}

	{
		ReservoirSamplerLinear<OnlyCopyConstructible> sampler;
		OnlyCopyConstructible v(2);
		sampler.sampleElement(4, v);
		sampler.sampleElement(2, OnlyCopyConstructible(2));
		sampler.sampleElementEmplace(1, 2);
		//sampler.sampleElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerLinear<OnlyCopyConstructible> samplerCopy(sampler);
		(void)samplerCopy;
	}

	{
		ReservoirSamplerLinear<OnlyCopyAssignable> sampler;
		OnlyCopyAssignable v(2);
		//sampler.sampleElement(4, v); // won't compile, no copy constructor
		//sampler.sampleElement(2, OnlyCopyAssignable(2)); // won't compile, should be copy or move constructible
		sampler.sampleElementEmplace(1, 2);
		//sampler.sampleElement(3, 2); // won't compile, explicit constructor
	}

	{
		ReservoirSamplerLinear<OnlyMoveConstructible> sampler;
		OnlyMoveConstructible v(2);
		//sampler.sampleElement(4, v); // won't compile, no copy constructor
		sampler.sampleElement(2, OnlyMoveConstructible(2));
		sampler.sampleElementEmplace(1, 2);
		//sampler.sampleElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerLinear<OnlyMoveConstructible> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerLinear<OnlyMoveAssignable> sampler;
		OnlyMoveAssignable v(2);
		//sampler.sampleElement(4, v); // won't compile, no copy constructor
		//sampler.sampleElement(2, OnlyMoveAssignable(2)); // won't compile, should be copy or move constructible
		sampler.sampleElementEmplace(1, 2);
		//sampler.sampleElement(3, 2); // won't compile, explicit constructor
	}
}

TEST(ReservoirSamplerLinear, Sampler_ConstructedFilledAndConsumed_ProducesReasonableAmountOfMoves)
{
	const size_t streamSize = 500;

	CopyMoveCounter::Reset();

	ReservoirSamplerLinear<CopyMoveCounter> sampler;
	EXPECT_EQ(0, CopyMoveCounter::GetConstructionsCount());
	EXPECT_EQ(0, CopyMoveCounter::GetCopiesCount());
	EXPECT_EQ(0, CopyMoveCounter::GetMovesCount());

	for (size_t n = 0; n < streamSize; ++n)
	{
		sampler.sampleElementEmplace(1);
	}

	const int constructionsCount = CopyMoveCounter::GetConstructionsCount();
	const int movesCount = CopyMoveCounter::GetMovesCount();
	EXPECT_GT(50, constructionsCount);
	EXPECT_LT(1, constructionsCount);
	EXPECT_EQ(0, CopyMoveCounter::GetCopiesCount());
	EXPECT_EQ(1, constructionsCount - movesCount);

	{
		auto result = sampler.getResult();
		(void)result;
	}

	EXPECT_EQ(constructionsCount, CopyMoveCounter::GetConstructionsCount());
	EXPECT_EQ(1, CopyMoveCounter::GetCopiesCount());
	EXPECT_EQ(movesCount, CopyMoveCounter::GetMovesCount());

	{
		std::optional<CopyMoveCounter> result = sampler.consumeResult();
		(void)result;
	}

	EXPECT_EQ(constructionsCount, CopyMoveCounter::GetConstructionsCount());
	EXPECT_EQ(1, CopyMoveCounter::GetCopiesCount());
	EXPECT_EQ(movesCount + 1, CopyMoveCounter::GetMovesCount());
}
