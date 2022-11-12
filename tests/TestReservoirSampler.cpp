#include <gtest/gtest.h>

#include "reservoir-sampler/reservoir_sampler.h"

#include <array>
#include <numeric>

#include "TestTypes.h"

TEST(ReservoirSampler, SamplersOfDifferentTypes_CreeateFillAndDestroy_DoNotCrash)
{
	{
		ReservoirSampler<std::string> sampler(5);
		sampler.addElement("list");
		sampler.addElement("of");
		sampler.addElement("test");
		sampler.addElement("string");
		sampler.addElement("items");
	}

	{
		ReservoirSampler<size_t> sampler(5);
		sampler.addElement(0);
		sampler.addElement(1);
		sampler.addElement(2);
		sampler.addElement(3);
		sampler.addElement(4);
	}

	{
		ReservoirSampler<std::vector<int>> sampler(5);
		sampler.addElement(std::vector<int>{{1, 2}});
		sampler.addElement(std::vector<int>{{3, 4}});
		sampler.addElement(std::vector<int>{{5, 6}});
		sampler.addElement(std::vector<int>{{7, 8}});
		sampler.addElement(std::vector<int>{{9, 10}});
	}

	{
		ReservoirSampler<std::thread> sampler(5);
		sampler.emplaceElement();
		sampler.emplaceElement();
		sampler.emplaceElement();
		sampler.emplaceElement();
		sampler.emplaceElement();
	}
}

