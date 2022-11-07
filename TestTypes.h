#pragma once

class ImplicitCtor {
public:
	ImplicitCtor(int) {}
};

class Simple {
public:
	explicit Simple(int) {}
};

class TwoArgs {
public:
	TwoArgs(int, float) {}
};

class NonCopyable {
public:
	explicit NonCopyable(int) {}
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
	NonCopyable(NonCopyable&&) noexcept = default;
	NonCopyable& operator=(NonCopyable&&) noexcept = default;
};

class NonMovable {
public:
	explicit NonMovable(int) {}
	NonMovable(const NonMovable&) = default;
	NonMovable& operator=(const NonMovable&) = default;
	NonMovable(NonMovable&&) noexcept = delete;
	NonMovable& operator=(NonMovable&&) noexcept = delete;
};

class NonCopyableNonMovable {
public:
	explicit NonCopyableNonMovable(int) {}
	NonCopyableNonMovable(const NonCopyableNonMovable&) = delete;
	NonCopyableNonMovable& operator=(const NonCopyableNonMovable&) = delete;
	NonCopyableNonMovable(NonCopyableNonMovable&&) noexcept = delete;
	NonCopyableNonMovable& operator=(NonCopyableNonMovable&&) noexcept = delete;
};

class OnlyCopyConstructible {
public:
	explicit OnlyCopyConstructible(int) {}
	OnlyCopyConstructible(const OnlyCopyConstructible&) = default;
	OnlyCopyConstructible& operator=(const OnlyCopyConstructible&) = delete;
	OnlyCopyConstructible(OnlyCopyConstructible&&) noexcept = delete;
	OnlyCopyConstructible& operator=(OnlyCopyConstructible&&) noexcept = delete;
};

class OnlyCopyAssignable {
public:
	explicit OnlyCopyAssignable(int) {}
	OnlyCopyAssignable(const OnlyCopyAssignable&) = delete;
	OnlyCopyAssignable& operator=(const OnlyCopyAssignable&) = default;
	OnlyCopyAssignable(OnlyCopyAssignable&&) noexcept = delete;
	OnlyCopyAssignable& operator=(OnlyCopyAssignable&&) noexcept = delete;
};

class OnlyMoveConstructible {
public:
	explicit OnlyMoveConstructible(int) {}
	OnlyMoveConstructible(const OnlyMoveConstructible&) = delete;
	OnlyMoveConstructible& operator=(const OnlyMoveConstructible&) = delete;
	OnlyMoveConstructible(OnlyMoveConstructible&&) noexcept = default;
	OnlyMoveConstructible& operator=(OnlyMoveConstructible&&) noexcept = delete;
};

class OnlyMoveAssignable {
public:
	explicit OnlyMoveAssignable(int) {}
	OnlyMoveAssignable(const OnlyMoveAssignable&) = delete;
	OnlyMoveAssignable& operator=(const OnlyMoveAssignable&) = delete;
	OnlyMoveAssignable(OnlyMoveAssignable&&) noexcept = delete;
	OnlyMoveAssignable& operator=(OnlyMoveAssignable&&) noexcept = default;
};

// this may be broken in the future as calling copy/move constructors can be omitted
class CopyMoveCounter
{
public:
	CopyMoveCounter() { ++sConstructions; }
	CopyMoveCounter(const CopyMoveCounter&) { ++sCopiesCount; }
	CopyMoveCounter& operator=(const CopyMoveCounter&) { ++sCopiesCount; return *this; }
	CopyMoveCounter(CopyMoveCounter&&) noexcept { ++sMovesCount; }
	CopyMoveCounter& operator=(CopyMoveCounter&&) noexcept { ++sMovesCount; return *this; }

	static int GetConstructionsCount() { return sConstructions; }
	static int GetCopiesCount() { return sCopiesCount; }
	static int GetMovesCount() { return sMovesCount; }

	static void Reset() { sConstructions = 0; sCopiesCount = 0; sMovesCount = 0; }

private:
	static int sConstructions;
	static int sCopiesCount;
	static int sMovesCount;
};

inline int CopyMoveCounter::sConstructions = 0;
inline int CopyMoveCounter::sCopiesCount = 0;
inline int CopyMoveCounter::sMovesCount = 0;
