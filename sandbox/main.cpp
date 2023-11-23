/**
 * @author Forrest Jablonski
 */

#include <iostream>
#include <vector>
#include <type_traits>

template <typename... Types>
struct Type_register{};

template <typename Queried_type>
constexpr int type_id(Type_register<>) { return -1; }

template <typename Queried_type, typename Type, typename... Types>
constexpr int type_id(Type_register<Type, Types...>) {
    if constexpr (std::is_same_v<Type, Queried_type>) return 0;
    else return 1 + type_id<Queried_type>(Type_register<Types...>());
}

int main() {
   Type_register<int, float, char, std::vector<int>, int&> registered_types;
   constexpr auto test1 = type_id<float>(registered_types);
   constexpr auto test2 = type_id<int>(registered_types);
   constexpr auto SS = sizeof(registered_types);
}