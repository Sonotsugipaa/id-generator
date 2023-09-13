#pragma once

#include <type_traits>
#include <concepts>
#include <limits>
#include <cstdint>



namespace idgen {

	template <typename T>
	concept SignedInt = std::signed_integral<T>;

	template <typename T>
	concept UnsignedInt = std::unsigned_integral<T>;

	template <typename T>
	concept GenericInt = SignedInt<T> || UnsignedInt<T>;

	template <typename T>
	concept ScopedEnum = std::is_scoped_enum_v<T> && GenericInt<std::underlying_type_t<T>>;

	template <typename T>
	concept Id = GenericInt<T> || ScopedEnum<T>;


	template <SignedInt   T> constexpr T baseId()    noexcept { return 0; }
	template <UnsignedInt T> constexpr T baseId()    noexcept { return 1; }
	template <SignedInt   T> constexpr T invalidId() noexcept { return std::numeric_limits<T>::min(); }
	template <UnsignedInt T> constexpr T invalidId() noexcept { return 0; }
	template <SignedInt   T> constexpr T minId()     noexcept { return invalidId<T>() + 1; }
	template <UnsignedInt T> constexpr T minId()     noexcept { return baseId<T>(); }
	template <GenericInt  T> constexpr T maxId()     noexcept { return std::numeric_limits<T>::max(); }

	template <ScopedEnum T> constexpr T baseId()    noexcept { return T(baseId<std::underlying_type_t<T>>()); }
	template <ScopedEnum T> constexpr T invalidId() noexcept { return T(invalidId<std::underlying_type_t<T>>()); }
	template <ScopedEnum T> constexpr T minId()     noexcept { return T(minId<std::underlying_type_t<T>>()); }
	template <ScopedEnum T> constexpr T maxId()     noexcept { return T(maxId<std::underlying_type_t<T>>()); }

	static_assert(invalidId<int8_t>() == -128);
	static_assert(minId<int8_t>()     == -127);



	template <ScopedEnum T>
	class IdGenerator {
	public:
		T generate() noexcept {
			auto r = std::underlying_type_t<T>(++ gen_value);
			#ifdef assert
				assert(r >= minId<std::underlying_type_t<T>>());
				assert(r <= maxId<std::underlying_type_t<T>>());
			#endif
			return T(r);
		}

		void recycle(T id) {
			// NOP
		}

	private:
		std::underlying_type_t<T> gen_value = baseId<std::underlying_type_t<T>>() - 1;
	};

}
