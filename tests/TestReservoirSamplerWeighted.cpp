#include <gtest/gtest.h>

#include "reservoir-sampler/reservoir_sampler_weighted.h"

#include <array>
#include <numeric>

#include "TestTypes.h"

TEST(ReservoirSamplerWeighted, SamplersOfDifferentTypes_CreeateFillAndDestroy_DoNotCrash)
{
	{
		ReservoirSamplerWeighted<std::string> sampler(5);
		sampler.sampleElement(1.0f, "list");
		sampler.sampleElement(1.0f, "of");
		sampler.sampleElement(1.0f, "test");
		sampler.sampleElement(1.0f, "string");
		sampler.sampleElement(1.0f, "items");
	}

	{
		ReservoirSamplerWeighted<size_t> sampler(5);
		sampler.sampleElement(1.0f, 0);
		sampler.sampleElement(1.0f, 1);
		sampler.sampleElement(1.0f, 2);
		sampler.sampleElement(1.0f, 3);
		sampler.sampleElement(1.0f, 4);
	}

	{
		ReservoirSamplerWeighted<std::vector<int>> sampler(5);
		sampler.sampleElement(1.0f, std::vector<int>{{1, 2}});
		sampler.sampleElement(1.0f, std::vector<int>{{3, 4}});
		sampler.sampleElement(1.0f, std::vector<int>{{5, 6}});
		sampler.sampleElement(1.0f, std::vector<int>{{7, 8}});
		sampler.sampleElement(1.0f, std::vector<int>{{9, 10}});
	}

	{
		ReservoirSamplerWeighted<std::thread> sampler(5);
		sampler.sampleElementEmplace(1.0f);
		sampler.sampleElementEmplace(1.0f);
		sampler.sampleElementEmplace(1.0f);
		sampler.sampleElementEmplace(1.0f);
		sampler.sampleElementEmplace(1.0f);
	}
}

TEST(ReservoirSamplerWeighted, SamplersOfDifferentSizes_CreeateFillAndDestroy_DoNotCrash)
{
	{
		ReservoirSamplerWeighted<size_t> sampler(5);
		sampler.sampleElement(1.0f, 0);
		sampler.sampleElement(1.0f, 1);
		sampler.sampleElement(1.0f, 2);
		sampler.sampleElement(1.0f, 3);
		sampler.sampleElement(1.0f, 4);
	}

	{
		ReservoirSamplerWeighted<size_t> sampler(1);
		sampler.sampleElement(1.0f, 0);
		sampler.sampleElement(1.0f, 1);
		sampler.sampleElement(1.0f, 2);
		sampler.sampleElement(1.0f, 3);
		sampler.sampleElement(1.0f, 4);
	}

	{
		ReservoirSamplerWeighted<size_t> sampler(300);
		sampler.sampleElement(1.0f, 0);
		sampler.sampleElement(1.0f, 1);
		sampler.sampleElement(1.0f, 2);
		sampler.sampleElement(1.0f, 3);
		sampler.sampleElement(1.0f, 4);
	}

	{
		ReservoirSamplerWeighted<size_t> sampler(3);
		(void)sampler;
	}
}

TEST(ReservoirSamplerWeighted, SamplerOfSizeFive_FiveElementsAdded_HasOnlyOriginalElements)
{
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	ReservoirSamplerWeighted<size_t> sampler(5);
	for (const size_t value : stream)
	{
		sampler.sampleElement(1.0f, value);
	}

	{
		// we can use range-based loop to iterate over the resulting data
		std::vector<size_t> result;
		result.reserve(stream.size());
		for (size_t item : sampler.getResult())
		{
			result.push_back(item);
		}
		std::sort(result.begin(), result.end());
		EXPECT_EQ(stream, result);
	}

	{
		// we can get raw pointer and size to iterate over them as C-array
		const auto [data, size] = sampler.getResult();
		std::vector<size_t> result;
		result.reserve(size);
		for (size_t	i = 0; i < size; ++i)
		{
			result.push_back(data[i]);
		}
		std::sort(result.begin(), result.end());
		EXPECT_EQ(stream, result);
	}

	{
		// we can construct a vector from the C-array by copying the data into it
		const auto [data, size] = sampler.getResult();
		std::vector<size_t> result(data, data + size);
		std::sort(result.begin(), result.end());
		EXPECT_EQ(stream, result);
	}

	{
		// we can move data into a new vector
		std::vector<size_t> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		EXPECT_EQ(stream, result);
	}
}

