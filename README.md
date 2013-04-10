# C++ library to convert [almost] any type to a string

`to_stream` is C++ **header only** library designed to output _almost_ any type to a std::o[w]stream.  
Main purpouse of the library is output arbitary variable/expression to a log file/console.


## Examples

```c++
int aVar = 10;

Utils::Manip::to_stream(std::cout, aVar);
```

Output:
```
10
```

or using `to_stream_helper.hpp`:

```c++
int aVar = 10;

std::cout << LOG_VAR(aVar);
```

Output:
```
aVar<10>
```

Not really exciting, but you can change type of `aVar` without touching anything else:
```c++
int bool = true;

std::cout << LOG_VAR(aVar);
```

Output:
```
aVar<true>
```

or

```c++
int aVar[] = {1, 2, 3, 4, 5, 6};

std::cout << LOG_VAR(aVar);
```

Output:
```
aVar<[6](1, 2, 3, 4, 5, 6)>
```


## Supported types

At the moment `to_stream` supports following types out of a box:
* builtin arithmetic types (char, unsigned, int, float, etc)
* unsigned char (outputs as *unsigned*)
* builtin (C style) arrays
* STL data types and containers/adapters:
  * std::pair
  * std::vector
  * std::list
  * std::queue
  * std::priority_queue
  * std::dequeue
  * std::stack
  * std::map
  * std::set
  * std::multimap
  * std::multiset
  * std::string
* boost data types/containers:
  * boost::unordered::unordered_set
  * boost::optional
  * boost::array
* any type which has an output operator (operator<<()) defined
* any STL-like container
* any type which has to_stream() method defined
* any type which has to_string() method defined

Output is recursive. It means `to_stream` can output variables of quite complex types:
```c++
std::vector<std::pair<int, boost::optional<std::string> > > aVar;
aVar.push_back(std::make_pair(1, "one"));
aVar.push_back(std::make_pair(2, "two"));
aVar.push_back(std::make_pair(3, boost::optional<std::string>()));

std::cout << LOG_VAR(aVar);
```

Output:
```
aVar<[3]((1, one), (2, two), (3, three))>
```

## Dependencies
`to_stream` depends on following `Boost` libraries:
  * type_traits
  * utility (enable_if)
  * MPL
  * optional


## Algorithm
`to_stream` using following algorithm to output type `T` to a stream:
  * if `T` is `bool` `to_stream' output `true` or `false`.
  * if `T` is `unsigned char` `to_stream' output value **casted** to `unsigned int`.
  * if `T` is `std::pair` `to_stream' output value as `(first, second)`. `to_stream` calls itself to output `first` and `second`. This allows to output complex types.
  * if `T` is `boost::optional`  `to_stream' output "--' if it is not initialized or calls itself to output the value.
  * if `T` has an output operator (`std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits> &, const T&)`) `to_stream` uses the output operator.
  * if `T` looks like STL container (has `T::const_iterator` and `T::value_type` defined) or `T` is builtin (C style) array `to_stream` loops thru elements of `T` and ouput them to a stream (in format \[N](E1, E2, .., En)). `to_stream` calls itself to output each element. This allows to output complex types.
  * if `T` is STL adapter (`std::stack`, `std::dequeue`, `std::queue` or `std::priority_queue`) `to_stream` output element of the adapter using the same format as STL container.
  * if `T` has `to_stream` method (`std::basic_ostream<CharT, Traits>& to_stream(std::basic_ostream<CharT, Traits>&) const`) `to_stream` uses the method.
  * if `T` has `to_string` method (`std::string to_string() const`) `to_stream` uses the method.


## Copyright

This library is available under the Boost Software License.  
Copyright (C) 2013 Dmitriy Maksimov.  
Distributed under the Boost Software License, Version 1.0. (See Accompanying File LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
