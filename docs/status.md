# STL Preview Status Dashboard

Generated from [README.md](../README.md#detailed-status) by [tools/readme_gardener.py](../tools/readme_gardener.py).

## Overall

| Scope | Implementation |
|-------|----------------|
| Total | 335/717 (47%)  |
| C++17 | 49/109 (45%)   |
| C++20 | 239/432 (55%)  |
| C++23 | 74/158 (47%)   |
| C++26 | 35/154 (23%)   |

## Headers

| Header                                          | Total        | C++17      | C++20        | C++23       | C++26       |
|-------------------------------------------------|--------------|------------|--------------|-------------|-------------|
| [algorithm](../README.md#algorithm)             | 56/115 (49%) | 2/4 (50%)  | 47/96 (49%)  | 9/18 (50%)  | 7/23 (30%)  |
| [any](../README.md#any)                         | 5/5 (100%)   | 5/5 (100%) |              |             |             |
| [array](../README.md#array)                     | 1/1 (100%)   |            | 1/1 (100%)   |             |             |
| [atomic](../README.md#atomic)                   | 0/17 (0%)    |            | 0/13 (0%)    |             | 0/4 (0%)    |
| [barrier](../README.md#barrier)                 | 0/1 (0%)     |            | 0/1 (0%)     |             |             |
| [bit](../README.md#bit)                         | 2/14 (14%)   |            | 1/13 (8%)    | 1/1 (100%)  |             |
| [charconv](../README.md#charconv)               | 0/5 (0%)     | 0/5 (0%)   |              | 0/2 (0%)    |             |
| [chrono](../README.md#chrono)                   | 0/47 (0%)    | 0/2 (0%)   | 0/47 (0%)    |             | 0/2 (0%)    |
| [cmath](../README.md#cmath)                     | 0/22 (0%)    | 0/22 (0%)  |              | 0/21 (0%)   | 0/1 (0%)    |
| [compare](../README.md#compare)                 | N/A          |            |              |             |             |
| [concepts](../README.md#concepts)               | 30/30 (100%) |            | 30/30 (100%) | 1/1 (100%)  |             |
| [coroutine](../README.md#coroutine)             | N/A          |            |              |             |             |
| [cstddef](../README.md#cstddef)                 | 2/2 (100%)   | 2/2 (100%) |              |             |             |
| [debugging](../README.md#debugging)             | 0/3 (0%)     |            |              |             | 0/3 (0%)    |
| [exception](../README.md#exception)             | 0/1 (0%)     | 0/1 (0%)   |              |             |             |
| [expected](../README.md#expected)               | 4/4 (100%)   |            |              | 4/4 (100%)  |             |
| [execution](../README.md#execution)             | N/A          |            |              |             |             |
| [flat_map](../README.md#flat_map)               | 0/4 (0%)     |            |              | 0/4 (0%)    |             |
| [flat_set](../README.md#flat_set)               | 0/2 (0%)     |            |              | 0/2 (0%)    |             |
| [format](../README.md#format)                   | 0/24 (0%)    |            | 0/16 (0%)    | 0/15 (0%)   | 0/10 (0%)   |
| [filesystem](../README.md#filesystem)           | N/A          |            |              |             |             |
| [functional](../README.md#functional)           | 16/23 (70%)  | 4/6 (67%)  | 12/14 (86%)  | 3/4 (75%)   | 4/6 (67%)   |
| [hazard_pointer](../README.md#hazard_pointer)   | 0/3 (0%)     |            |              |             | 0/3 (0%)    |
| [inplace_vector](../README.md#inplace_vector)   | 0/1 (0%)     |            |              |             | 0/1 (0%)    |
| [iterator](../README.md#iterator)               | 57/59 (97%)  | 1/3 (33%)  | 51/52 (98%)  | 5/5 (100%)  | 2/2 (100%)  |
| [latch](../README.md#latch)                     | 0/1 (0%)     |            | 0/1 (0%)     |             |             |
| [linalg](../README.md#linalg)                   | 0/51 (0%)    |            |              |             | 0/51 (0%)   |
| [mdspan](../README.md#mdspan)                   | 9/15 (60%)   |            |              | 7/7 (100%)  | 2/8 (25%)   |
| [memory](../README.md#memory)                   | 9/44 (20%)   | 4/11 (36%) | 9/35 (26%)   | 0/4 (0%)    | 0/2 (0%)    |
| [memory_resource](../README.md#memory_resource) | N/A          |            |              |             |             |
| [mutex](../README.md#mutex)                     | 0/1 (0%)     | 0/1 (0%)   |              |             |             |
| [new](../README.md#new)                         | 0/5 (0%)     | 0/4 (0%)   | 0/1 (0%)     |             |             |
| [numbers](../README.md#numbers)                 | 13/13 (100%) |            | 13/13 (100%) |             |             |
| [numeric](../README.md#numeric)                 | 1/15 (7%)    | 0/9 (0%)   | 0/4 (0%)     | 1/1 (100%)  | 0/5 (0%)    |
| [print](../README.md#print)                     | 0/6 (0%)     |            |              | 0/6 (0%)    |             |
| [optional](../README.md#optional)               | 7/7 (100%)   | 7/7 (100%) |              | 1/1 (100%)  | 1/1 (100%)  |
| [random](../README.md#random)                   | 1/1 (100%)   |            | 1/1 (100%)   |             |             |
| [ranges](../README.md#ranges)                   | 78/86 (91%)  |            | 60/60 (100%) | 30/36 (83%) | 15/17 (88%) |
| [rcu](../README.md#rcu)                         | 0/6 (0%)     |            |              |             | 0/6 (0%)    |
| [semaphore](../README.md#semaphore)             | 0/2 (0%)     |            | 0/2 (0%)     |             |             |
| [shared_mutex](../README.md#shared_mutex)       | 0/1 (0%)     | 0/1 (0%)   |              |             |             |
| [stop_token](../README.md#stop_token)           | 0/5 (0%)     |            | 0/5 (0%)     |             |             |
| [string](../README.md#string)                   | 0/3 (0%)     |            | 0/3 (0%)     | 0/1 (0%)    | 0/2 (0%)    |
| [string_view](../README.md#string_view)         | 3/3 (100%)   | 3/3 (100%) | 1/1 (100%)   | 1/1 (100%)  | 1/1 (100%)  |
| [source_location](../README.md#source_location) | N/A          |            |              |             |             |
| [syncstream](../README.md#syncstream)           | 0/3 (0%)     |            | 0/3 (0%)     |             |             |
| [span](../README.md#span)                       | 4/4 (100%)   |            | 4/4 (100%)   | 1/1 (100%)  | 1/1 (100%)  |
| [spanstream](../README.md#spanstream)           | 0/4 (0%)     |            |              | 0/4 (0%)    |             |
| [stacktrace](../README.md#stacktrace)           | 0/4 (0%)     |            |              | 0/4 (0%)    |             |
| [text_encoding](../README.md#text_encoding)     | 0/1 (0%)     |            |              |             | 0/1 (0%)    |
| [thread](../README.md#thread)                   | 0/1 (0%)     |            | 0/1 (0%)     |             |             |
| [tuple](../README.md#tuple)                     | 5/6 (83%)    | 2/2 (100%) |              | 5/6 (83%)   |             |
| [type_traits](../README.md#type_traits)         | 15/29 (52%)  | 8/10 (80%) | 7/13 (54%)   | 2/6 (33%)   | 0/2 (0%)    |
| [utility](../README.md#utility)                 | 8/8 (100%)   | 2/2 (100%) | 2/2 (100%)   | 3/3 (100%)  | 1/1 (100%)  |
| [variant](../README.md#variant)                 | 9/9 (100%)   | 9/9 (100%) |              |             | 1/1 (100%)  |
| [version](../README.md#version)                 |              |            |              |             |             |

## Review Queue

| Header          | Remaining tracked entries |
|-----------------|---------------------------|
| algorithm       | 59                        |
| atomic          | 17                        |
| barrier         | 1                         |
| bit             | 12                        |
| charconv        | 5                         |
| chrono          | 47                        |
| cmath           | 22                        |
| compare         | N/A                       |
| coroutine       | N/A                       |
| debugging       | 3                         |
| exception       | 1                         |
| execution       | N/A                       |
| flat_map        | 4                         |
| flat_set        | 2                         |
| format          | 24                        |
| filesystem      | N/A                       |
| functional      | 7                         |
| hazard_pointer  | 3                         |
| inplace_vector  | 1                         |
| iterator        | 2                         |
| latch           | 1                         |
| linalg          | 51                        |
| mdspan          | 6                         |
| memory          | 35                        |
| memory_resource | N/A                       |
| mutex           | 1                         |
| new             | 5                         |
| numeric         | 14                        |
| print           | 6                         |
| ranges          | 8                         |
| rcu             | 6                         |
| semaphore       | 2                         |
| shared_mutex    | 1                         |
| stop_token      | 5                         |
| string          | 3                         |
| source_location | N/A                       |
| syncstream      | 3                         |
| spanstream      | 4                         |
| stacktrace      | 4                         |
| text_encoding   | 1                         |
| thread          | 1                         |
| tuple           | 1                         |
| type_traits     | 14                        |
| version         | No tracked entries        |
