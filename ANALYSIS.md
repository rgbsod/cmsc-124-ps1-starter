# ANALYSIS

## Item 1 
| Category | Implementation In Another Language | Cost Comparison with C
|---|---|---|
| Map | In Python,  the dictionary allows directly coding d["beta"] = 22. The interpreter handles the chaining, hashing, and key-value pairing. Python's dictionary actually uses two arrays (indices table and entries table) for its implementation, similiar to the buckets + insertion-order in this activity. |  Unlike our C implementation where key is an allocated byte copy and value is a tag + union struct, Python considers both the keys and value as full objects. Python's key and values are both full objects with their own header, resulting more memory being allocated for each key-value's attributes (object header, hash value, etc.). This becomes noticeable if a map was to hold a large number of entries. Each entries' overhead multipled across many keys that will add up to more memory than what our C implementation would do for the same data | 
|  |  | |
|  |  | |

## Item 2
 


## Item 3 
Say we decided to drop insertion order. dt_map would simply need to iterate through the buckets to get the value of the given key. If in this implementation, the programmer were to ask what was the first key or what key came before this given key. The version of this map wouldn't be able to answer that as there is no order of how the keys are sequenced.
Arguably, in most situations where maps are implemented, this wouldn't matter at all. After all, you just need the key-value pair. In designing a language though, you would need to solve a gap a prior language had, and considering situations programmers need to know the order of the keys inserted (notably LRU caching), I wouldn't ship this version despite it being more simple.


## Item 4 
