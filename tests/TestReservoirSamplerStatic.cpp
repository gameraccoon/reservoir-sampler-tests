#include <gtest/gtest.h>

#include "reservoir-sampler/reservoir_sampler_static.h"

#include <array>
#include <numeric>

#include "TestTypes.h"

TEST(ReservoirSamplerStatic, SamplersOfDifferentTypes_CreeateFillAndDestroy_DoNotCrash)
{
	{
		ReservoirSamplerStatic<std::string, 5> sampler;
		sampler.addElement("list");
		sampler.addElement("of");
		sampler.addElement("test");
		sampler.addElement("string");
		sampler.addElement("items");
	}

	{
		ReservoirSamplerStatic<size_t, 5> sampler;
		sampler.addElement(0);
		sampler.addElement(1);
		sampler.addElement(2);
		sampler.addElement(3);
		sampler.addElement(4);
	}

	{
		ReservoirSamplerStatic<std::vector<int>, 5> sampler;
		sampler.addElement(std::vector<int>{{1, 2}});
		sampler.addElement(std::vector<int>{{3, 4}});
		sampler.addElement(std::vector<int>{{5, 6}});
		sampler.addElement(std::vector<int>{{7, 8}});
		sampler.addElement(std::vector<int>{{9, 10}});
	}

	{
		ReservoirSamplerStatic<std::thread, 5> sampler;
		sampler.emplaceElement();
		sampler.emplaceElement();
		sampler.emplaceElement();
		sampler.emplaceElement();
		sampler.emplaceElement();
	}
}

TEST(ReservoirSamplerStatic, SamplersOfDifferentSizes_CreeateFillAndDestroy_DoNotCrash)
{
	{
		ReservoirSamplerStatic<size_t, 5> sampler;
		sampler.addElement(0);
		sampler.addElement(1);
		sampler.addElement(2);
		sampler.addElement(3);
		sampler.addElement(4);
	}

	{
		ReservoirSamplerStatic<size_t, 1> sampler;
		sampler.addElement(0);
		sampler.addElement(1);
		sampler.addElement(2);
		sampler.addElement(3);
		sampler.addElement(4);
	}

	{
		ReservoirSamplerStatic<size_t, 300> sampler;
		sampler.addElement(0);
		sampler.addElement(1);
		sampler.addElement(2);
		sampler.addElement(3);
		sampler.addElement(4);
	}

	{
		ReservoirSamplerStatic<size_t, 3> sampler;
		(void)sampler;
	}
}

