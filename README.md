# Katana Occam CME4

Ultra Low Latency C++ Handler for CME iLink3 Order Routing Protocol
- Low level bindings for C++ interface to SolareFlare ef_vi library
- Many custom HFT concepts built in, including custom mapping structures and highly optimized internal data structures
- Algo Handling Capabilities
  - Algos reside in the same process as the exchange interface, dramatically reducing latency
  - Common AlgoBaseV3 class is the base class for all hosted algos
  - Parameters and command are JSON formatted strings for ease of use, modification, and debugging

This is an Eclipse project
Built with GCC 9 on Eclipse
