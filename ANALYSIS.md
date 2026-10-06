# ANALYSIS

## Item 1 
| Category | Implementation In Another Language | Cost Comparison with C
|---|---|---|
| Map | In Python,  the dictionary allows directly coding d["beta"] = 22. The interpreter handles the chaining, hashing, and key-value pairing. Python's dictionary actually uses two arrays (indices table and entries table) for its implementation, similiar to the buckets + insertion-order in this activity. |  Unlike our C implementation where key is an allocated byte copy and value is a tag + union struct, Python considers both the keys and value as full objects. Python's key and values are both full objects with their own header, resulting more memory being allocated for each key-value's attributes (object header, hash value, etc.). This becomes noticeable if a map was to hold a large number of entries. Each entries' overhead multipled across many keys that will add up to more memory than what our C implementation would do for the same data | 
| Array | In Java, it automatically checks for the validity of the index accessing the array. It throws `ArrayIndexOutOfBoundsException` when accessing an invalid index. | Accessing arrays in C is faster compare to Java, however, Java offers more safety as it checks every access if it it is valid to its bounds. |
| Enum | In Dart, enum contains declared constant values. Dart has a built-in access to name, index, and find an index by its name by Dart's built-in getter for enums: `.name`, `.index`, and `.values.byName()` notation. For instance, in the Color, if declared in Dart, accessing the name is `Color.blue.name` and its output is `blue`. Built-in access for the index is by `.index`, `Color.blue.index` which is `2`. In finding the index by the name, however, has to access the value by name and use the `.index`. For example, `Color.values.byName("blue").index`. It's an extra step compare to C. | Efficient when dealing with small size of enum, convenient with the built-in getters. Memory cost more than in C since it only deals with `ordinal` integers as representation of its corresponding color name, which is smaller in memory than Dart that treats them as objects. This is noticeable in large size of enum. |


## Item 2
The C version allows you to be in full control of the rules you want to implement for each value that you deal with, which is limited when the compiler automatically checks. Basically, it makes you deal with what you want to deal with. It can be a double-edged sword, although this freedom can make you explore and allows flexibility in the program, dealing with the edge cases should be extra careful as it might crash due to unhandled invalid inputs. 


## Item 3 
Say we decided to drop insertion order. dt_map would simply need to iterate through the buckets to get the value of the given key. If in this implementation, the programmer were to ask what was the first key or what key came before this given key. The version of this map wouldn't be able to answer that as there is no order of how the keys are sequenced.
Arguably, in most situations where maps are implemented, this wouldn't matter at all. After all, you just need the key-value pair. In designing a language though, you would need to solve a gap a prior language had, and considering situations programmers need to know the order of the keys inserted (notably LRU caching), I wouldn't ship this version despite it being more simple.


## Item 4 
Accessing an allocation after its release can produce incorrect data due to incorrect access. The address still points at the memory, but it may be given to a different allocation that is different from the old one that it used to hold. Hence, when accessed, the address could contain a different data. Allocation that remains unreleased can keep adding up to the memory when ignored, which in long time can make the memory access slower and may crash.