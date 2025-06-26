# B+ Tree Implementation in C++

This repository contains a complete implementation of a B+ tree in C++, supporting insertion, deletion, and search operations. The implementation includes a simple command-line interface (Terminal Wizard) to interactively operate on the tree and visualize its structure.

## Features

* **Dynamic B+ Tree**: Supports configurable order (`m`) for internal nodes and leaf node capacity (`m_leaf`).
* **Insertion**: Inserts keys, splits nodes when full, and promotes keys up the tree.
* **Deletion**: Removes keys, handles underflow via redistribution or merging, and adjusts the tree root when empty.
* **Search**: Finds if a key exists in the tree in O(log n) time.
* **Visualization**: Prints the tree level-by-level to the console for easy debugging.
* **Parent Pointers**: Maintains parent pointers for efficient upward operations.

## Directory Structure

```
├── src
│   └── BPlusTree.cpp     # Main implementation and CLI
└── README.md             # This file
```

> **Note**: In this repository, all code is contained in `BPlusTree.cpp`. You may rename or reorganize into multiple files as needed.

## Requirements

* C++11 (or later) compiler (e.g., g++, clang++)

## Compilation

Compile the code using your preferred C++ compiler. For example:

```bash
g++ -std=c++11 -O2 -o bptree BPlusTree.cpp
```

* `-std=c++11` enables modern C++ features.
* `-O2` turns on optimizations.
* `-o bptree` names the output executable `bptree`.

## Usage

Run the compiled executable and follow the on-screen wizard:

```bash
./bptree
```

1. **Enter `m` and `m_leaf`**: Specify the order of internal nodes (`m`) and the maximum keys in a leaf (`m_leaf`).
2. **Terminal Wizard Commands**:

   * `i <value>`: Insert a key with value `<value>`.
   * `d <value>`: Delete a key with value `<value>`.
   * `s <value>`: Search for a key with value `<value>`.
   * `p`:      Print the current B+ tree structure.
   * `h`:      Display the wizard commands again.
   * `q`:      Quit the application.

Example session:

```
Enter m , m_leaf: 4 3

------Terminal Wizard------

CommandSyntax   Action
---------------------------
p         Print the B+ tree structure.
i <value> Insert a key.
d <value> Delete a key.
s <value> Search a key.
h         Show commands.
q         Quit.

Enter Command: > i 10
Successfully Inserted!

Enter Command: > i 20
Successfully Inserted!

Enter Command: > p
B+ tree structure...
[ 10 20 ]

Enter Command: > q
bye!
```

## Code Overview

### `Node` Class

* **Members**:

  * `ll cur_size`: Number of keys currently in the node.
  * `ll size`: Capacity parameter (leaf or internal).
  * `bool isLeaf`: Flag indicating leaf or internal node.
  * `vector<ll> keyValues`: Container for keys.
  * `vector<Node*> pointerValues`: Child pointers (internal) or data/leaf pointers.
  * `Node* parent`: Pointer to the parent node.

* **Key Methods**:

  * `insert_key(key)`: Inserts a key into the node and keeps it sorted.
  * `insert_with_pointer(key, left, right)`: Inserts a key with two child pointers (after split).
  * `delete_key(key)`: Removes a key and shifts subsequent keys.
  * `get_pointer_from_key(key)`: Chooses the child index to descend for a given key.
  * `print()`: Displays the node's keys.

### `BPlusTree` Class

* **Member**:

  * `Node* root`: Root pointer of the B+ tree.

* **Key Methods**:

  * `insertHandler(key)`: Public API to insert a key, handles root creation.
  * `deleteHandler(key)`: Public API to delete a key, adjusts root if empty.
  * `searchHandler(key)`: Returns `true` if the key exists.
  * `print_directory(root)`: Level-order print of the tree.
  * Internal helpers for splitting (`split_leaf`, `split_node`), merging, redistribution, and pointer updates.

## Limitations and TODOs

* **Memory Management**: The `Node` destructor clears vectors but does not recursively delete child nodes, which may cause memory leaks.
* **Efficiency**: Uses `sort` on every insert in a node; for large node capacities, consider binary search + shift.
* **Error Handling**: Minimal checks on invalid input (e.g., small `m` values) and pointer bounds.

## License

This code is provided under the MIT License. See `LICENSE` for details.
