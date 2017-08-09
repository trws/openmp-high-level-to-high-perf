//1d array, 1-1 mapping
part_copy(a[1:n])
//1d array, requires 2 neighbors from each side
part_copy(a[1:n:2])
//3x3 stencil input/output by columns
part_copy(a[0:n:1][1:m:1])
//column and row copy, persistent, size 6
part_copyin(a(1,6)[1:n][1:n]) persist(a)

