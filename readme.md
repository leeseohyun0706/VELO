# A project VELO

VELO provides many data structure for speed.

It will be developing and provides you absolutely faster experience.

It uses the template syntaxes in c++ for flexible scalability.

#### Developing enviroment

- std=c++17
- macos m1 (currently)
- llvm/clang++

---

# Benchmark

### VELO::RBSet

```
+====================+============+=================+==================+================+================+
| Benchmark          | Iterations | CPU / iter (ms) | Real / iter (ms) |    Items / sec |    Bytes / sec |
+--------------------+------------+-----------------+------------------+----------------+----------------+
| VELO::RBSet/8      |     262144 |           0.006 |            0.008 |   3.1M items/s | 200.1M bytes/s |
| VELO::RBSet/64     |      16384 |           0.059 |            0.082 |   2.3M items/s | 149.4M bytes/s |
| VELO::RBSet/512    |       2048 |           0.558 |            0.563 |   2.7M items/s | 174.6M bytes/s |
| VELO::RBSet/4096   |        256 |           5.474 |            5.513 |   2.2M items/s | 142.7M bytes/s |
| VELO::RBSet/32768  |         16 |          70.675 |           99.486 | 988.1K items/s |  63.2M bytes/s |
| VELO::RBSet/256000 |          2 |         695.952 |          701.991 |   1.1M items/s |  70.0M bytes/s |
| std::set/8         |     131072 |           0.009 |            0.012 |   1.9M items/s | 123.5M bytes/s |
| std::set/64        |      16384 |           0.107 |            0.137 |   1.4M items/s |  90.0M bytes/s |
| std::set/512       |       1024 |           1.114 |            1.133 |   1.4M items/s |  86.8M bytes/s |
| std::set/4096      |        128 |          12.519 |           25.816 | 476.0K items/s |  30.5M bytes/s |
| std::set/32768     |          4 |         152.965 |          263.814 | 372.6K items/s |  23.8M bytes/s |
| std::set/256000    |          1 |        1556.773 |         2332.672 | 329.2K items/s |  21.1M bytes/s |
+====================+============+=================+==================+================+================+
```

### VELO::HashMap

```
+===========================+============+=================+==================+==============+================+
| Benchmark                 | Iterations | CPU / iter (ms) | Real / iter (ms) |  Items / sec |    Bytes / sec |
+---------------------------+------------+-----------------+------------------+--------------+----------------+
| VELO::HashMap/8           |     262144 |           0.005 |            0.005 | 5.2M items/s | 352.1M bytes/s |
| VELO::HashMap/64          |      32768 |           0.039 |            0.039 | 4.9M items/s | 333.1M bytes/s |
| VELO::HashMap/512         |       4096 |           0.292 |            0.296 | 5.2M items/s | 352.4M bytes/s |
| VELO::HashMap/4096        |        512 |           2.289 |            2.325 | 5.3M items/s | 359.4M bytes/s |
| VELO::HashMap/32768       |         64 |          18.765 |           18.960 | 5.2M items/s | 352.6M bytes/s |
| VELO::HashMap/256000      |          8 |         199.432 |          203.097 | 3.8M items/s | 257.1M bytes/s |
| std::unordered_map/8      |     262144 |           0.008 |            0.011 | 2.2M items/s | 151.5M bytes/s |
| std::unordered_map/64     |      16384 |           0.055 |            0.074 | 2.6M items/s | 177.2M bytes/s |
| std::unordered_map/512    |       2048 |           0.452 |            0.683 | 2.2M items/s | 152.8M bytes/s |
| std::unordered_map/4096   |        512 |           3.387 |            3.441 | 3.6M items/s | 242.8M bytes/s |
| std::unordered_map/32768  |         64 |          29.379 |           29.765 | 3.3M items/s | 224.6M bytes/s |
| std::unordered_map/256000 |          4 |         313.079 |          315.747 | 2.4M items/s | 165.4M bytes/s |
+===========================+============+=================+==================+==============+================+
```
