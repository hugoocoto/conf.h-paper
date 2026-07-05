#import "@local/ieee-paper:1.0.0": ieee;

#show: ieee.with(
  title: [Conf.h: The JQuery of Lua],
  authors: (
    (name: "Hugo Coto", email: "hugo.coto@outlook.com"),
  ),
  abstract: [In this paper we present you a new library to read Lua @lua code from
    C. The aim of this library is to make easy to use Lua configuration
    files in our projects. Thats why it's distributed as a single file, following stb @stb
    style; and the public API consist of a little set of functions that the user
    would expect. The fist thing that we have in ming at the time of take
    decisions around this library, is to help the user (i.e., the developer) to
    have a nice time and don't suffer in such a secundary task: reading
    user configurations.],
  index-terms: ("C", "Library", "Lua", "Configuration files"),
  bibliography: bibliography("refs.bib"),
  lang: "en",
)

= Introduction

= Related Work

= Design & Implementation

= Performance

@fig-loop shows the average time to access a single integer from a
9-levels-deep table using a path string (conf.h) compared to a pre-resolved
registry reference (Lua C API). The cached reference is an order of
magnitude faster, since conf.h must re-parse the full dot-separated path
and walk the hash table on every call.

#figure(
  image("loop.png", width: 100%),
  caption: [Deep-table access time: re-traverse (conf.h) vs cached ref (Lua C API)],
) <fig-loop>

To understand how the cost scales with nesting depth, @fig-nesting
measures access time at depths 0 (top-level key) through 9. Conf.h's cost
grows roughly linearly ($approx 0.2 "µs"$ per level), reflecting the
path-parsing and hash-lookup overhead of each `table` hop. Lua, using a
single registry reference per depth, stays near constant ($approx 0.1 "µs"$).

#figure(
  image("nesting.png", width: 100%),
  caption: [Access time per nesting depth for conf.h and the Lua C API],
) <fig-nesting>

@fig-speedup-nesting plots the ratio of Lua time to conf.h time per depth.
A ratio below 1 means Lua is faster. The gap widens with depth, confirming
that string-path traversal is the dominant cost for conf.h.

#figure(
  image("speedup_nesting.png", width: 100%),
  caption: [Nesting speedup ratio of conf.h over the Lua C API],
) <fig-speedup-nesting>

Finally, @fig-speedup-loop shows the per-trial speedup for the deep-table
loop. Each dot is one of the 1000 trials. The ratio fluctuates slightly
due to scheduler noise but stays consistently below 1, confirming that Lua's
cached-reference approach dominates across all runs.

#figure(
  image("speedup_loop.png", width: 100%),
  caption: [Loop speedup ratio per trial],
) <fig-speedup-loop>

=== Conclusion

The performance measurements show that conf.h's string-based path API is
2--$17 times$ slower than equivalent hand-written Lua C code using cached
registry references. This gap is expected: conf.h prioritises developer
ergonomics over raw speed, trading parse-once semantics for a convenient
dot-separated interface.

To put the numbers in perspective, consider a midsize configuration with
200 keys, averaging a depth of 3. From @fig-nesting, conf.h takes
$approx 0.8 "µs"$ per access at that depth, while Lua takes
$approx 0.1 "µs"$. The total startup cost is:

$ "conf.h:" 200 times 0.8 "µs" &= 160 "µs" \
  "Lua:"   200 times 0.1 "µs" &= 20 "µs" $

The absolute difference is $140 "µs"$ --- less than the time to
render a single frame at 60 FPS ($16.7 "ms"$). For a read-once-at-startup
workload the overhead is undetectable to the user.

What conf.h buys in return is conciseness. Reading a single key with conf.h
is one line of C:

```c
Conf_get_int(conf, &val, "server.port");
```

The equivalent Lua C API code requires a global lookup, a table traversal,
type checking, and stack management---about 8--10 lines per value,
multiplied by 200 keys yields roughly 1,600--2,000 lines of boilerplate.
Conf.h reduces this to 200 lines, making configuration parsing essentially
free in both maintenance cost and runtime overhead.

= Conclusion & Future Work
