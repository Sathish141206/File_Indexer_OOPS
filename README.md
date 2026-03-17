# Memory-Efficient Versioned File Indexer
**Name :** Palagiri Sathish Reddy  
**Roll No. :** 240717


## Project Description

* This project implements a memory efficient versioned file indexer in C++.
* The program processes large text files incrementally using a fixed size buffer and builds a word-level frequency index.

* The system allows users to run analytical queries on indexed data such as:

    * Word frequency in a version

    * Top-K most frequent words

    * Difference in frequency of a word between two versions

* The design ensures that the entire file is never loaded into memory, making the program scalable for large datasets.  


## Compilation Command
    g++ -O2 code.cpp -o analyzer

## Execution Commands (Examples)
*  ### Word Count Query :  
       ./analyzer --file test_logs.txt --version v1 --buffer 256 --query word --word error 
* ### Top-K Query : 
      ./analyzer --file test_logs.txt --version v1 --buffer 256 --query top --top 10
* ### Diff Query : 
      ./analyzer --file1 test_logs.txt --version1 v1 --file2 verbose_logs.txt --version2 v2 --buffer 256 --query diff --word error

## Command Line Arguments

| Argument | Description |
|---------|-------------|
| `--file` | input file used for single-version queries (word count or top-K). |
| `--file1` | first input file used in the difference query. |
| `--file2` | second input file used in the difference query. |
| `--version` | Version name of the input file for single-version queries. |
| `--version1` | Version name of the first input file in the difference query. |
| `--version2` | Version name of the second input file in the difference query. |
| `--buffer` | Buffer size used for reading the file incrementally (must be between 250KB and 1024KB). |
| `--query` | Specifies the type of query to execute. Supported values are `word`, `top`, and `diff`. |
| `--word` | The word whose frequency is queried (used in `word` and `diff` queries). |
| `--top` | Number of most frequent words to display in the `top` query. |



## Description of Classes

| Class Name | Description |
|------------|-------------|
| `MapStore<K, V>` | A template class that stores key–value mappings using an `unordered_map`. It is used to store version names and their corresponding word-frequency indices. |
| `Base_Reader` | An abstract base class that defines the interface for reading data sources. It declares virtual functions such as `open()`, `readChunk()`, and `close()` for file reading. |
| `Buffered_Reader` | A derived class of `Base_Reader` that reads data from files using a fixed-size buffer. It supports incremental file processing without loading the entire file into memory. |
| `String_Reader` | Another derived class of `Base_Reader` used to demonstrate runtime polymorphism by reading from a string instead of a file. |
| `Base_Tokenizer` | An abstract base class responsible for defining the interface for tokenization. It declares the virtual `feed()` function used to process chunks of data. |
| `Word_Tokenizer` | A derived tokenizer class that extracts alphanumeric words from the input, converts them to lowercase for case-insensitive matching, and updates the word frequency index. |
| `Versioned_Index` | Manages the indexing system for different versions of files. It builds word-frequency indices and provides query functions for word count, top-K frequent words, and differences between versions. |
| `Query_Processor` | Handles command line queries, invokes indexing operations, executes queries, and prints the results along with execution time and buffer size. |


## Core Requirements Implemented
* **Fixed-Size Buffer :** Processes files in chunks between 256 KB and 1024 KB.
* **Word-Level Indexing :** Maps unique alphanumeric tokens to their frequency.
* **Version Management :** Supports multiple versions and comparative queries.
* **Case-Insensitivity :** All words are treated as lowercase.

## OOP Design & C++ Features
* **Inheritance :**  Uses `Base_Reader` as an abstract base class for `Buffered_Reader` and `String_Reader`.
* **Polymorphism :**  Implements runtime polymorphism via virtual functions in the reader classes.
* **Templates :**  Includes a `MapStore` class template for generic data storage.
* **Function Overloading :**  The `Word_Tokenizer` class overloads the `feed` function.
* **Exception Handling :**  Uses `try-catch` blocks for robust error management.

