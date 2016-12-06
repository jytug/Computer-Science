#ifndef PIZZA_H
#define PIZZA_H
#include <cstddef>
#include <type_traits>
#include <array>

//HELPERS
namespace helpers{
  // Returns -1 if LookingFor is not found
  template <typename LookingFor>
  constexpr int NumberOfType(size_t)
  {
    return -1;
  }

  template <typename LookingFor, typename Head, typename... Tail>
  constexpr int NumberOfType(size_t pos)
  {
    return (std::is_same<LookingFor,Head>::value) ? pos : helpers::NumberOfType<LookingFor,Tail...>(pos+1);
  }
  
  //IsInjective<List...>::value is true iff there are no two same types in List
  
  template <typename... >
  constexpr bool IsInjective = true;

  template <typename Head, typename... Tail>
  constexpr bool IsInjective<Head, Tail...> = ((NumberOfType<Head, Tail...>(0) ==-1) && IsInjective<Tail...>);


  // Returns number of pieces to take
  // takes: max, maxyum(yumminess(max)) and curr are part of recursive call
  // amount is maximum of pieces we can take of given type
  template <typename Kind>
  constexpr size_t most_yummy (size_t max, unsigned long long int max_yum, 
			       size_t curr, size_t amount)
  {
    return (curr == amount+1) ? max :
      (Kind::yumminess(curr) < 0) ? most_yummy<Kind>(max, max_yum, curr+1, amount) :
      ((static_cast<unsigned long long int>(Kind::yumminess(curr)) >= max_yum) ? 
       most_yummy<Kind>(curr, static_cast<unsigned long long int>(Kind::yumminess(curr)), curr+1, amount) :
       most_yummy<Kind>(max, max_yum, curr+1, amount));
  }
  
  // Check if Kind has proper yumminess function.
  template <typename Kind>
  constexpr bool is_yum()
  {
    static_assert(std::is_arithmetic<decltype(Kind::yumminess(0))>::value,
		  "I can't decide is this pizza good or bad!");
    return Kind::yumminess(0) == 0;
  }

  template <bool... K>
  constexpr bool bool_and()
  {
    return (K && ...); 
  }
}
//\HELPERS
template<typename Pizza1, typename Pizza2> 
struct best_mix
{
  static_assert(std::is_same<typename Pizza1::pizzeria, typename Pizza2::pizzeria>::value,
		"We can't mix pizzas from diffrent pizzerias!");
  typedef typename Pizza1::pizzeria::template pizza_mix<Pizza1, Pizza2>::type type;
};

template<typename... Kinds> struct Pizzeria {
  static_assert(helpers::IsInjective<Kinds...>,
  		"No need to mention one pizza type twice!");

  template<size_t... slices> 
  struct Pizza {
    static constexpr std::array<size_t, sizeof... (Kinds)> as_array() {
      return  {{slices...}};
    }

    template<typename PKind> static constexpr size_t count() {
      int pos = helpers::NumberOfType<PKind,Kinds...>(0);
      constexpr std::array<size_t, sizeof... (slices)> arr_of_types = {{slices...}};
      return (pos >= 0) ? arr_of_types[pos] : 0;
    }

    typedef Pizzeria<Kinds...> pizzeria;
    typedef Pizza<(2*slices)...> sliced_type;
  };

  template<typename Pizza1, typename Pizza2>
  struct pizza_mix
  {
    static_assert((helpers::is_yum<Kinds>() && ...), "that pizza looks weird!");
    typedef Pizza<helpers::most_yummy<Kinds>
		   (0,0,0,Pizza1::template count<Kinds>()+Pizza2::template count<Kinds>())...> type;
  };

  template<typename Kind> struct make_pizza {
    static_assert(helpers::NumberOfType<Kind, Kinds...>(0) != -1,
		  "We don't have this one in menu!");
     using type =  Pizza< (std::is_same<Kinds,Kind>::value ? 8 : 0) ...> ;
  };
};

#endif
