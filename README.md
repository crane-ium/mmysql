## MMYSQL Database
# Stephen Crane

**Features**
* Uses user-defined:
 * BPlusTrees to hold data when drawing from file
 * multimap for efficient pairing of data
 * parsing tools
 * expression tree for logical comparisons
  * Also handles efficient set and/or/xor
* Bitmask flagging for enum types
* Parsing for reading to/from file

**Issues**
* Unneccessary data copying in the BPlusTree for types that don't have move constructors
 * Need move constructors for some data structures
  * multimaps
  * simple_map
  * bplustree