TEST(ReservoirSamplerWeighted, SamplerOfSizeFive_ConsumeResultTo_ConsumedResultIsCorrect)
{
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	ReservoirSamplerWeighted<size_t> sampler(5);
	for (const size_t value : stream)
	{
		sampler.sampleElement(1.0f, value);
	}

	// we can move data into an existing C-array
	size_t result[5];
	const size_t resultSize = sampler.getResultSize();
	ASSERT_EQ(static_cast<size_t>(5), resultSize);
	sampler.consumeResultTo(result);
	std::sort(result, result + resultSize);
	EXPECT_EQ(stream[0], result[0]);
	EXPECT_EQ(stream[1], result[1]);
	EXPECT_EQ(stream[2], result[2]);
	EXPECT_EQ(stream[3], result[3]);
	EXPECT_EQ(stream[4], result[4]);
}

TEST(ReservoirSamplerWeighted, SamplerOfSizeFive_ConsumeResultTo_SamplerCanBeReused)
{
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	ReservoirSamplerWeighted<size_t> sampler(5);
	for (const size_t value : stream)
	{
		sampler.sampleElement(1.0f, value);
	}

	{
		size_t result[5];
		sampler.consumeResultTo(result);
	}

	EXPECT_EQ(static_cast<size_t>(0), sampler.getResultSize());
	for (const size_t value : stream)
	{
		sampler.sampleElement(1.0f, value);
	}

	std::vector<size_t> result = sampler.consumeResult();
	std::sort(result.begin(), result.end());
	EXPECT_EQ(stream, result);
}