TEST(ReservoirSamplerStatic, SamplerOfSizeFive_FiveElementsAdded_HasOnlyOriginalElements)
{
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	ReservoirSamplerStatic<size_t, 5> sampler;
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

TEST(ReservoirSamplerStatic, SamplerOfSizeFive_ThreeElementsAdded_HasOnlyOriginalElements)
{
	const std::vector<size_t> stream({10, 11, 12});

	ReservoirSamplerStatic<size_t, 5> sampler;
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

TEST(ReservoirSamplerStatic, EmptySampler_Reset_DoesNotCrash)
{
	ReservoirSamplerStatic<size_t, 5> sampler;
	sampler.reset();
}

TEST(ReservoirSamplerStatic, SamplerWithAResult_Reset_CanBeReused)
{
	ReservoirSamplerStatic<size_t, 5> sampler;
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

TEST(ReservoirSamplerStatic, SamplerWithAResult_Consume_CanBeReused)
{
	ReservoirSamplerStatic<size_t, 5> sampler;
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

TEST(ReservoirSamplerStatic, EmptySampler_Copied_DoesNotCrash)
{
	ReservoirSamplerStatic<size_t, 5> sampler;
	ReservoirSamplerStatic<size_t, 5> copySampler(sampler);
	(void)copySampler;
}

TEST(ReservoirSamplerStatic, Sampler_Copied_HoldsTheData)
{
	ReservoirSamplerStatic<size_t, 5> sampler;
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	for (const size_t value : stream)
	{
		sampler.addElement(value);
	}

	ReservoirSamplerStatic<size_t, 5> samplerCopy(sampler);

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

TEST(ReservoirSamplerStatic, EmptySampler_Moved_DoesNotCrash)
{
	ReservoirSamplerStatic<size_t, 5> sampler;
	ReservoirSamplerStatic<size_t, 5> copySampler(std::move(sampler));
	(void)copySampler;
}

TEST(ReservoirSamplerStatic, Sampler_Moved_ValueIsMoved)
{
	ReservoirSamplerStatic<size_t, 5> sampler;
	const std::vector<size_t> stream({10, 11, 12, 13, 14});

	for (const size_t value : stream)
	{
		sampler.addElement(value);
	}

	ReservoirSamplerStatic<size_t, 5> samplerMovedTo(std::move(sampler));

	{
		std::vector<size_t> result = samplerMovedTo.consumeResult();
		std::sort(result.begin(), result.end());
		EXPECT_EQ(result, stream);
	}
}

TEST(ReservoirSamplerStatic, Sampler_Moved_OldSamplerCanBeReused)
{
	ReservoirSamplerStatic<size_t, 5> sampler;
	const std::vector<size_t> stream1({10, 11, 12, 13, 14});
	const std::vector<size_t> stream2({15, 16, 17, 18, 19});

	for (const size_t value : stream1)
	{
		sampler.addElement(value);
	}

	{
		ReservoirSamplerStatic<size_t, 5> samplerMovedTo(std::move(sampler));
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

TEST(ReservoirSamplerStatic, SamplerSizeOfFive_SamplingFromStreamOfTwenty_ProducesEqualFrequencies)
{
	std::array<int, 20> frequences{};
	// reuse random to speed things up a bit
	std::mt19937 rand{std::random_device{}()};
	for (int i = 0; i < 10000; ++i)
	{
		ReservoirSamplerStatic<int, 5, std::mt19937&> sampler(rand);

		for (int n = 0; n < 20; ++n)
		{
			sampler.addElement(n);
		}

		ReservoirSamplerStatic<int, 5, std::mt19937&> samplerCopy(sampler);
		ReservoirSamplerStatic<int, 5, std::mt19937&> samplerMoved(std::move(samplerCopy));

		const auto [data, size] = samplerMoved.getResult();
		for (size_t k = 0; k < size; ++k)
		{
			++frequences[data[k]];
		}
	}

	const float frequencySum = std::accumulate(frequences.begin(), frequences.end(), 0.0f);
	ASSERT_EQ(5.0f * 10000, frequencySum);
	for (const int freq : frequences)
	{
		EXPECT_NEAR(0.05f, freq/frequencySum, 0.01f);
	}
}

TEST(ReservoirSamplerStatic, Sampler_AddingWhenWillBeConsidered_ProducesEqualFrequencies)
{
	std::array<int, 20> frequences{};
	// reuse random to speed things up a bit
	std::mt19937 rand{std::random_device{}()};
	for (int i = 0; i < 10000; ++i)
	{
		ReservoirSamplerStatic<int, 5, std::mt19937&> sampler(rand);

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

		ReservoirSamplerStatic<int, 5, std::mt19937&> samplerCopy(sampler);
		ReservoirSamplerStatic<int, 5, std::mt19937&> samplerMoved(std::move(samplerCopy));

		const auto [data, size] = samplerMoved.getResult();
		for (size_t k = 0; k < size; ++k)
		{
			++frequences[data[k]];
		}
	}

	const float frequencySum = std::accumulate(frequences.begin(), frequences.end(), 0.0f);
	ASSERT_EQ(5.0f * 10000, frequencySum);
	for (const int freq : frequences)
	{
		EXPECT_NEAR(0.05f, freq/frequencySum, 0.01f);
	}
}

TEST(ReservoirSamplerStatic, Sampler_JumpAheadWhenAdding_ProducesEqualFrequencies)
{
	std::array<int, 20> frequences{};
	// reuse random to speed things up a bit
	std::mt19937 rand{std::random_device{}()};
	for (int i = 0; i < 10000; ++i)
	{
		ReservoirSamplerStatic<int, 5, std::mt19937&> sampler(rand);

		for (int n = 0; n < 20; ++n)
		{
			sampler.addElement(n);
			n += static_cast<int>(sampler.getNextElementsSkippedNumber());
			sampler.jumpAhead(sampler.getNextElementsSkippedNumber());
		}

		ReservoirSamplerStatic<int, 5, std::mt19937&> samplerCopy(sampler);
		ReservoirSamplerStatic<int, 5, std::mt19937&> samplerMoved(std::move(samplerCopy));

		const auto [data, size] = samplerMoved.getResult();
		for (size_t k = 0; k < size; ++k)
		{
			++frequences[data[k]];
		}
	}

	const float frequencySum = std::accumulate(frequences.begin(), frequences.end(), 0.0f);
	ASSERT_EQ(5.0f * 10000, frequencySum);
	for (const int freq : frequences)
	{
		EXPECT_NEAR(0.05f, freq/frequencySum, 0.01f);
	}
}

TEST(ReservoirSamplerStatic, SamplersWithDifferentTypes_ConstructedFilledCopiedAndMoved_Compiles)
{
	{
		ReservoirSamplerStatic<int, 2> sampler;
		sampler.addElement(10);
		sampler.emplaceElement(20);
		sampler.addElement(40);

		ReservoirSamplerStatic<int, 2> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerStatic<int, 2> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerStatic<std::string, 2> sampler;
		sampler.addElement("test");
		sampler.emplaceElement("test2");
		sampler.addElement("test3");

		ReservoirSamplerStatic<std::string, 2> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerStatic<std::string, 2> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerStatic<ImplicitCtor, 2> sampler;
		ImplicitCtor v(2);
		sampler.addElement(v);
		sampler.addElement(ImplicitCtor(2));
		sampler.emplaceElement(2);
		sampler.addElement(2);

		ReservoirSamplerStatic<ImplicitCtor, 2> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerStatic<ImplicitCtor, 2> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerStatic<Simple, 2> sampler;
		Simple v(2);
		sampler.addElement(v);
		sampler.addElement(Simple(2));
		sampler.emplaceElement(2);
		//sampler.addElement(2); // won't compile, explicit constructor

		ReservoirSamplerStatic<Simple, 2> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerStatic<Simple, 2> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerStatic<TwoArgs, 2> sampler;
		TwoArgs v(2, 5.5f);
		sampler.addElement(v);
		sampler.addElement(TwoArgs(2, 9.0f));
		sampler.emplaceElement(2, 3.5f);
		//sampler.addElement(2); // won't compile, no matching constructor

		ReservoirSamplerStatic<TwoArgs, 2> samplerCopy(sampler);
		(void)samplerCopy;
		ReservoirSamplerStatic<TwoArgs, 2> samplerMovedTo(std::move(samplerCopy));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerStatic<NonCopyable, 2> sampler;
		//NonCopyable v(2);
		//sampler.addElement(v);	// won't compile, no copy constructor
		sampler.addElement(NonCopyable(2));
		sampler.emplaceElement(2);
		//sampler.addElement(2); // won't compile, explicit constructor

		ReservoirSamplerStatic<NonCopyable, 2> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerStatic<NonMovable, 2> sampler;
		NonMovable v(2);
		sampler.addElement(v);
		sampler.addElement(NonMovable(2));
		sampler.emplaceElement(2);
		//sampler.addElement(2); // won't compile, explicit constructor

		ReservoirSamplerStatic<NonMovable, 2> samplerCopy(sampler);
		(void)samplerCopy;
	}

	{
		ReservoirSamplerStatic<NonCopyableNonMovable, 2> sampler;
		//NonCopyableNonMovable v(2);
		//sampler.addElement(v); // won't compile, no copy constructor
		//sampler.addElement(NonCopyableNonMovable(2)); // won't compile, should be copy or move constructible
		sampler.emplaceElement(2);
		//sampler.addElement(2); // won't compile, explicit constructor
	}

	{
		ReservoirSamplerStatic<OnlyCopyConstructible, 2> sampler;
		OnlyCopyConstructible v(2);
		sampler.addElement(v);
		sampler.addElement(OnlyCopyConstructible(2));
		sampler.emplaceElement(2);
		//sampler.addElement(2); // won't compile, explicit constructor

		ReservoirSamplerStatic<OnlyCopyConstructible, 2> samplerCopy(sampler);
		(void)samplerCopy;
	}

	{
		ReservoirSamplerStatic<OnlyCopyAssignable, 2> sampler;
		//OnlyCopyAssignable v(2);
		//sampler.addElement(v); // won't compile, no copy constructor
		//sampler.addElement(OnlyCopyAssignable(2)); // won't compile, should be copy or move constructible
		sampler.emplaceElement(2);
		//sampler.addElement(2); // won't compile, explicit constructor
	}

	{
		ReservoirSamplerStatic<OnlyMoveConstructible, 2> sampler;
		//OnlyMoveConstructible v(2);
		//sampler.addElement(v); // won't compile, no copy constructor
		sampler.addElement(OnlyMoveConstructible(2));
		sampler.emplaceElement(2);
		//sampler.addElement(2); // won't compile, explicit constructor

		ReservoirSamplerStatic<OnlyMoveConstructible, 2> samplerMovedTo(std::move(sampler));
		(void)samplerMovedTo;
	}

	{
		ReservoirSamplerStatic<OnlyMoveAssignable, 2> sampler;
		//OnlyMoveAssignable v(2);
		//sampler.addElement(v); // won't compile, no copy constructor
		//sampler.addElement(OnlyMoveAssignable(2)); // won't compile, should be copy or move constructible
		sampler.emplaceElement(2);
		//sampler.addElement(2); // won't compile, explicit constructor
	}
}

TEST(ReservoirSamplerStatic, Sampler_ConstructedFilledAndConsumed_ProducesReasonableAmountOfMoves)
{
	constexpr size_t sampleSize = 5;
	const size_t streamSize = 500;

	CopyMoveCounter::Reset();

	ReservoirSamplerStatic<CopyMoveCounter, sampleSize> sampler;
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
