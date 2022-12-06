#include <gtest/gtest.h>

#include "reservoir-sampler/reservoir_sampler.h"

#include <array>
#include <numeric>

#include "TestTypes.h"

TEST(ReservoirSampler, SamplersOfDifferentTypes_CreeateFillAndDestroy_DoNotCrash)
{
	{
		ReservoirSampler<std::string> sampler(5);
		sampler.sampleElement("list");
		sampler.sampleElement("of");
		sampler.sampleElement("test");
		sampler.sampleElement("string");
		sampler.sampleElement("items");
	}

	{
		ReservoirSampler<size_t> sampler(5);
		sampler.sampleElement(0);
		sampler.sampleElement(1);
		sampler.sampleElement(2);
		sampler.sampleElement(3);
		sampler.sampleElement(4);
	}

	{
		ReservoirSampler<std::vector<int>> sampler(5);
		sampler.sampleElement(std::vector<int>{{1, 2}});
		sampler.sampleElement(std::vector<int>{{3, 4}});
		sampler.sampleElement(std::vector<int>{{5, 6}});
		sampler.sampleElement(std::vector<int>{{7, 8}});
		sampler.sampleElement(std::vector<int>{{9, 10}});
	}

	{
		ReservoirSampler<std::thread> sampler(5);
		sampler.sampleElementEmplace();
		sampler.sampleElementEmplace();
		sampler.sampleElementEmplace();
		sampler.sampleElementEmplace();
		sampler.sampleElementEmplace();
	}
}

TEST(ReservoirSampler, SamplersOfDifferentSizes_CreeateFillAndDestroy_DoNotCrash)
{
	{
		ReservoirSampler<size_t> sampler(5);
		sampler.sampleElement(0);
		sampler.sampleElement(1);
		sampler.sampleElement(2);
		sampler.sampleElement(3);
		sampler.sampleElement(4);
	}

	{
		ReservoirSampler<size_t> sampler(1);
		sampler.sampleElement(0);
		sampler.sampleElement(1);
		sampler.sampleElement(2);
		sampler.sampleElement(3);
		sampler.sampleElement(4);
	}

	{
		ReservoirSampler<size_t> sampler(300);
		sampler.sampleElement(0);
		sampler.sampleElement(1);
		sampler.sampleElement(2);
		sampler.sampleElement(3);
		sampler.sampleElement(4);
	}

	{
		ReservoirSampler<size_t> sampler(3);
		(void)sampler;
	}
}