TEST(ReservoirSampler, SamplersOfDifferentSizes_CreeateFillAndDestroy_DoNotCrash)
{
	{
		ReservoirSampler<size_t> sampler(5);
		sampler.addElement(0);
		sampler.addElement(1);
		sampler.addElement(2);
		sampler.addElement(3);
		sampler.addElement(4);
	}

	{
		ReservoirSampler<size_t> sampler(1);
		sampler.addElement(0);
		sampler.addElement(1);
		sampler.addElement(2);
		sampler.addElement(3);
		sampler.addElement(4);
	}

	{
		ReservoirSampler<size_t> sampler(300);
		sampler.addElement(0);
		sampler.addElement(1);
		sampler.addElement(2);
		sampler.addElement(3);
		sampler.addElement(4);
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
		sampler.addElement(value);
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

TEST(ReservoirSampler, SamplerOfSizeFive_ThreeElementsAdded_HasOnlyOriginalElements)
{
	const std::vector<size_t> stream({10, 11, 12});

	ReservoirSampler<size_t> sampler(5);
	for (const size_t value : stream)
	{
		sampler.addElement(value);
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
		sampler.addElement(value);
	}

	sampler.reset();

	for (const size_t value : stream2)
	{
		sampler.addElement(value);
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
		sampler.addElement(value);
	}

	{
		std::vector<size_t> result = sampler.consumeResult();
		std::sort(result.begin(), result.end());
		ASSERT_EQ(result, stream1);
	}

	for (const size_t value : stream2)
	{
		sampler.addElement(value);
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
		sampler.addElement(value);
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
		sampler.addElement(value);
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
		sampler.addElement(value);
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
		sampler.addElement(value);
	}

	{
		ReservoirSampler<size_t> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	for (const size_t value : stream2)
	{
		sampler.addElement(value);
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
			sampler.addElement(n);
		}

		ReservoirSampler<int, std::mt19937&> samplerCopy(sampler);
		ReservoirSampler<int, std::mt19937&> samplerMoved(std::move(samplerCopy));

		const std::vector<int> result = samplerMoved.consumeResult();
		for (const int value : result)
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
			if (sampler.willNextBeConsidered())
			{
				sampler.addElement(n);
			}
			else
			{
				sampler.addDummyElement();
			}
		}

		ReservoirSampler<int, std::mt19937&> samplerCopy(sampler);
		ReservoirSampler<int, std::mt19937&> samplerMoved(std::move(samplerCopy));

		const std::vector<int> result = samplerMoved.consumeResult();
		for (const int value : result)
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
			sampler.addElement(n);
			n += static_cast<int>(sampler.getNextElementsSkippedNumber());
			sampler.jumpAhead(sampler.getNextElementsSkippedNumber());
		}

		ReservoirSampler<int, std::mt19937&> samplerCopy(sampler);
		ReservoirSampler<int, std::mt19937&> samplerMoved(std::move(samplerCopy));

		const std::vector<int> result = samplerMoved.consumeResult();
		for (const int value : result)
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

TEST(ReservoirSampler, SamplersWithDifferentTypes_ConstructedFilledCopiedAndMoved_Compiles)
{
	{
		ReservoirSampler<int> sampler(2);
		sampler.addElement(10);
		sampler.emplaceElement(20);
		sampler.addElement(40);

		ReservoirSampler<int> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSampler<int> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<std::string> sampler(2);
		sampler.addElement("test");
		sampler.emplaceElement("test2");
		sampler.addElement("test3");

		ReservoirSampler<std::string> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSampler<std::string> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<ImplicitCtor> sampler(2);
		ImplicitCtor v(2);
		sampler.addElement(v);
		sampler.addElement(ImplicitCtor(2));
		sampler.emplaceElement(2);
		sampler.addElement(2);

		ReservoirSampler<ImplicitCtor> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSampler<ImplicitCtor> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<Simple> sampler(2);
		Simple v(2);
		sampler.addElement(v);
		sampler.addElement(Simple(2));
		sampler.emplaceElement(2);
		//sampler.addElement(2); // won't compile, explicit constructor

		ReservoirSampler<Simple> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSampler<Simple> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<TwoArgs> sampler(2);
		TwoArgs v(2, 5.5f);
		sampler.addElement(v);
		sampler.addElement(TwoArgs(2, 9.0f));
		sampler.emplaceElement(2, 3.5f);
		//sampler.addElement(2); // won't compile, no matching constructor

		ReservoirSampler<TwoArgs> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSampler<TwoArgs> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<NonCopyable> sampler(2);
		//NonCopyable v(2);
		//sampler.addElement(v);	// won't compile, no copy constructor
		sampler.addElement(NonCopyable(2));
		sampler.emplaceElement(2);
		//sampler.addElement(2); // won't compile, explicit constructor

		ReservoirSampler<NonCopyable> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<NonMovable> sampler(2);
		NonMovable v(2);
		sampler.addElement(v);
		sampler.addElement(NonMovable(2));
		sampler.emplaceElement(2);
		//sampler.addElement(2); // won't compile, explicit constructor

		ReservoirSampler<NonMovable> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSampler<NonMovable> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<NonCopyableNonMovable> sampler(2);
		//NonCopyableNonMovable v(2);
		//sampler.addElement(v); // won't compile, no copy constructor
		//sampler.addElement(NonCopyableNonMovable(2)); // won't compile, should be copy or move constructible
		sampler.emplaceElement(2);
		//sampler.addElement(2); // won't compile, explicit constructor

		ReservoirSampler<NonCopyableNonMovable> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<OnlyCopyConstructible> sampler(2);
		OnlyCopyConstructible v(2);
		sampler.addElement(v);
		sampler.addElement(OnlyCopyConstructible(2));
		sampler.emplaceElement(2);
		//sampler.addElement(2); // won't compile, explicit constructor

		ReservoirSampler<OnlyCopyConstructible> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSampler<OnlyCopyConstructible> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<OnlyCopyAssignable> sampler(2);
		//OnlyCopyAssignable v(2);
		//sampler.addElement(v); // won't compile, no copy constructor
		//sampler.addElement(OnlyCopyAssignable(2)); // won't compile, should be copy or move constructible
		sampler.emplaceElement(2);
		//sampler.addElement(2); // won't compile, explicit constructor

		ReservoirSampler<OnlyCopyAssignable> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<OnlyMoveConstructible> sampler(2);
		//OnlyMoveConstructible v(2);
		//sampler.addElement(v); // won't compile, no copy constructor
		sampler.addElement(OnlyMoveConstructible(2));
		sampler.emplaceElement(2);
		//sampler.addElement(2); // won't compile, explicit constructor

		ReservoirSampler<OnlyMoveConstructible> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSampler<OnlyMoveAssignable> sampler(2);
		//OnlyMoveAssignable v(2);
		//sampler.addElement(v); // won't compile, no copy constructor
		//sampler.addElement(OnlyMoveAssignable(2)); // won't compile, should be copy or move constructible
		sampler.emplaceElement(2);
		//sampler.addElement(2); // won't compile, explicit constructor

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
		sampler.emplaceElement();
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