TEST(ReservoirSamplerWeighted, SamplerOfSizeFive_ThreeElementsAdded_HasOnlyOriginalElements)
{
	const std::vector<size_t> stream({10, 11, 12});

	ReservoirSamplerWeighted<size_t> sampler(5);
	for (const size_t value : stream)
	{
		sampler.sampleElement(1.0f, value);
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

TEST(ReservoirSamplerWeighted, EmptySampler_Reset_DoesNotCrash)
{
	ReservoirSamplerWeighted<size_t> sampler(5);
	sampler.reset();
}

TEST(ReservoirSamplerWeighted, SamplerWithAResult_Reset_CanBeReused)
{
	ReservoirSamplerWeighted<size_t> sampler(5);
	const std::vector<size_t> stream1({10, 11, 12, 13, 14});
	const std::vector<size_t> stream2({15, 16, 17, 18, 19});

	for (const size_t value : stream1)
	{
		sampler.sampleElement(1.0f, value);
	}

	sampler.reset();

	for (const size_t value : stream2)
	{
		sampler.sampleElement(1.0f, value);
	}

	{
		std::vector<size_t> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		EXPECT_EQ(result, stream2);
	}
}

TEST(ReservoirSamplerWeighted, SamplerWithAResult_Consume_CanBeReused)
{
	ReservoirSamplerWeighted<size_t> sampler(5);
	const std::vector<size_t> stream1({10, 11, 12, 13, 14});
	const std::vector<size_t> stream2({15, 16, 17, 18, 19});

	for (const size_t value : stream1)
	{
		sampler.sampleElement(1.0f, value);
	}

	{
		std::vector<size_t> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		ASSERT_EQ(result, stream1);
	}

	for (const size_t value : stream2)
	{
		sampler.sampleElement(1.0f, value);
	}

	{
		std::vector<size_t> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		EXPECT_EQ(result, stream2);
	}
}

TEST(ReservoirSamplerWeighted, Sampler_PreallocateData_ProducesExpectedResult)
{
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	ReservoirSamplerWeighted<size_t> sampler(5);
	sampler.allocateData();

	for (const size_t value : stream)
	{
		sampler.sampleElement(1.0f, value);
	}

	std::vector<size_t> result = sampler.consumeResult();
	std::sort(result.begin(), result.end());
	EXPECT_EQ(result, stream);
}

TEST(ReservoirSamplerWeighted, EmptySampler_Copied_DoesNotCrash)
{
	ReservoirSamplerWeighted<size_t> sampler(5);
	ReservoirSamplerWeighted<size_t> copySampler(sampler);
	(void)copySampler;
}

TEST(ReservoirSamplerWeighted, Sampler_Copied_HoldsTheData)
{
	ReservoirSamplerWeighted<size_t> sampler(5);
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	for (const size_t value : stream)
	{
		sampler.sampleElement(1.0f, value);
	}

	ReservoirSamplerWeighted<size_t> samplerCopy(sampler);

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

TEST(ReservoirSamplerWeighted, EmptySampler_Moved_DoesNotCrash)
{
	ReservoirSamplerWeighted<size_t> sampler(5);
	ReservoirSamplerWeighted<size_t> copySampler(std::move(sampler));
	(void)copySampler;
}

TEST(ReservoirSamplerWeighted, Sampler_Moved_ValueIsMoved)
{
	ReservoirSamplerWeighted<size_t> sampler(5);
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	for (const size_t value : stream)
	{
		sampler.sampleElement(1.0f, value);
	}

	ReservoirSamplerWeighted<size_t> samplerMovedTo(std::move(sampler));

	{
		std::vector<size_t> result = samplerMovedTo.consumeResult();
		std::sort(result.begin(), result.end());
		EXPECT_EQ(result, stream);
	}
}

TEST(ReservoirSamplerWeighted, Sampler_Moved_OldSamplerCanBeReused)
{
	ReservoirSamplerWeighted<size_t> sampler(5);
	const std::vector<size_t> stream1({10, 11, 12, 13, 14});
	const std::vector<size_t> stream2({15, 16, 17, 18, 19});

	for (const size_t value : stream1)
	{
		sampler.sampleElement(1.0f, value);
	}

	{
		ReservoirSamplerWeighted<size_t> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	for (const size_t value : stream2)
	{
		sampler.sampleElement(1.0f, value);
	}

	{
		std::vector<size_t> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		EXPECT_EQ(result, stream2);
	}
}

TEST(ReservoirSamplerWeighted, SamplersWithDifferentWeightTypes_FilledWithData_ProduceExpectedResults)
{
	{
		ReservoirSamplerWeighted<int, float> sampler(5);
		for (int i = 0; i < 100; ++i)
		{
			const float weight = (i >= 5 && i < 10) ? 1.0f : 0.0f;
			sampler.sampleElement(weight, i);
		}

		std::vector<int> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		ASSERT_EQ(result, std::vector<int>({5, 6, 7, 8 ,9}));
	}

	{
		ReservoirSamplerWeighted<int, double> sampler(5);
		for (int i = 0; i < 100; ++i)
		{
			const double weight = (i >= 5 && i < 10) ? 1.0 : 0.0;
			sampler.sampleElement(weight, i);
		}

		std::vector<int> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		ASSERT_EQ(result, std::vector<int>({5, 6, 7, 8 ,9}));
	}

	{
		ReservoirSamplerWeighted<int, int> sampler(5);
		for (int i = 0; i < 100; ++i)
		{
			const int weight = (i >= 5 && i < 10) ? 1 : 0;
			sampler.sampleElement(weight, i);
		}

		std::vector<int> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		ASSERT_EQ(result, std::vector<int>({5, 6, 7, 8 ,9}));
	}

	{
		ReservoirSamplerWeighted<int, char> sampler(5);
		for (int i = 0; i < 100; ++i)
		{
			const char weight = (i >= 5 && i < 10) ? 1 : 0;
			sampler.sampleElement(weight, i);
		}

		std::vector<int> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		ASSERT_EQ(result, std::vector<int>({5, 6, 7, 8 ,9}));
	}

	{
		ReservoirSamplerWeighted<int, bool> sampler(5);
		for (int i = 0; i < 100; ++i)
		{
			const bool weight = (i >= 5 && i < 10);
			sampler.sampleElement(weight, i);
		}

		std::vector<int> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		ASSERT_EQ(result, std::vector<int>({5, 6, 7, 8 ,9}));
	}
}

TEST(ReservoirSamplerWeighted, SamplerSizeOfFive_SamplingFromStreamOfTwenty_ProducesEqualFrequencies)
{
	std::array<int, 20> frequences{};
	// reuse random to speed things up a bit
	std::mt19937 rand{std::random_device{}()};
	for (int i = 0; i < 10000; ++i)
	{
		ReservoirSamplerWeighted<int, int, std::mt19937&> sampler(5, rand);

		for (int n = 0; n < 20; ++n)
		{
			sampler.sampleElement(1, n);
		}

		ReservoirSamplerWeighted<int, int, std::mt19937&> samplerCopy(sampler);
		ReservoirSamplerWeighted<int, int, std::mt19937&> samplerMoved(std::move(samplerCopy));

		const auto [data, size] = samplerMoved.getResult();
		for (size_t k = 0; k < size; ++k)
		{
			++frequences[data[k]];
		}
	}

	const float frequencySum = std::accumulate(frequences.begin(), frequences.end(), 0.0f);
	ASSERT_EQ(5.0f * 10000, frequencySum);
	for (int freq : frequences)
	{
		EXPECT_NEAR(0.05f, freq/frequencySum, 0.01f);
	}
}

TEST(ReservoirSamplerWeighted, Sampler_AddingWhenWillBeConsidered_ProducesEqualFrequencies)
{
	std::array<int, 20> frequences{};
	// reuse random to speed things up a bit
	std::mt19937 rand{std::random_device{}()};
	for (int i = 0; i < 10000; ++i)
	{
		ReservoirSamplerWeighted<int, int, std::mt19937&> sampler(5, rand);

		for (int n = 0; n < 20; ++n)
		{
			if (sampler.willNextElementBeConsidered(1))
			{
				sampler.sampleElement(1, n);
			}
			else
			{
				sampler.skipNextElement(1);
			}
		}

		ReservoirSamplerWeighted<int, int, std::mt19937&> samplerCopy(sampler);
		ReservoirSamplerWeighted<int, int, std::mt19937&> samplerMoved(std::move(samplerCopy));

		const auto [data, size] = samplerMoved.getResult();
		for (size_t k = 0; k < size; ++k)
		{
			++frequences[data[k]];
		}
	}

	const float frequencySum = std::accumulate(frequences.begin(), frequences.end(), 0.0f);
	ASSERT_EQ(5.0f * 10000, frequencySum);
	for (int freq : frequences)
	{
		EXPECT_NEAR(0.05f, freq/frequencySum, 0.01f);
	}
}

TEST(ReservoirSamplerWeighted, SamplerSizeOfFive_SamplingFromStreamOfWeightedValues_ProducesExpectedFrequencies)
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
		ReservoirSamplerWeighted<size_t, int, std::mt19937&> sampler(5, rand);

		for (size_t n = 0; n < elementsCount; ++n)
		{
			sampler.sampleElement(weights[n], n);
		}

		ReservoirSamplerWeighted<size_t, int, std::mt19937&> samplerCopy(sampler);
		ReservoirSamplerWeighted<size_t, int, std::mt19937&> samplerMoved(std::move(samplerCopy));

		const auto [data, size] = samplerMoved.getResult();
		for (size_t k = 0; k < size; ++k)
		{
			++frequences[data[k]];
		}
	}

	const float frequencySum = std::accumulate(frequences.begin(), frequences.end(), 0.0f);
	for (size_t i = 0; i < elementsCount; ++i)
	{
		EXPECT_NEAR(expectedFrequencies[i], frequences[i]/frequencySum, 0.01f);
	}
}

TEST(ReservoirSamplerWeighted, SamplersWithDifferentTypes_ConstructedFilledCopiedAndMoved_Compiles)
{
	{
		ReservoirSamplerWeighted<int, int> sampler(2);
		sampler.sampleElement(2, 10);
		sampler.sampleElementEmplace(1, 20);
		sampler.sampleElement(3, 40);

		ReservoirSamplerWeighted<int, int> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerWeighted<int, int> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeighted<std::string, int> sampler(2);
		sampler.sampleElement(2, "test");
		sampler.sampleElementEmplace(1, "test2");
		sampler.sampleElement(3, "test3");

		ReservoirSamplerWeighted<std::string, int> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerWeighted<std::string, int> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeighted<ImplicitCtor> sampler(2);
		ImplicitCtor v(2);
		sampler.sampleElement(4, v);
		sampler.sampleElement(2, ImplicitCtor(2));
		sampler.sampleElementEmplace(1, 2);
		sampler.sampleElement(3, 2);

		ReservoirSamplerWeighted<ImplicitCtor> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerWeighted<ImplicitCtor> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeighted<Simple> sampler(2);
		Simple v(2);
		sampler.sampleElement(4, v);
		sampler.sampleElement(2, Simple(2));
		sampler.sampleElementEmplace(1, 2);
		//sampler.sampleElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerWeighted<Simple> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerWeighted<Simple> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeighted<TwoArgs> sampler(2);
		TwoArgs v(2, 5.5f);
		sampler.sampleElement(4, v);
		sampler.sampleElement(2, TwoArgs(2, 9.0f));
		sampler.sampleElementEmplace(1, 2, 3.5f);
		//sampler.sampleElement(3, 2); // won't compile, no matching constructor

		ReservoirSamplerWeighted<TwoArgs> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerWeighted<TwoArgs> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeighted<NonCopyable> sampler(2);
		//NonCopyable v(2);
		//sampler.sampleElement(4, v);	// won't compile, no copy constructor
		sampler.sampleElement(2, NonCopyable(2));
		sampler.sampleElementEmplace(1, 2);
		//sampler.sampleElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerWeighted<NonCopyable> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeighted<NonMovable> sampler(2);
		NonMovable v(2);
		sampler.sampleElement(4, v);
		sampler.sampleElement(2, NonMovable(2));
		sampler.sampleElementEmplace(1, 2);
		//sampler.sampleElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerWeighted<NonMovable> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerWeighted<NonMovable> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeighted<NonCopyableNonMovable> sampler(2);
		//NonCopyableNonMovable v(2);
		//sampler.sampleElement(4, v); // won't compile, no copy constructor
		//sampler.sampleElement(2, NonCopyableNonMovable(2)); // won't compile, should be copy or move constructible
		sampler.sampleElementEmplace(1, 2);
		//sampler.sampleElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerWeighted<NonCopyableNonMovable> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeighted<OnlyCopyConstructible> sampler(2);
		OnlyCopyConstructible v(2);
		sampler.sampleElement(4, v);
		sampler.sampleElement(2, OnlyCopyConstructible(2));
		sampler.sampleElementEmplace(1, 2);
		//sampler.sampleElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerWeighted<OnlyCopyConstructible> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerWeighted<OnlyCopyConstructible> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeighted<OnlyCopyAssignable> sampler(2);
		//OnlyCopyAssignable v(2);
		//sampler.sampleElement(4, v); // won't compile, no copy constructor
		//sampler.sampleElement(2, OnlyCopyAssignable(2)); // won't compile, should be copy or move constructible
		sampler.sampleElementEmplace(1, 2);
		//sampler.sampleElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerWeighted<OnlyCopyAssignable> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeighted<OnlyMoveConstructible> sampler(2);
		//OnlyMoveConstructible v(2);
		//sampler.sampleElement(4, v); // won't compile, no copy constructor
		sampler.sampleElement(2, OnlyMoveConstructible(2));
		sampler.sampleElementEmplace(1, 2);
		//sampler.sampleElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerWeighted<OnlyMoveConstructible> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerWeighted<OnlyMoveAssignable> sampler(2);
		//OnlyMoveAssignable v(2);
		//sampler.sampleElement(4, v); // won't compile, no copy constructor
		//sampler.sampleElement(2, OnlyMoveAssignable(2)); // won't compile, should be copy or move constructible
		sampler.sampleElementEmplace(1, 2);
		//sampler.sampleElement(3, 2); // won't compile, explicit constructor

		ReservoirSamplerWeighted<OnlyMoveAssignable> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}
}

TEST(ReservoirSamplerWeighted, Sampler_ConstructedFilledAndConsumed_ProducesReasonableAmountOfMoves)
{
	const size_t sampleSize = 5;
	const size_t streamSize = 500;

	CopyMoveCounter::Reset();

	ReservoirSamplerWeighted<CopyMoveCounter> sampler(sampleSize);
	EXPECT_EQ(0, CopyMoveCounter::GetConstructionsCount());
	EXPECT_EQ(0, CopyMoveCounter::GetCopiesCount());
	EXPECT_EQ(0, CopyMoveCounter::GetMovesCount());

	for (size_t n = 0; n < streamSize; ++n)
	{
		sampler.sampleElementEmplace(1.0f);
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
