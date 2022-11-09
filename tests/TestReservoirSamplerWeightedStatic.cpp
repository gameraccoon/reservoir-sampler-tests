#include <gtest/gtest.h>

#include "reservoir-sampler/reservoir_sampler_weighted_static.h"

#include "TestTypes.h"

TEST(ReservoirSamplerWeightedStatic, SamplersOfDifferentTypes_CreeateFillAndDestroy_DoNotCrash)
{
	{
		ReservoirSamplerWeightedStatic<std::string, 5> sampler;
		sampler.addElement(1.0f, "list");
		sampler.addElement(1.0f, "of");
		sampler.addElement(1.0f, "test");
		sampler.addElement(1.0f, "string");
		sampler.addElement(1.0f, "items");
	}

	{
		ReservoirSamplerWeightedStatic<size_t, 5> sampler;
		sampler.addElement(1.0f, 0);
		sampler.addElement(1.0f, 1);
		sampler.addElement(1.0f, 2);
		sampler.addElement(1.0f, 3);
		sampler.addElement(1.0f, 4);
	}

	{
		ReservoirSamplerWeightedStatic<std::vector<int>, 5> sampler;
		sampler.addElement(1.0f, std::vector<int>{{1, 2}});
		sampler.addElement(1.0f, std::vector<int>{{3, 4}});
		sampler.addElement(1.0f, std::vector<int>{{5, 6}});
		sampler.addElement(1.0f, std::vector<int>{{7, 8}});
		sampler.addElement(1.0f, std::vector<int>{{9, 10}});
	}

	{
		ReservoirSamplerWeightedStatic<std::thread, 5> sampler;
		sampler.emplaceElement(1.0f);
		sampler.emplaceElement(1.0f);
		sampler.emplaceElement(1.0f);
		sampler.emplaceElement(1.0f);
		sampler.emplaceElement(1.0f);
	}
}

TEST(ReservoirSamplerWeightedStatic, SamplersOfDifferentSizes_CreeateFillAndDestroy_DoNotCrash)
{
	{
		ReservoirSamplerWeightedStatic<size_t, 5> sampler;
		sampler.addElement(1.0f, 0);
		sampler.addElement(1.0f, 1);
		sampler.addElement(1.0f, 2);
		sampler.addElement(1.0f, 3);
		sampler.addElement(1.0f, 4);
	}

	{
		ReservoirSamplerWeightedStatic<size_t, 1> sampler;
		sampler.addElement(1.0f, 0);
		sampler.addElement(1.0f, 1);
		sampler.addElement(1.0f, 2);
		sampler.addElement(1.0f, 3);
		sampler.addElement(1.0f, 4);
	}

	{
		ReservoirSamplerWeightedStatic<size_t, 300> sampler;
		sampler.addElement(1.0f, 0);
		sampler.addElement(1.0f, 1);
		sampler.addElement(1.0f, 2);
		sampler.addElement(1.0f, 3);
		sampler.addElement(1.0f, 4);
	}

	{
		ReservoirSamplerWeightedStatic<size_t, 3> sampler;
		(void)sampler;
	}
}

TEST(ReservoirSamplerWeightedStatic, SamplerOfSizeFive_FiveElementsAdded_HasOnlyOriginalElements)
{
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	ReservoirSamplerWeightedStatic<size_t, 5> sampler;
	for (size_t value : stream)
	{
		sampler.addElement(1.0f, value);
	}

	{
		std::vector<size_t> result1;
		result1.reserve(stream.size());
		const auto [data, size] = sampler.getResult();
		for (size_t i = 0; i < size; ++i)
		{
			result1.push_back(data[i]);
		}
		std::sort(result1.begin(), result1.end());
		EXPECT_EQ(stream, result1);
	}

	{
		std::vector<size_t> result2 = sampler.consumeResult();
		std::sort(result2.begin(), result2.end());
		EXPECT_EQ(stream, result2);
	}
}

