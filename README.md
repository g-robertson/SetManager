# SetManager
Set management tool for using set operations with generated sets of strings
It allows for you to create subsets or groups of any given collection of strings and to manipulate them with the set types provided

## Set Types
The program comes with 9 types of sets available to use:

### Global Set
This is the set that contains all elements, and thusly can be branched off to create any subset of elements.

### Word Set
This type of set contains words defined by the user that exist within the parent set.

### Faux Word Set
This type of set contains words defined by the user, however, for any word that does not exist within the parent set, will be added to a set
of faux elements that get added back into the set's output if they reappear in the parent set. 

### Directory Set
This type of set mirrors the filenames of a directory as the elements within the set.

### Intersection Set
This type of set allows you to take any two subsets of its parent set and generates the intersection between them as the elements within the set.

### Union Set
This type of set allows you to take any two subsets of its parent set and generates the union between them as the elements within the set.

### Difference Set
This type of set allows you to take any two subsets of its parent set and generates the difference between them as the elements within the set.

### Symmetric Difference Set
This type of set allows you to take any two subsets of its parent set and generates the symmetric difference between them as the elements within the set.

### Relative Complement Set
This type of set allows you to take a subset of the parent and generates the relative complement of it as the elements within the set.
