# PARCOACH Microbenchmark Suite
This microbenchmark suite contains multiple MPI test case with collective issues.

## Tests Description

#### CIVL_barrierReduce.c

Collective mismatch: Processes with an even rank call the collective sequence  **{MPI_Barrier, MPI_Reduce}** whereas processes with an odd rank call the collective sequence **{MPI_Reduce, MPI_Barrier}**.

#### CIVL_barrierScatter.c

Collective mismatch: Root process call the collective sequence **{MPI_Scatter,MPI_Barrier}** whereas other processes call the collective sequence **{MPI_Barrier, MPI_Scatter}**.

#### CIVL_BcastReduce_bad.c

Collective mismatch: Processes with an even rank call the collective sequence **{MPI_Bcast, MPI_Reduce, MPI_Bcast}** whereas processes with an odd rank call the collective sequence **{MPI_Bcast, MPI_Bcast, MPI_Reduce}**.

#### field-sensitive.c

Structure with a multi-valued field: field `mype` of structure `H` is multi-valued whereas field `nproc` is single-valued and the barrier line `15` is conditionally called depending on the value of `H->nproc`.

#### index-dep.c

Use of an array: The barrier line `15` is conditionally called depending on the value of `A[r]` which is multi-valued as `r` is the process rank and each element of array `A` is assigned a different value line `12`.

#### loop_barrier.c

Barriers called in a loop. Processes with an odd rank will call `9` barriers whereas others will call `10` barriers.

#### mismatch_barrier.c

Collective mismatch: Only processes with an odd rank will call the barrier line `20`.

#### mismatch_barrier_com.c

MPI communicator mismatch.

#### mismatch_barrier_nb.c

Collective mismatch: Processes with an odd rank execute a blocking barrier whereas other processes execute a non-blocking barrier.

#### MPIexample.c

Variable `n` is assigned different values depending on the process rank and becomes multi-valued line `26`. Hence, the collective line `12` is not called by all processes because of the conditional line `26`.

#### noerror_barrier.c

Correct usage of barrier.

#### not_verifiable.c

Correct, but structurally incorrect code. Processes with an odd rank call the barrier line `20` whereas processes with an even rank call the barrier line `23`.

#### phi-cond.c

Control-flow dependence: The value of variable `v` becomes multi-valued line `24`, thus the barrier line `25` is not called by all processes.

#### pointer-alias.c

Use of aliases: Pointer `b` points to variable `a` and `a` becomes multi-valued line `13` because of the call to `MPI_Comm_rank` taking as parameter pointer `b`. Hence, the barrier line `16` is not called by
all processes.

#### pointer-instance.c

Variable `s` becomes multi-valued line `19`, hence the barrier line `22` is not called by all processes.

## Collective error detection comparison between the work in [1] and PARCOACH using our PDCG, SVF and Parfait


| Program Name            | Zhang et. al [1] | PARCOACH PDCG  | PARCOACH SVF   | PARCOACH Parfait | Comment |
| ----------------------- | ---------------- | -------------- | -------------- | ---------------- | ------- |
| field-sensitive.c       | False Positive   | False Positive | False Positive | False Positive   | None of the methods resort to a field-sensitive alias analysis, hence `H->nproc` is tagged as multi-valued because of the statement line `12` and all methods emit a warning for the barrier line `15`.
| index-dep.c             | Passed           | Passed         | False Negative | Passed           | SVF does not consider the dependency with the array index, hence `A[r]` is tagged as single-valued and no warning is emitted for the barrier line `15`.
| phi-cond.c              | Passed           | Passed         | False Negative | Passed           | SVF does not consider control dependencies. Hence it tags the instance of the variable `v` line `24` as single-valued and does not emit a warning for the barrier line `25`.
| pointer-instance.c      | Passed           | Passed         | Passed         | False Positive   | In LLVM IR `s` is an address-taken variable read and written with store and load operations on the top-level pointer `%s`. However, Parfait does not differentiate the multiple values of address-taken variables after each store instruction. Hence, Parfait tags the instance of `s` at line `18` as multi-valued because of the statement line `19` and emits a false positive warning for the barrier line `6`.
| pointer-alias.c         | False Negative   | Passed         | Passed         | False Positive   | As [1] does not handle pointer aliases, it tags the instance of `a` line `15` as single-valued and does not emit a warning for the barrier line `16`. As Parfait does not differentiate the values of address-taken variables after each store instruction, it tags the instance of `a` line `10` as multi-valued because of the statement line `13` and emits a false positive warning for the barrier line `11`.
| CIVL_barrierReduce.c    | False Negative   | Passed         | Passed         | Passed           | [1] does not support any other collective than MPI_Barrier and does not emit any warning.
| CIVL_barrierScatter.c   | False Negative   | Passed         | Passed         | Passed           | [1] does not support any other collective than MPI_Barrier and does not emit any warning.
| CIVL_BcastReduce_bad.c  |  False Negative  | Passed         | Passed         | Passed           | [1] does not support any other collective than MPI_Barrier and does not emit any warning.
| mismatch-barrier.c      | Passed           | Passed         | Passed         | Passed           |
| mismatch_barrier_com.c  | Passed           | Passed         | Passed         | Passed           |
| mismatch_barrier_nb.c   | Passed           | Passed         | Passed         | Passed           |
| MPIexample.c            | False Negative   | Passed         | False Negative | False Negative   | [1] does not support any other collective than MPI_Barrier, SVF does not consider control-flow dependencies and Parfait does not handle control-flow dependencies for address-taken variables.
| noerror_barrier.c       | Passed           | Passed         | Passed         | Passed           |
| not_verifiable.c        | False Positive   | False Positive | False Positive | False Positive   | Structurally incorrect program. All methods emit false positive warnings for the barriers lines `20` and `23`.
| loop_barrier.c          | Passed           | Passed         | Passed         | Passed           |

**[1] Zhang, Y., Duesterwald, E.: Barrier matching for programs with textually unaligned barriers. PPoPP, ACM (2007) 194–204**