TEST(ReservoirSamplerWeightedStatic, SamplerOfSizeFive_ThreeElementsAdded_HasOnlyOriginalElements)
{
	const std::vector<size_t> stream({10, 11, 12});

	ReservoirSamplerWeightedStatic<size_t, 5> sampler;
	for (size_t value : stream)
	{
		sampler.addElement(1.0f, value);
	}

	{
		std::vector<size_t> result1;
		result1.reserve(stream.size());
		const auto [data, size] = sampler.getResult();
		for (size_t i = 0; i < size; ++i)
		{
			result1.push_back(data[i]);
		}
		std::sort(result1.begin(), result1.end());
		EXPECT_EQ(stream, result1);
	}

	{
		std::vector<size_t> result2 = sampler.consumeResult();
		std::sort(result2.begin(), result2.end());
		EXPECT_EQ(stream, result2);
	}
}

TEST(ReservoirSamplerWeightedStatic, EmptySampler_Reset_DoesNotCrash)
{
	ReservoirSamplerWeightedStatic<size_t, 5> sampler;
	sampler.reset();
}

TEST(ReservoirSamplerWeightedStatic, SamplerWithAResult_Reset_CanBeReused)
{
	ReservoirSamplerWeightedStatic<size_t, 5> sampler;
	const std::vector<size_t> stream1({10, 11, 12, 13, 14});
	const std::vector<size_t> stream2({15, 16, 17, 18, 19});

	for (size_t value : stream1)
	{
		sampler.addElement(1.0f, value);
	}

	sampler.reset();

	for (size_t value : stream2)
	{
		sampler.addElement(1.0f, value);
	}

	{
		std::vector<size_t> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		EXPECT_EQ(result, stream2);
	}
}

TEST(ReservoirSamplerWeightedStatic, SamplerWithAResult_Consume_CanBeReused)
{
	ReservoirSamplerWeightedStatic<size_t, 5> sampler;
	const std::vector<size_t> stream1({10, 11, 12, 13, 14});
	const std::vector<size_t> stream2({15, 16, 17, 18, 19});

	for (size_t value : stream1)
	{
		sampler.addElement(1.0f, value);
	}

	{
		std::vector<size_t> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		ASSERT_EQ(result, stream1);
	}

	for (size_t value : stream2)
	{
		sampler.addElement(1.0f, value);
	}

	{
		std::vector<size_t> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		EXPECT_EQ(result, stream2);
	}
}

TEST(ReservoirSamplerWeightedStatic, Sampler_AddDummyWhenNotConsidered_ProducesExpectedResult)
{
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	ReservoirSamplerWeightedStatic<size_t, 5> sampler;

	for (size_t value : stream)
	{
		const float weight = 1.0f;
		if (sampler.willNextBeConsidered(weight))
		{
			sampler.addElement(weight, value);
		}
		else
		{
			sampler.addDummyElement(weight);
		}
	}

	std::vector<size_t> result = sampler.consumeResult();
	std::sort(result.begin(), result.end());
	EXPECT_EQ(result, stream);
}

TEST(ReservoirSamplerWeightedStatic, EmptySampler_Copied_DoesNotCrash)
{
	ReservoirSamplerWeightedStatic<size_t, 5> sampler;
	ReservoirSamplerWeightedStatic<size_t, 5> copySampler(sampler);
	(void)copySampler;
}

TEST(ReservoirSamplerWeightedStatic, Sampler_Copied_HoldsTheData)
{
	ReservoirSamplerWeightedStatic<size_t, 5> sampler;
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	for (size_t value : stream)
	{
		sampler.addElement(1.0f, value);
	}

	ReservoirSamplerWeightedStatic<size_t, 5> samplerCopy(sampler);

	{
		std::vector<size_t> result = samplerCopy.consumeResult();
		std::sort(result.begin(), result.end());
		EXPECT_EQ(result, stream);
	}

	{
		std::vector<size_t> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		EXPECT_EQ(result, stream);
	}
}

TEST(ReservoirSamplerWeightedStatic, EmptySampler_Moved_DoesNotCrash)
{
	ReservoirSamplerWeightedStatic<size_t, 5> sampler;
	ReservoirSamplerWeightedStatic<size_t, 5> copySampler(std::move(sampler));
	(void)copySampler;
}

