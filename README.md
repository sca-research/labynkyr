# Labynkyr

## Quick overview

This project contains C++11 implementations of key rank estimation and parallelisable key search/enumeration algorithms.  This consists of:

* A C++11 rank estimation library.
* A C++11 parallel key search library.
* A Java rank estimation library and runnable JAR project.  This implementation is minimal and slow.  If you're looking for performance or are trying benchmarking please use the C++ code.
* Examples of Matlab integration with the Java library.
* Examples of rank estimation and parallel search using the C++ APIs.

If you're in a hurry, check `INSTALL.md` for installation instructions and requirements.  You'll need

* A C++11 compiler
* An installation of boost (>= 1.58), unittest++ (>= 1.4.0) and CMake to build (>= 2.8.4)

The project has been built on Linux and OSX.  A Windows build should be achievable but we have not yet tried.

To use the library in your own projects, include the header files that will be placed into the `include/` directory after building.  If you're familiar with key rank and enumeration concepts, have a look around the `examples/` directory to see how to use the library.  More detailed descriptions follow below.

## Referencing

In addition to the requirements of the attached `license.txt`, if you use this code in any publication we would appreciate it if you could reference the following two publications if using the key rank estimation related code:
~~~~
@inproceedings{MartinOOS15,
  author    = {Daniel P. Martin and Jonathan F. O'Connell and Elisabeth Oswald and Martijn Stam},
  title     = {Counting Keys in Parallel After a Side Channel Attack},
  booktitle = {Advances in Cryptology - {ASIACRYPT} 2015 - 21st International Conference
               on the Theory and Application of Cryptology and Information Security,
               Auckland, New Zealand, November 29 - December 3, 2015, Proceedings,
               Part {II}},
  pages     = {313--337},
  year      = {2015},
  doi       = {10.1007/978-3-662-48800-3_13},
}
@article{MartinMOS16,
  author    = {Daniel P. Martin and Luke Mather and Elisabeth Oswald and Martijn Stam},
  title     = {Characterisation and Estimation of the Key Rank Distribution in the Context of Side Channel Evaluations},
  journal   = {{IACR} Cryptology ePrint Archive},
  volume    = {2016},
  pages     = {491},
  year      = {2016},
  url       = {http://eprint.iacr.org/2016/491},
}
~~~~
and the following publication if using the key enumeration code:
~~~~
@article{Longo0MOSS16,
  author    = {Jake Longo and Daniel P. Martin and Luke Mather and Elisabeth Oswald and Benjamin Sach and Martijn Stam},
  title     = {How low can you go? Using side-channel data to enhance brute-force key recovery},
  journal   = {{IACR} Cryptology ePrint Archive},
  volume    = {2016},
  pages     = {609},
  year      = {2016},
  url       = {http://eprint.iacr.org/2016/609},
~~~~

## Examples

The `examples/` directory contains three sets of example uses of labynkr for rank estimation and parallel key search.

The examples will be built into an executable as part of the build process.

## Key rank estimation

The following section briefly describes how programmatically run a simple rank estimation using the C++ API.  The unit tests in PathCountRankTests.cpp will also be informative.

The sample code below lists the necessary objects and function calls to estimate the rank of a 128-bit AES key, assuming the attack produced 16 separate distinguishing vectors each targeting 8 bits of the key. 
~~~~
using namespace labynkyr;

// Define the known key
Key<128> const key("000102030405060708090A0B0C0D0E0F");

// Construct a distinguishing table.  scores is a 256*16 length vector containing the distinguishing scores
// in the 1st distinguishing vector, then the 2nd, and so on.
std::vector<double> scores(256 * 16);
// ... set the scores ...
DistinguishingTable<16, 8, double> table(scores);

// Transform the scores to meet weight conversion requirements -- example is for correlation coefficients
table.takeLogarithm(2.0);
table.applyAbsoluteValue();

// Convert to integer scores
uint32_t const precisionBits = 15;
auto weightTable = table.mapToWeight<uint32_t>(precisionBits);

// Estimate rank
BigInt<128> const estimatedRank = rank::PathCountRank<16, 8, uint32_t>::rank(key, weightTable);
~~~~

## Parallel key search

The following section briefly describes how to run a parallel key search on a single host using the C++ API.

The sample code below lists the necessary objects and function calls to do an side-channel enhanced search of an unknown 128-bit AES key, assuming the side-channel attack produced 16 separate distinguishing vectors each targeting 8 bits of the key.  The example will use two threads for parallelism and will search the first 2^45 most likely key candidates.
~~~~
using namespace labynkyr;
using namespace search;

// Construct a distinguishing table.  scores is a 256*16 length vector containing the distinguishing scores
// in the 1st distinguishing vector, then the 2nd, and so on.
std::vector<double> scores(256 * 16);
// ... set the scores ...
DistinguishingTable<16, 8, double> table(scores);

// Transform the scores to meet weight conversion requirements -- example is for correlation coefficients
table.takeLogarithm(2.0);
table.applyAbsoluteValue();

// Convert to integer scores
uint32_t const precisionBits = 15;
auto weightTable = table.mapToWeight<uint32_t>(precisionBits);

// Use AES-NI to verify the key candidates using a known plaintext / ciphertext pair
std::vector<uint8_t> const plaintext = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
std::vector<uint8_t> const ciphertext = {0xc5, 0x11, 0xb3, 0xb8, 0xe8, 0x2e, 0x57, 0xac, 0x0a, 0xd3, 0x03, 0x19, 0xa7, 0x44, 0x63, 0xa6};
AES128NIEncryptUnrolledKeyVerifierFactory verifierFactory(plaintext, ciphertext);

// 1:1 mapping between two enumeration and two verifier instances
uint32_t const peuCount = 2;
uint32_t const verifierCount = 2;
PEUPool<16, 8, uint32_t, uint8_t> peuPool(peuCount, verifierFactory, verifierCount, 100UL);

// Aim to search the first 2^45 key candidates
uint32_t const budgetBits = 45;
SearchSpecBuilder<128> const searchSpecBuilder(budgetBits);
auto const searchSpec = searchSpecBuilder.createSpec();

// Allocate the required effort for the preferred task sizes of 2^34 bits
uint32_t const preferredTaskSizeBits = 34;
EffortAllocation<16, 8, uint32_t> effort(searchSpec, weightTable, preferredTaskSizeBits);

// Run the enumeration
WorkScheduler<16, 8, uint32_t, uint8_t> scheduler(100UL);
scheduler.runSearch(peuPool, effort);
		
// Check for success
bool const keyFound = peuPool.isKeyFound();
if(keyFound) {
    Key<128> const correctKey = peuPool.correctKey();
}
~~~~