TEST(ReservoirSampler, SamplerOfSizeFive_FiveElementsAdded_HasOnlyOriginalElements)
{
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	ReservoirSampler<size_t> sampler(5);
	for (const size_t value : stream)
	{
		sampler.sampleElement(value);
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

TEST(ReservoirSampler, SamplerOfSizeFive_ConsumeResultTo_ConsumedResultIsCorrect)
{
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	ReservoirSampler<size_t> sampler(5);
	for (const size_t value : stream)
	{
		sampler.sampleElement(value);
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

TEST(ReservoirSampler, SamplerOfSizeFive_ConsumeResultTo_SamplerCanBeReused)
{
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	ReservoirSampler<size_t> sampler(5);
	for (const size_t value : stream)
	{
		sampler.sampleElement(value);
	}

	{
		size_t result[5];
		sampler.consumeResultTo(result);
	}

	EXPECT_EQ(static_cast<size_t>(0), sampler.getResultSize());
	for (const size_t value : stream)
	{
		sampler.sampleElement(value);
	}

	std::vector<size_t> result = sampler.consumeResult();
	std::sort(result.begin(), result.end());
	EXPECT_EQ(stream, result);
}

TEST(ReservoirSampler, SamplerOfSizeFive_ThreeElementsAdded_HasOnlyOriginalElements)
{
	const std::vector<size_t> stream({10, 11, 12});

	ReservoirSampler<size_t> sampler(5);
	for (const size_t value : stream)
	{
		sampler.sampleElement(value);
	}

	{
		std::vector<size_t> result1;
		result1.reserve(stream.size());
		for (size_t item : sampler.getResult())
		{
			result1.push_back(item);
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

TEST(ReservoirSampler, EmptySampler_Reset_DoesNotCrash)
{
	ReservoirSampler<size_t> sampler(5);
	sampler.reset();
}

TEST(ReservoirSampler, SamplerWithAResult_Reset_CanBeReused)
{
	ReservoirSampler<size_t> sampler(5);
	const std::vector<size_t> stream1({10, 11, 12, 13, 14});
	const std::vector<size_t> stream2({15, 16, 17, 18, 19});

	for (const size_t value : stream1)
	{
		sampler.sampleElement(value);
	}

	sampler.reset();

	for (const size_t value : stream2)
	{
		sampler.sampleElement(value);
	}

	{
		std::vector<size_t> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		EXPECT_EQ(result, stream2);
	}
}

TEST(ReservoirSampler, SamplerWithAResult_Consume_CanBeReused)
{
	ReservoirSampler<size_t> sampler(5);
	const std::vector<size_t> stream1({10, 11, 12, 13, 14});
	const std::vector<size_t> stream2({15, 16, 17, 18, 19});

	for (const size_t value : stream1)
	{
		sampler.sampleElement(value);
	}

	{
		std::vector<size_t> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		ASSERT_EQ(result, stream1);
	}

	for (const size_t value : stream2)
	{
		sampler.sampleElement(value);
	}

	{
		std::vector<size_t> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		EXPECT_EQ(result, stream2);
	}
}

TEST(ReservoirSampler, Sampler_PreallocateData_ProducesExpectedResult)
{
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	ReservoirSampler<size_t> sampler(5);
	sampler.allocateData();

	for (const size_t value : stream)
	{
		sampler.sampleElement(value);
	}

	std::vector<size_t> result = sampler.consumeResult();
	std::sort(result.begin(), result.end());
	EXPECT_EQ(result, stream);
}

TEST(ReservoirSampler, EmptySampler_Copied_DoesNotCrash)
{
	ReservoirSampler<size_t> sampler(5);
	ReservoirSampler<size_t> copySampler(sampler);
	(void)copySampler;
}

TEST(ReservoirSampler, Sampler_Copied_HoldsTheData)
{
	ReservoirSampler<size_t> sampler(5);
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	for (const size_t value : stream)
	{
		sampler.sampleElement(value);
	}

	ReservoirSampler<size_t> samplerCopy(sampler);

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

TEST(ReservoirSampler, EmptySampler_Moved_DoesNotCrash)
{
	ReservoirSampler<size_t> sampler(5);
	ReservoirSampler<size_t> copySampler(std::move(sampler));
	(void)copySampler;
}

TEST(ReservoirSampler, Sampler_Moved_ValueIsMoved)
{
	ReservoirSampler<size_t> sampler(5);
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	for (const size_t value : stream)
	{
		sampler.sampleElement(value);
	}

	ReservoirSampler<size_t> samplerMovedTo(std::move(sampler));

	{
		std::vector<size_t> result = samplerMovedTo.consumeResult();
		std::sort(result.begin(), result.end());
		EXPECT_EQ(result, stream);
	}
}

TEST(ReservoirSampler, Sampler_Moved_OldSamplerCanBeReused)
{
	ReservoirSampler<size_t> sampler(5);
	const std::vector<size_t> stream1({10, 11, 12, 13, 14});
	const std::vector<size_t> stream2({15, 16, 17, 18, 19});

	for (const size_t value : stream1)
	{
		sampler.sampleElement(value);
	}

	{
		ReservoirSampler<size_t> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	for (const size_t value : stream2)
	{
		sampler.sampleElement(value);
	}

	{
		std::vector<size_t> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		EXPECT_EQ(result, stream2);
	}
}

TEST(ReservoirSampler, SamplerSizeOfFive_SamplingFromStreamOfTwenty_ProducesEqualFrequencies)
{
	std::array<int, 20> frequences{};
	// reuse random to speed things up a bit
	std::mt19937 rand{std::random_device{}()};
	for (int i = 0; i < 10000; ++i)
	{
		ReservoirSampler<int, std::mt19937&> sampler(5, rand);

		for (int n = 0; n < 20; ++n)
		{
			sampler.sampleElement(n);
		}

		ReservoirSampler<int, std::mt19937&> samplerCopy(sampler);
		ReservoirSampler<int, std::mt19937&> samplerMoved(std::move(samplerCopy));

		for (int item : samplerMoved.getResult())
		{
			++frequences[item];
		}
	}

	const float frequencySum = std::accumulate(frequences.begin(), frequences.end(), 0.0f);
	ASSERT_EQ(5.0f * 10000, frequencySum);
	for (int freq : frequences)
	{
		EXPECT_NEAR(0.05f, freq/frequencySum, 0.01f);
	}
}

TEST(ReservoirSampler, Sampler_AddingWhenWillBeConsidered_ProducesEqualFrequencies)
{
	std::array<int, 20> frequences{};
	// reuse random to speed things up a bit
	std::mt19937 rand{std::random_device{}()};
	for (int i = 0; i < 10000; ++i)
	{
		ReservoirSampler<int, std::mt19937&> sampler(5, rand);

		for (int n = 0; n < 20; ++n)
		{
			if (sampler.willNextElementBeConsidered())
			{
				sampler.sampleElement(n);
			}
			else
			{
				sampler.skipNextElement();
			}
		}

		ReservoirSampler<int, std::mt19937&> samplerCopy(sampler);
		ReservoirSampler<int, std::mt19937&> samplerMoved(std::move(samplerCopy));

		for (int item : samplerMoved.getResult())
		{
			++frequences[item];
		}
	}

	const float frequencySum = std::accumulate(frequences.begin(), frequences.end(), 0.0f);
	ASSERT_EQ(5.0f * 10000, frequencySum);
	for (int freq : frequences)
	{
		EXPECT_NEAR(0.05f, freq/frequencySum, 0.01f);
	}
}

TEST(ReservoirSampler, Sampler_JumpAheadWhenAdding_ProducesEqualFrequencies)
{
	std::array<int, 20> frequences{};
	// reuse random to speed things up a bit
	std::mt19937 rand{std::random_device{}()};
	for (int i = 0; i < 10000; ++i)
	{
		ReservoirSampler<int, std::mt19937&> sampler(5, rand);

		for (int n = 0; n < 20; ++n)
		{
			sampler.sampleElement(n);
			n += static_cast<int>(sampler.getNextSkippedElementsCount());
			sampler.jumpAhead(sampler.getNextSkippedElementsCount());
		}

		ReservoirSampler<int, std::mt19937&> samplerCopy(sampler);
		ReservoirSampler<int, std::mt19937&> samplerMoved(std::move(samplerCopy));

		for (int item : samplerMoved.getResult())
		{
			++frequences[item];
		}
	}

	const float frequencySum = std::accumulate(frequences.begin(), frequences.end(), 0.0f);
	ASSERT_EQ(5.0f * 10000, frequencySum);
	for (int freq : frequences)
	{
		EXPECT_NEAR(0.05f, freq/frequencySum, 0.01f);
	}
}

TEST(ReservoirSampler, SamplersWithDifferentTypes_ConstructedFilledCopiedAndMoved_Compiles)
{
	{
		ReservoirSampler<int> sampler(2);
		sampler.sampleElement(10);
		sampler.sampleElementEmplace(20);
		sampler.sampleElement(40);

		ReservoirSampler<int> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSampler<int> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<std::string> sampler(2);
		sampler.sampleElement("test");
		sampler.sampleElementEmplace("test2");
		sampler.sampleElement("test3");

		ReservoirSampler<std::string> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSampler<std::string> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<ImplicitCtor> sampler(2);
		ImplicitCtor v(2);
		sampler.sampleElement(v);
		sampler.sampleElement(ImplicitCtor(2));
		sampler.sampleElementEmplace(2);
		sampler.sampleElement(2);

		ReservoirSampler<ImplicitCtor> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSampler<ImplicitCtor> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<Simple> sampler(2);
		Simple v(2);
		sampler.sampleElement(v);
		sampler.sampleElement(Simple(2));
		sampler.sampleElementEmplace(2);
		//sampler.sampleElement(2); // won't compile, explicit constructor

		ReservoirSampler<Simple> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSampler<Simple> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<TwoArgs> sampler(2);
		TwoArgs v(2, 5.5f);
		sampler.sampleElement(v);
		sampler.sampleElement(TwoArgs(2, 9.0f));
		sampler.sampleElementEmplace(2, 3.5f);
		//sampler.sampleElement(2); // won't compile, no matching constructor

		ReservoirSampler<TwoArgs> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSampler<TwoArgs> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<NonCopyable> sampler(2);
		//NonCopyable v(2);
		//sampler.sampleElement(v);	// won't compile, no copy constructor
		sampler.sampleElement(NonCopyable(2));
		sampler.sampleElementEmplace(2);
		//sampler.sampleElement(2); // won't compile, explicit constructor

		ReservoirSampler<NonCopyable> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<NonMovable> sampler(2);
		NonMovable v(2);
		sampler.sampleElement(v);
		sampler.sampleElement(NonMovable(2));
		sampler.sampleElementEmplace(2);
		//sampler.sampleElement(2); // won't compile, explicit constructor

		ReservoirSampler<NonMovable> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSampler<NonMovable> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<NonCopyableNonMovable> sampler(2);
		//NonCopyableNonMovable v(2);
		//sampler.sampleElement(v); // won't compile, no copy constructor
		//sampler.sampleElement(NonCopyableNonMovable(2)); // won't compile, should be copy or move constructible
		sampler.sampleElementEmplace(2);
		//sampler.sampleElement(2); // won't compile, explicit constructor

		ReservoirSampler<NonCopyableNonMovable> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<OnlyCopyConstructible> sampler(2);
		OnlyCopyConstructible v(2);
		sampler.sampleElement(v);
		sampler.sampleElement(OnlyCopyConstructible(2));
		sampler.sampleElementEmplace(2);
		//sampler.sampleElement(2); // won't compile, explicit constructor

		ReservoirSampler<OnlyCopyConstructible> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSampler<OnlyCopyConstructible> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<OnlyCopyAssignable> sampler(2);
		//OnlyCopyAssignable v(2);
		//sampler.sampleElement(v); // won't compile, no copy constructor
		//sampler.sampleElement(OnlyCopyAssignable(2)); // won't compile, should be copy or move constructible
		sampler.sampleElementEmplace(2);
		//sampler.sampleElement(2); // won't compile, explicit constructor

		ReservoirSampler<OnlyCopyAssignable> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<OnlyMoveConstructible> sampler(2);
		//OnlyMoveConstructible v(2);
		//sampler.sampleElement(v); // won't compile, no copy constructor
		sampler.sampleElement(OnlyMoveConstructible(2));
		sampler.sampleElementEmplace(2);
		//sampler.sampleElement(2); // won't compile, explicit constructor

		ReservoirSampler<OnlyMoveConstructible> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<OnlyMoveAssignable> sampler(2);
		//OnlyMoveAssignable v(2);
		//sampler.sampleElement(v); // won't compile, no copy constructor
		//sampler.sampleElement(OnlyMoveAssignable(2)); // won't compile, should be copy or move constructible
		sampler.sampleElementEmplace(2);
		//sampler.sampleElement(2); // won't compile, explicit constructor

		ReservoirSampler<OnlyMoveAssignable> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}
}

TEST(ReservoirSampler, Sampler_ConstructedFilledAndConsumed_ProducesReasonableAmountOfMoves)
{
	const size_t sampleSize = 5;
	const size_t streamSize = 500;

	CopyMoveCounter::Reset();

	ReservoirSampler<CopyMoveCounter> sampler(sampleSize);
	EXPECT_EQ(0, CopyMoveCounter::GetConstructionsCount());
	EXPECT_EQ(0, CopyMoveCounter::GetCopiesCount());
	EXPECT_EQ(0, CopyMoveCounter::GetMovesCount());

	for (size_t n = 0; n < streamSize; ++n)
	{
		sampler.sampleElementEmplace();
	}

	const int constructionsCount = CopyMoveCounter::GetConstructionsCount();
	const int movesCount = CopyMoveCounter::GetMovesCount();
	EXPECT_GT(50, constructionsCount);
	EXPECT_LT(static_cast<int>(sampleSize), constructionsCount);
	EXPECT_EQ(0, CopyMoveCounter::GetCopiesCount());
	EXPECT_EQ(static_cast<int>(sampleSize), constructionsCount - movesCount);

	{
		auto result = sampler.getResult();
		(void)result;
	}

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