TEST(ReservoirSamplerWeightedStatic, Sampler_Moved_ValueIsMoved)
{
	ReservoirSamplerWeightedStatic<size_t, 5> sampler;
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	for (size_t value : stream)
	{
		sampler.addElement(1.0f, value);
	}

	ReservoirSamplerWeightedStatic<size_t, 5> samplerMovedTo(std::move(sampler));

	{
		std::vector<size_t> result = samplerMovedTo.consumeResult();
		std::sort(result.begin(), result.end());
		EXPECT_EQ(result, stream);
	}
}

TEST(ReservoirSamplerWeightedStatic, Sampler_Moved_OldSamplerCanBeReused)
{
	ReservoirSamplerWeightedStatic<size_t, 5> sampler;
	const std::vector<size_t> stream1({10, 11, 12, 13, 14});
	const std::vector<size_t> stream2({15, 16, 17, 18, 19});

	for (size_t value : stream1)
	{
		sampler.addElement(1.0f, value);
	}

	{
		ReservoirSamplerWeightedStatic<size_t, 5> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	for (size_t value : stream2)
	{
		sampler.addElement(1.0f, value);
	}

	{
		std::vector<size_t> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		EXPECT_EQ(result, stream2);
	}
}

TEST(ReservoirSamplerWeightedStatic, SamplersWithDifferentWeightTypes_FilledWithData_ProduceExpectedResults)
{
	{
		ReservoirSamplerWeightedStatic<int, 5, float> sampler;
		for (int i = 0; i < 100; ++i)
		{
			const float weight = (i >= 5 && i < 10) ? 1.0f : 0.0f;
			sampler.addElement(weight, i);
		}

		std::vector<int> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		ASSERT_EQ(result, std::vector<int>({5, 6, 7, 8 ,9}));
	}

	{
		ReservoirSamplerWeightedStatic<int, 5, double> sampler;
		for (int i = 0; i < 100; ++i)
		{
			const double weight = (i >= 5 && i < 10) ? 1.0 : 0.0;
			sampler.addElement(weight, i);
		}

		std::vector<int> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		ASSERT_EQ(result, std::vector<int>({5, 6, 7, 8 ,9}));
	}

	{
		ReservoirSamplerWeightedStatic<int, 5, int> sampler;
		for (int i = 0; i < 100; ++i)
		{
			const int weight = (i >= 5 && i < 10) ? 1 : 0;
			sampler.addElement(weight, i);
		}

		std::vector<int> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		ASSERT_EQ(result, std::vector<int>({5, 6, 7, 8 ,9}));
	}

	{
		ReservoirSamplerWeightedStatic<int, 5, char> sampler;
		for (int i = 0; i < 100; ++i)
		{
			const char weight = (i >= 5 && i < 10) ? 1 : 0;
			sampler.addElement(weight, i);
		}

		std::vector<int> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		ASSERT_EQ(result, std::vector<int>({5, 6, 7, 8 ,9}));
	}

	{
		ReservoirSamplerWeightedStatic<int, 5, bool> sampler;
		for (int i = 0; i < 100; ++i)
		{
			const bool weight = (i >= 5 && i < 10);
			sampler.addElement(weight, i);
		}

		std::vector<int> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		ASSERT_EQ(result, std::vector<int>({5, 6, 7, 8 ,9}));
	}
}

TEST(ReservoirSamplerWeightedStatic, SamplerSizeOfFive_SamplingFromStreamOfTwenty_ProducesEqualFrequencies)
{
	std::array<int, 20> frequences{};
	// reuse random to speed things up a bit
	std::mt19937 rand{std::random_device{}()};
	for (int i = 0; i < 10000; ++i)
	{
		// shift each iteration by one generated value
		rand.discard(1);
		ReservoirSamplerWeightedStatic<int, 5, int, std::mt19937> sampler(rand);

		for (int n = 0; n < 20; ++n)
		{
			sampler.addElement(1, n);
		}

		ReservoirSamplerWeightedStatic<int, 5, int, std::mt19937> samplerCopy(sampler);
		ReservoirSamplerWeightedStatic<int, 5, int, std::mt19937> samplerMoved(std::move(samplerCopy));

		const std::vector<int> result = samplerMoved.consumeResult();
		for (int value : result)
		{
			++frequences[value];
		}
	}

	const float frequencySum = std::accumulate(frequences.begin(), frequences.end(), 0.0f);
	ASSERT_EQ(5.0f * 10000, frequencySum);
	for (int freq : frequences)
	{
		EXPECT_NEAR(0.05f, freq/frequencySum, 0.01f);
	}
}

TEST(ReservoirSamplerWeightedStatic, SamplerSizeOfFive_SamplingFromStreamOfWeightedValues_ProducesExpectedFrequencies)
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
		// shift each iteration by one generated value
		rand.discard(1);
		ReservoirSamplerWeightedStatic<int, 5, int, std::mt19937> sampler(rand);

		for (size_t i = 0; i < elementsCount; ++i)
		{
			sampler.addElement(weights[i], i);
		}

		ReservoirSamplerWeightedStatic<int, 5, int> samplerCopy(sampler);
		ReservoirSamplerWeightedStatic<int, 5, int> samplerMoved(std::move(samplerCopy));

		const std::vector<int> result = samplerMoved.consumeResult();
		for (int value : result)
		{
			++frequences[value];
		}
	}

	const float frequencySum = std::accumulate(frequences.begin(), frequences.end(), 0.0f);
	for (size_t i = 0; i < elementsCount; ++i)
	{
		EXPECT_NEAR(expectedFrequencies[i], frequences[i]/frequencySum, 0.01f);
	}
}

