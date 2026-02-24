# Riff
Collection of header only, C utilities - macros, structures and algorithms, designed to be reused in range of future projects.

## Contents
* Macro framework for creating generic algorithms / data structures
* Dynamic Array
* Double-Linked-List
* Queue
* Hashmap

## Conventions

### 1. Safe Failure
- Functions that may fail return either:
  - `SCC` — Success  
  - `ERR` — Error  
  (Unless otherwise specified — some pointer-returning functions return `NULL` on failure.)  
- **Guarantee:** On failure, the container remains **unchanged** and **valid**.

### 2. Consistent Ownership Policy
- Riff containers **take ownership** of objects **only on successful** add/push/insert operations.  
- On failure, the **caller retains ownership** of the object.  
- Special ownership rules, if any, are documented per function.  
- All add/push/insert operations perform a **shallow copy** of the object.  
- Objects owned by a container are **automatically destroyed** when the container is destroyed.

### 3. Valid NULL / Zero States
- Riff containers are valid even when **zero-initialized**.  
- That means containers in global or static scope can be used **without prior initialization**.

### 4. Transparent Allocations
- All memory operations go through the **provided allocator**; there are no hidden `malloc`/`free` calls.  
- Each instantiation of a container can other **custom allocator**.  
- If no allocator is provided, Riff defaults to the **stdlib heap allocator**.

### 5. Iteration and Complexity Guarantees
- Time complexity of an operation is described per function - but it always is.

## State of development
Not mature yet. Still playing with core api.