TEST(ReservoirSamplerWeightedStatic, SamplersWithDifferentTypes_ConstructedFilledCopiedAndMoved_Compiles)
{
	{
		ReservoirSamplerWeightedStatic<int, 2, int> sampler;
		sampler.addElement(2, 10);
		sampler.emplaceElement(1, 20);
		sampler.addElement(3, 40);

		ReservoirSamplerWeightedStatic<int, 2, int> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerWeightedStatic<int, 2, int> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeightedStatic<std::string, 2, int> sampler;
		sampler.addElement(2, "test");
		sampler.emplaceElement(1, "test2");
		sampler.addElement(3, "test3");

		ReservoirSamplerWeightedStatic<std::string, 2, int> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerWeightedStatic<std::string, 2, int> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeightedStatic<ImplicitCtor, 2> sampler;
		ImplicitCtor v(2);
		sampler.addElement(4, v);
		sampler.addElement(2, ImplicitCtor(2));
		sampler.emplaceElement(1, 2);
		sampler.addElement(3, 2);

		ReservoirSamplerWeightedStatic<ImplicitCtor, 2> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerWeightedStatic<ImplicitCtor, 2> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeightedStatic<Simple, 2> sampler;
		Simple v(2);
		sampler.addElement(4, v);
		sampler.addElement(2, Simple(2));
		sampler.emplaceElement(1, 2);
		//sampler.addElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerWeightedStatic<Simple, 2> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerWeightedStatic<Simple, 2> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeightedStatic<TwoArgs, 2> sampler;
		TwoArgs v(2, 5.5f);
		sampler.addElement(4, v);
		sampler.addElement(2, TwoArgs(2, 9.0f));
		sampler.emplaceElement(1, 2, 3.5f);
		//sampler.addElement(3, 2); // won't compile, no matching constructor

		ReservoirSamplerWeightedStatic<TwoArgs, 2> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerWeightedStatic<TwoArgs, 2> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeightedStatic<NonCopyable, 2> sampler;
		//NonCopyable v(2);
		//sampler.addElement(4, v);	// won't compile, no copy constructor
		sampler.addElement(2, NonCopyable(2));
		sampler.emplaceElement(1, 2);
		//sampler.addElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerWeightedStatic<NonCopyable, 2> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeightedStatic<NonMovable, 2> sampler;
		NonMovable v(2);
		sampler.addElement(4, v);
		sampler.addElement(2, NonMovable(2));
		sampler.emplaceElement(1, 2);
		//sampler.addElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerWeightedStatic<NonMovable, 2> samplerCopy(sampler);
		(void)samplerCopy;
	}

	{
		ReservoirSamplerWeightedStatic<NonCopyableNonMovable, 2> sampler;
		//NonCopyableNonMovable v(2);
		//sampler.addElement(4, v); // won't compile, no copy constructor
		//sampler.addElement(2, NonCopyableNonMovable(2)); // won't compile, should be copy or move constructible
		sampler.emplaceElement(1, 2);
		//sampler.addElement(3, 2); // won't compile, explicit constructor
	}

	{
		ReservoirSamplerWeightedStatic<OnlyCopyConstructible, 2> sampler;
		OnlyCopyConstructible v(2);
		sampler.addElement(4, v);
		sampler.addElement(2, OnlyCopyConstructible(2));
		sampler.emplaceElement(1, 2);
		//sampler.addElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerWeightedStatic<OnlyCopyConstructible, 2> samplerCopy(sampler);
		(void)samplerCopy;
	}

	{
		ReservoirSamplerWeightedStatic<OnlyCopyAssignable, 2> sampler;
		//OnlyCopyAssignable v(2);
		//sampler.addElement(4, v); // won't compile, no copy constructor
		//sampler.addElement(2, OnlyCopyAssignable(2)); // won't compile, should be copy or move constructible
		sampler.emplaceElement(1, 2);
		//sampler.addElement(3, 2); // won't compile, explicit constructor
	}

	{
		ReservoirSamplerWeightedStatic<OnlyMoveConstructible, 2> sampler;
		//OnlyMoveConstructible v(2);
		//sampler.addElement(4, v); // won't compile, no copy constructor
		sampler.addElement(2, OnlyMoveConstructible(2));
		sampler.emplaceElement(1, 2);
		//sampler.addElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerWeightedStatic<OnlyMoveConstructible, 2> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeightedStatic<OnlyMoveAssignable, 2> sampler;
		//OnlyMoveAssignable v(2);
		//sampler.addElement(4, v); // won't compile, no copy constructor
		//sampler.addElement(2, OnlyMoveAssignable(2)); // won't compile, should be copy or move constructible
		sampler.emplaceElement(1, 2);
		//sampler.addElement(3, 2); // won't compile, explicit constructor
	}
}

TEST(ReservoirSamplerWeightedStatic, Sampler_ConstructedFilledAndConsumed_ProducesReasonableAmountOfMoves)
{
	constexpr size_t sampleSize = 5;
	const size_t streamSize = 500;

	CopyMoveCounter::Reset();

	ReservoirSamplerWeightedStatic<CopyMoveCounter, sampleSize> sampler;
	EXPECT_EQ(0, CopyMoveCounter::GetConstructionsCount());
	EXPECT_EQ(0, CopyMoveCounter::GetCopiesCount());
	EXPECT_EQ(0, CopyMoveCounter::GetMovesCount());

	for (size_t n = 0; n < streamSize; ++n)
	{
		sampler.emplaceElement(1.0f);
	}

	const int constructionsCount = CopyMoveCounter::GetConstructionsCount();
	const int movesCount = CopyMoveCounter::GetMovesCount();
	EXPECT_GT(50, constructionsCount);
	EXPECT_LT(static_cast<int>(sampleSize), constructionsCount);
	EXPECT_EQ(0, CopyMoveCounter::GetCopiesCount());
	EXPECT_EQ(static_cast<int>(sampleSize), constructionsCount - movesCount);

	auto [samples, count] = sampler.getResult();
	(void)samples; (void)count;

	EXPECT_EQ(constructionsCount, CopyMoveCounter::GetConstructionsCount());
	EXPECT_EQ(0, CopyMoveCounter::GetCopiesCount());
	EXPECT_EQ(movesCount, CopyMoveCounter::GetMovesCount());

	{
		std::vector<CopyMoveCounter> result = sampler.consumeResult();
		(void)result;
	}

	EXPECT_EQ(constructionsCount, CopyMoveCounter::GetConstructionsCount());
	EXPECT_EQ(0, CopyMoveCounter::GetCopiesCount());
	EXPECT_EQ(movesCount + static_cast<int>(sampleSize), CopyMoveCounter::GetMovesCount());
}
